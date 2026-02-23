#include "system/obj/fstream/folder.h"
#include "DecToolsBox/debug/messenger.h"
#include "SimZip.h"
#include "server/object_server.h"
#include "system/obj/fstream/base.h"
#include "system/obj/fstream/file.h"
#include "server/file_server.h"
#include <filesystem>
#include <fstream>
#include <ios>
#include <mfobjects.h>
#include <set>
#include <vector>

FStreamFolder::FStreamFolder(){
    BIND_CLASS(FStreamFolder);
}
void FStreamFolder::generate_children_objs(){
    if(!this->is_exists()){
        return;
    }

    std::set<FString> dir = this->dir_to_set(DirMode::FILES_AND_FOLDERS);

    for(auto& p : std::filesystem::directory_iterator(this->get_path())){
        if(dir.contains(p.path().filename().string())){
           continue;
        }
        if(std::filesystem::is_directory(p)){
            FStreamFolder* ptr = ObjectServer::Ref()->queue_create<FStreamFolder>();
            ptr->set_path(p);
            ptr->set_parent(this->get_id());
            ptr->generate_children_objs();
            this->children.emplace(ptr->get_id());
        }else{
            FStreamFile* ptr = ObjectServer::Ref()->queue_create<FStreamFile>();
            ptr->set_path(p);
            ptr->set_parent(this->get_id());
            this->children.emplace(ptr->get_id());
        }
    }
    m_is_first_generate_done = true;
}

FStreamFolder::~FStreamFolder(){

}

void FStreamFolder::m_clear_garbage(){
    std::vector<OID> child_to_erase;
    for(OID id : children){
        if(!ObjectServer::Ref()->is_id_valid(id)){
            child_to_erase.push_back(id);
        }
    }
    for(OID id : child_to_erase){
        children.erase(id);
    }
}

void FStreamFolder::ready(){

}
void FStreamFolder::pre_process(){
    m_clear_garbage();
}
void FStreamFolder::process(){

}
void FStreamFolder::post_process(){

}
void FStreamFolder::draw(){

}

FSizeUnit FStreamFolder::get_size(){
    uintmax_t total = 0;
    for(auto& p : std::filesystem::recursive_directory_iterator(get_path())){
        if(std::filesystem::is_regular_file(p)){
            total += std::filesystem::file_size(p);
        }
    }
    return FSizeUnit(total);
}

OID FStreamFolder::create_file(FString p_name){
    if(this->is_file_exists(p_name)){
        return -1;
    }

    std::filesystem::path new_path = get_path();
    new_path.append(p_name);
    if(std::filesystem::exists(new_path)){
        return -100;
    }

    std::ofstream file(new_path, std::ios::out);
    file.close();

    FStreamFile* file_ptr = ObjectServer::Ref()->queue_create<FStreamFile>();
    OID file_id = file_ptr->get_id();
    file_ptr->set_path(new_path);
    children.emplace(file_id);

    return file_id;
}
OID FStreamFolder::create_folder(FString p_name){
    if(this->is_folder_exists(p_name)){
        return -1;
    }

    std::filesystem::path new_path = get_path();
    new_path.append(p_name);
    if(std::filesystem::exists(new_path)){
        return -100;
    }

    std::filesystem::create_directory(new_path);

    FStreamFolder* folder_ptr = ObjectServer::Ref()->queue_create<FStreamFolder>();
    OID folder_id = folder_ptr->get_id();
    folder_ptr->set_path(new_path);
    children.emplace(folder_id);
    
    return folder_id;
}

std::set<OID> FStreamFolder::get_children(){
    return children;
}

void FStreamFolder::clear(){
    for(OID id : children){
        FStreamBase* base = ObjectServer::Ref()->get_instance<FStreamBase>(id);
        if(base->is_directory()){
            FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(id);
            folder->remove();
        }
        if(base->is_file()){
            FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(id);
            file->remove();
        }
    }
    children.clear();
}

void FStreamFolder::remove(){
    this->clear();
    std::filesystem::remove(this->get_path());
    this->queue_free();
}

bool FStreamFolder::is_folder_exists(FString p_name){
    FSPath this_path = this->get_path();
    this_path.append(p_name);
    return (std::filesystem::exists(this_path) && std::filesystem::is_directory(this_path));
}
bool FStreamFolder::is_file_exists(FString p_name){
    FSPath this_path = this->get_path();
    this_path.append(p_name);
    return (std::filesystem::exists(this_path) && std::filesystem::is_regular_file(this_path));
}

std::vector<FString> FStreamFolder::dir_to_vector(DirMode p_mode){
    std::vector<FString> ret;
    switch (p_mode) {
        case FILES:{
            for(OID id : children){
                FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(id);
                if(file){
                    ret.push_back(file->get_name());
                }
            }
            break;
        }
        case FOLDERS:{
            for(OID id : children){
                FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(id);
                if(folder){
                    ret.push_back(folder->get_name());
                }
            }
            break;
        }
        case FILES_AND_FOLDERS:{
            for(OID id : children){
                FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(id);
                if(folder){
                    ret.push_back(folder->get_name());
                }
                FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(id);
                if(file){
                    ret.push_back(file->get_name());
                }
            }
            break;
        }
    }
    return ret;
}

std::set<FString> FStreamFolder::dir_to_set(DirMode p_mode){
    std::set<FString> ret;
    switch (p_mode) {
        case FILES:{
            for(OID id : children){
                FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(id);
                if(file){
                    ret.emplace(file->get_name());
                }
            }
            break;
        }
        case FOLDERS:{
            for(OID id : children){
                FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(id);
                if(folder){
                    ret.emplace(folder->get_name());
                }
            }
            break;
        }
        case FILES_AND_FOLDERS:{
            for(OID id : children){
                FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(id);
                if(folder){
                    ret.emplace(folder->get_name());
                }
                FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(id);
                if(file){
                    ret.emplace(file->get_name());
                }
            }
            break;
        }
    }
    return ret;
}

OID FStreamFolder::get_child(FString p_name){
    for(OID id : children){
        FStreamBase* obj = ObjectServer::Ref()->get_instance<FStreamBase>(id);
        if(obj){
            if(obj->get_name() == p_name){
                return id;
            }
        }
    }
    return -1;
}
OID FStreamFolder::get_file(FString p_name){
    for(OID id : children){
        FStreamFile* obj = ObjectServer::Ref()->get_instance<FStreamFile>(id);
        if(obj){
            if(obj->get_name() == p_name){
                return id;
            }
        }
    }
    return -1;
}
OID FStreamFolder::get_folder(FString p_name){
    for(OID id : children){
        FStreamFolder* obj = ObjectServer::Ref()->get_instance<FStreamFolder>(id);
        if(obj){
            if(obj->get_name() == p_name){
                return id;
            }
        }
    }
    return -1;
}
void FStreamFolder::extract_from(FString p_path){
    FSPath path = std::filesystem::absolute(p_path);
    if(!std::filesystem::exists(path)){
        return;
    }

    if(!std::filesystem::is_regular_file(path)){
        return;
    }
    
    SimZip zip(path.string());
    zip.setmode(SimZip::OpenMode::Read);
    zip.extractall(this->get_path().string());
    zip.close();

    generate_children_objs();
}