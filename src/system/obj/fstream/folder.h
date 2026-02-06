#pragma once

#include "system/obj/fstream/base.h"
#include <set>
#include <string>

typedef std::string FString;

class FStreamFolder : public FStreamBase{
private:
    bool m_is_first_generate_done = false;

    std::set<OID> children;

    void m_clear_garbage();
public:
    FStreamFolder();
    ~FStreamFolder();

    void generate_children_objs();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    std::set<OID> get_children();

    FSizeUnit get_size() override;

    OID create_file(FString p_name);
    OID create_folder(FString p_name);

    void clear();
    void remove() override;
};