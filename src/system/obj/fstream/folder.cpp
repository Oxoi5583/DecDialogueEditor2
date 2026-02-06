#include "system/obj/fstream/folder.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/object_server.h"
#include "system/obj/fstream/base.h"
#include "system/obj/fstream/file.h"
#include "server/file_server.h"
#include <filesystem>
#include <fstream>
#include <ios>
#include <set>
#include <vector>

FStreamFolder::FStreamFolder(){
    BIND_CLASS(FStreamFolder);
}
void FStreamFolder::generate_children_objs(){
    if(!this->is_exists()){
        return;
    }
    if(m_is_first_generate_done){
        return;
    }

    for(auto& p : std::filesystem::directory_iterator(this->get_path())){
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
}
void FStreamFolder::remove(){
    this->clear();
    std::filesystem::remove(this->get_path());
    this->queue_free();
}