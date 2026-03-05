#pragma once

#include "server/object_base.h"
#include <filesystem>

typedef std::filesystem::path FSPath;
typedef std::string FString;
struct FSizeUnit;

class FStreamBase : public ObjectBase{
private:
    OID m_parent = -100;
    FSPath m_path;
public:
    FStreamBase();
    ~FStreamBase();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
    
    void set_path(FSPath p_path);
    void set_parent(OID p_parent);

    FSPath get_path();
    
    bool is_exists();
    bool is_directory();
    bool is_file();
    bool is_hidden();
    
    void set_hidden();

    virtual FSizeUnit get_size();
    
    FString get_name();

    virtual void remove(){}
};