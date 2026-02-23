#include "system/obj/fstream/file.h"
#include "DecToolsBox/debug/messenger.h"
#include "boost/interprocess/sync/file_lock.hpp"
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
    bool is_relocked_needed = false;
    if(this->is_locked()){
        this->unlocked();
        is_relocked_needed = true;
    }

    std::ofstream file(this->get_path(), std::ios::out);
    file.close();

    if(is_relocked_needed){
        this->locked();
    }
}
void FStreamFile::append(FString p_text){
    bool is_relocked_needed = false;
    if(this->is_locked()){
        this->unlocked();
        is_relocked_needed = true;
    }

    std::ofstream file(this->get_path(), std::ios::app);
    if(file.is_open()){
        file << p_text;
        file.close();
    }

    if(is_relocked_needed){
        this->locked();
    }
}
FString FStreamFile::read(){
    FString ret;

    bool is_relocked_needed = false;
    if(this->is_locked()){
        this->unlocked();
        is_relocked_needed = true;
    }

    std::ofstream file(this->get_path(), std::ios::in);
    if(file.is_open()){
        std::ostringstream ss;
        ss << file.rdbuf();
        ret = ss.str();
        file.close();
    }

    if(is_relocked_needed){
        this->locked();
    }

    return ret;
}

void FStreamFile::remove(){
    if(this->is_locked()){
        this->unlocked();
    }

    std::filesystem::remove(this->get_path());
    this->queue_free();
}

bool FStreamFile::is_locked(){
    return m_is_locked;
}

void FStreamFile::locked(){
    if(is_locked()){
        return;
    }

    m_flock = boost::interprocess::file_lock(this->get_path().string().c_str());
    m_flock.lock();

    m_is_locked = true;
}

void FStreamFile::unlocked(){
    if(!is_locked()){
        return;
    }

    m_flock.unlock();

    m_is_locked = false;
}