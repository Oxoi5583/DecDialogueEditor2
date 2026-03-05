#include "system/obj/fstream/base.h"
#include "server/object_server.h"
#include <filesystem>
#include "server/file_server.h"

FStreamBase::FStreamBase(){
    BIND_CLASS(FStreamBase);
}
FStreamBase::~FStreamBase(){

}

void FStreamBase::ready(){

}
void FStreamBase::pre_process(){
}
void FStreamBase::process(){

}
void FStreamBase::post_process(){

}
void FStreamBase::draw(){

}

FSPath FStreamBase::get_path(){
    return m_path;
}
void FStreamBase::set_path(FSPath p_path){
    m_path = p_path;
}
void FStreamBase::set_parent(OID p_parent){
    m_parent = p_parent;
}

bool FStreamBase::is_exists(){
    return std::filesystem::exists(m_path);
}
bool FStreamBase::is_directory(){
    return std::filesystem::is_directory(m_path);
}
bool FStreamBase::is_file(){
    return std::filesystem::is_regular_file(m_path);
}
FSizeUnit FStreamBase::get_size(){
    return FSizeUnit(std::filesystem::file_size(m_path));
}

FString FStreamBase::get_name(){
    std::filesystem::path name = m_path;
    return name.filename().string();
}

bool FStreamBase::is_hidden(){
    return FileServer::Ref()->is_file_hidden(this->m_path);
}

void FStreamBase::set_hidden(){
    FileServer::Ref()->set_file_to_hidden(this->m_path);
}