#include "server/file_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "nlohmann/json.hpp"
#include "server/object_server.h"
#include "server/object_base.h"
#include "system/obj/fstream/base.h"
#include "system/obj/fstream/file.h"
#include "system/obj/fstream/folder.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace std {
    string to_string(FSizeUnit p_size){
        return p_size.to_string();
    }
}

void FSizeUnit::update_type(){
    if((value / BYTES_PER_PETABYTE) >= 0.1){
        type = PETABYTE;
        return;
    }
    if((value / BYTES_PER_TERABYTE) >= 0.1){
        type = TERABYTE;
        return;
    }
    if((value / BYTES_PER_GIGABYTE) >= 0.1){
        type = GIGABYTE;
        return;
    }
    if((value / BYTES_PER_MEGABYTE) >= 0.1){
        type = MEGABYTE;
        return;
    }
    if((value / BYTES_PER_KILOBYTE) >= 0.1){
        type = KILOBYTE;
        return;
    }
    type = BYTE;
}
double FSizeUnit::get(){
    update_type();
    switch (type) {
        case PETABYTE:{
            return value / BYTES_PER_PETABYTE;
        }
        case TERABYTE:{
            return value / BYTES_PER_TERABYTE;
        }
        case GIGABYTE:{
            return value / BYTES_PER_GIGABYTE;
        }
        case MEGABYTE:{
            return value / BYTES_PER_MEGABYTE;
        }
        case KILOBYTE:{
            return value / BYTES_PER_KILOBYTE;
        }
        case BYTE:{
            return value;
        }
    }
}
std::string FSizeUnit::to_string(){
    update_type();
    switch (type) {
        case PETABYTE:{
            return std::to_string(value) + " PB";
        }
        case TERABYTE:{
            return std::to_string(value) + " TB";
        }
        case GIGABYTE:{
            return std::to_string(value) + " GB";
        }
        case MEGABYTE:{
            return std::to_string(value) + " MB";
        }
        case KILOBYTE:{
            return std::to_string(value) + " KB";
        }
        case BYTE:{
            return std::to_string(value) + " B";
        }
    }
}


void FileServer::init(){
    std::filesystem::path root_path("./");
    std::filesystem::path abs_root_path = std::filesystem::absolute(root_path);

    m_root_ptr = ObjectServer::Ref()->queue_create<FStreamFolder>();
    m_root_ptr->set_path(abs_root_path);
    m_root_ptr->generate_children_objs();

    m_rebuild_tree();

    for(NeededFolder& folder : m_needed_folder){
        FStreamLink& link = folder.link;
        bool need_hidden = folder.need_hidden;

        OID id = get_fstream_obj(link);
        if(!ObjectServer::Ref()->is_id_valid(id)){
            FString msg = "Folder (./";
            for(FString part : link){
                msg += part;
                msg += "/";
            }
            msg += ") not found!";
            ERROR_MSG(msg);
            exit(-1);
        }else{
            FStreamBase* folder_obj = ObjectServer::Ref()->get_instance<FStreamBase>(id);
            if(need_hidden && !folder_obj->is_hidden()){
                folder_obj->set_hidden();
            }
        }
    }


};

void FStreamNode::build_tree(){
    if(!ObjectServer::Ref()->is_id_valid(this->id)){
        return;
    }

    FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(this->id);
    if(!folder){
        return;
    }

    std::set<OID> folder_children = folder->get_children();
    for(OID child_id : folder_children){
        if(!ObjectServer::Ref()->is_id_valid(child_id)){
            continue;
        }

        FStreamBase* base_ptr = ObjectServer::Ref()->get_instance<FStreamBase>(child_id);
        FString name = base_ptr->get_name();
        bool is_dir = base_ptr->is_directory();

        FStreamNode node;
        node.id = child_id;
        this->children.emplace(name ,node);
        if(is_dir){
            this->children[name].build_tree();
        }
    }
}

void FileServer::m_rebuild_tree(){
   m_root = FStreamNode();
   m_root.id = m_root_ptr->get_id();
   m_root.build_tree();
}

void FileServer::process(){
    m_rebuild_tree();
}

OID FileServer::get_fstream_obj(FStreamLink p_link){
    FStreamNode* node = &m_root;
    for(FString l : p_link){
        if(!node->children.contains(l)){
            return -100;
        }

        node = &node->children[l];
    }
    return node->id;
}


#ifdef _WIN32
#define _AMD64_
#include <windows.h>
#include <minwindef.h>
#include <fileapi.h>
#endif

bool FileServer::is_file_hidden(FPath p_path){
    #ifdef _WIN32
        DWORD attributes = GetFileAttributes(p_path.string().c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES) {
            return (attributes & FILE_ATTRIBUTE_HIDDEN) != 0;
        }
        return false;
    #else
        fs::path::string_type name = p.filename();
        if (name != "." && name != ".." && !name.empty() && name[0] == '.') {
            return true;
        }
        return false;
    #endif
}
bool FileServer::is_file_exists(FPath p_path){
    return std::filesystem::exists(p_path);
}

void FileServer::set_file_to_hidden(FPath p_path){
    #ifdef _WIN32
        if(SetFileAttributesA(p_path.string().c_str(), FILE_ATTRIBUTE_HIDDEN)){
            SUCCESS_MSG(p_path.string() << " have set to hidden.");
        }else{
            ERROR_MSG(p_path.string() << " failed to set hidden.");
        }
    #else
        try{
            std::string path_only = p_path.parent_path().string();
            std::string file_name = p_path.filename().string();
            if(!file_name.starts_with(".")){
                std::filesystem::path new_path(path_only + "." + file_name);
                std::filesystem::rename(p_path, new_path);
            }
            
            SUCCESS_MSG(p_path.string() << " have set to hidden.");
        }catch(std::filesystem::filesystem_error const& ex){
            ERROR_MSG(p_path.string() << " failed to set hidden.");
        }
    #endif
}