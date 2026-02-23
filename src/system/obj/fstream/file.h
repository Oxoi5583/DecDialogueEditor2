#pragma once

#include "system/obj/fstream/base.h"
#include <fstream>

#include "boost/interprocess/sync/file_lock.hpp"

typedef std::string FString;

class FStreamFile : public FStreamBase{
private:
    bool m_is_locked = false;
    boost::interprocess::file_lock m_flock;

public:
    FStreamFile();
    ~FStreamFile();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    FSizeUnit get_size() override;

    void truncate();
    void append(FString p_text);
    FString read();

    bool is_locked();
    void locked();
    void unlocked();

    void remove() override;
};