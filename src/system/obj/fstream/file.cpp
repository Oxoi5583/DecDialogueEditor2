#include "system/obj/fstream/file.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/object_server.h"
#include "server/file_server.h"
#include <filesystem>
#include <fstream>
#include <ios>

FStreamFile::FStreamFile(){
    BIND_CLASS(FStreamFile);
}
FStreamFile::~FStreamFile(){

}

void FStreamFile::ready(){

}
void FStreamFile::pre_process(){
    
}
void FStreamFile::process(){

}
void FStreamFile::post_process(){

}
void FStreamFile::draw(){

}

FSizeUnit FStreamFile::get_size(){
    return FSizeUnit(std::filesystem::file_size(get_path()));
}

void FStreamFile::truncate(){
    std::ofstream file(this->get_path(), std::ios::out);
    file.close();
}
void FStreamFile::append(FString p_text){
    std::ofstream file(this->get_path(), std::ios::app);
    if(file.is_open()){
        file << p_text;
        file.close();
    }
}

void FStreamFile::remove(){
    std::filesystem::remove(this->get_path());
    this->queue_free();
}