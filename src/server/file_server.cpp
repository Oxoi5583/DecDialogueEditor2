#include "server/file_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "nlohmann/json.hpp"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <memory>
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

FPathWrapper& FPathWrapper::operator[](FString p_index) {
    if (!children.contains(p_index)) {
        ERROR_MSG("Child path not found.");
        for(auto& it : children){
            DEBUG_MSG("child : " << it.first);
        }
        assert(false); 
    }
    return children[p_index];
}

void FPathWrapper::update_last_write_epoch(){
    long long last_write_time_epoch = std::filesystem::last_write_time(this->path).time_since_epoch().count();
    this->last_write_time = last_write_time_epoch;
}

void FPathWrapper::build_tree(){
    this->children.clear();

    for(auto& f : std::filesystem::directory_iterator(path)){
        long long last_write_time_epoch = std::filesystem::last_write_time(f.path()).time_since_epoch().count();
        FPathWrapper wrapper = {f.path(), this, last_write_time_epoch, {}, {}};
        FString name = wrapper.get_name();
        this->children.emplace(name, wrapper);
        if(std::filesystem::is_directory(f.path())){
            this->children[name].build_tree();
        }
    }
}
FString FPathWrapper::get_name(){
    return this->path.filename().string();
}
FString FPathWrapper::get_extension(){
    return this->path.extension().string();
}
FString FPathWrapper::get_content(){
    FString ret = "";
    if(!is_file()){
        return ret;
    }
    
    std::ifstream file(this->path, std::ios::binary | std::ios::ate);
    if(!file){
        return ret;
    }

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    ret.resize(static_cast<size_t>(size));

    if(size > 0){
        file.read(&ret[0], size);
    }

    return ret;
}
nlohmann::json FPathWrapper::get_json(){
    if(!is_json()){
        return nlohmann::json();
    }

    return nlohmann::json::parse(this->get_content());
}
uintmax_t FPathWrapper::get_size(){
    if(std::filesystem::is_directory(this->path)){
        uintmax_t ret = 0;
        for(auto& f : std::filesystem::recursive_directory_iterator(this->path)){
            if(f.is_regular_file() && !f.is_symlink()){
                ret += std::filesystem::file_size(f.path());
            }
        }
        return ret;
    }else if(std::filesystem::is_regular_file(this->path)){
        return std::filesystem::file_size(this->path);
    }else{
        return 0;
    }
}

bool FPathWrapper::has_child(){
    return !children.empty();
}


void FPathWrapper::remove(FString p_target){
    if(p_target == ""){
        if(std::filesystem::exists(path)){
            std::filesystem::remove(this->path);
        }

        FString name = parent->get_name();
        if(parent && parent->children.contains(name)){
            parent->children.erase(name);
        }
    }else{
        if(this->children.contains(p_target)){
            FPathWrapper& target_path_wrapper = this->children[p_target];
            FPath target_path = target_path_wrapper.path;
            target_path.append(p_target);

            target_path_wrapper.clear();
            std::filesystem::remove(target_path);
        }
    }

    this->build_tree();
    this->run_modified_callback();
    this->update_last_write_epoch();
}
void FPathWrapper::clear(){
    if(std::filesystem::is_directory(this->path)){
        FPath target_path = this->path;
        FString target_name = target_path.filename().string();

        if(std::filesystem::exists(target_path)){
            if(std::filesystem::is_directory(target_path)){
                for(auto& f : std::filesystem::recursive_directory_iterator(target_path)){
                    if(!std::filesystem::is_directory(f.path())){
                        std::filesystem::remove(f.path());
                    }
                }
                for(auto& f : std::filesystem::recursive_directory_iterator(target_path)){
                    if(std::filesystem::is_directory(f.path())
                    && std::filesystem::is_empty(f.path())){
                        std::filesystem::remove(f.path());
                    }
                }
            }
        }
        this->children.clear();
    }else{
        this->truncate_text();
    }
    this->run_modified_callback();
    this->update_last_write_epoch();
}

bool FPathWrapper::contains(FString p_path){
    return children.contains(p_path);
}

void FPathWrapper::create_dir(FString p_dir){
    FPath new_path = path;
    new_path.append(p_dir);
    if(std::filesystem::is_directory(path)
    && !std::filesystem::exists(new_path)){
        std::filesystem::create_directory(new_path);
    }
    
    this->build_tree();
    this->run_modified_callback();
    this->update_last_write_epoch();
}
void FPathWrapper::create_file(FString p_name){
    FPath new_file_path = this->path;
    new_file_path.append(p_name);

    if(std::filesystem::is_directory(this->path)
    && !std::filesystem::exists(new_file_path)){
        std::fstream file(new_file_path.string(), std::ios::out | std::ios::trunc);
        file.close();
    }else{
        INFO_MSG("Attempt to create file but file existed.");
    }

    this->build_tree();
    this->run_modified_callback();
    this->update_last_write_epoch();
}

void FPathWrapper::append_text(FString p_text){
    if(std::filesystem::is_regular_file(this->path)){
        std::ofstream file;
        file.open(this->path, std::ios::app);
        if(file.is_open()){
            file << p_text;
            file.close();
        }
    }
    this->run_modified_callback();
    this->update_last_write_epoch();
}
void FPathWrapper::truncate_text(){
    if(std::filesystem::is_regular_file(this->path)){
        std::ofstream file;
        file.open(this->path, std::ios::out | std::ios::trunc);
        if(file.is_open()){
            file.close();
        }
    }
    this->run_modified_callback();
}
void FPathWrapper::duplicate(std::string p_target, std::string p_name){
    std::filesystem::path from = std::filesystem::path(this->path).append(p_target);
    std::filesystem::path dist = std::filesystem::path(this->path).append(p_name);
    if(!std::filesystem::is_directory(this->path)){
        return;
    }
    if(!std::filesystem::exists(from)){
        return;
    }
    if(!std::filesystem::exists(dist)){
        std::filesystem::create_directory(dist);
    }

    try{
        std::filesystem::copy(from, dist, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        this->build_tree();
    } catch (std::filesystem::filesystem_error& e){
        ERROR_MSG("Error copy file : " << e.what());
    }
}
void FPathWrapper::run_modified_callback(){
    for(auto& callback : modified_callback){
        callback();
    }
    if(parent){
        parent->run_modified_callback();
    }
}
void FPathWrapper::add_modified_callback(std::function<void()> p_callback){
    modified_callback.push_back(p_callback);
}
#include <nlohmann/json.hpp>
bool FPathWrapper::is_directory(){
    return std::filesystem::is_directory(this->path);
}
bool FPathWrapper::is_file(){
    return std::filesystem::is_regular_file(this->path);
}
bool FPathWrapper::is_json(){
    if(!this->is_file()){
        return false;
    }

    FString content = this->get_content();
    return nlohmann::json::accept(content);
}

void FileServer::m_folder_checking(){
    for(FString& folder : m_needed_folder){
        if(!std::filesystem::exists(folder)){
            m_root.create_dir(folder);
            ERROR_MSG("System folder [" << folder << "] not exists. Created a new one.");
        }
    }
}


void FileServer::init(){
    this->refresh_tree();
    m_folder_checking();
}
void FileServer::refresh_tree(){
    m_root.build_tree();
}

FPathWrapper& FileServer::get_root(){
    return m_root;
}