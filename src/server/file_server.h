#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/object_base.h"
#include "system/obj/fstream/folder.h"
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>
#include <string>

typedef std::filesystem::path FPath;
typedef std::string FString;

namespace std{
    string to_string(FSizeUnit& p_size);
}

struct FStreamNode{
    std::map<FString, FStreamNode> children;
    OID id;
    
    void build_tree();
};

struct FSizeUnit{
    enum Type{
        PETABYTE,
        TERABYTE,
        GIGABYTE,
        MEGABYTE,
        KILOBYTE,
        BYTE,
    };

    static constexpr const long double BYTES_PER_PETABYTE = 1125899906842624.0L;
    static constexpr const long double BYTES_PER_TERABYTE = 1000000000000;
    static constexpr const long double BYTES_PER_GIGABYTE = 1000000000;
    static constexpr const long double BYTES_PER_MEGABYTE = 1000000;
    static constexpr const long double BYTES_PER_KILOBYTE = 1000;

    uintmax_t value;
    Type type;
    
    operator float(){
        return (float)get();
    }
    operator double(){
        return get();
    }

    double get();
    std::string to_string();
    void update_type();
};

typedef std::vector<FString> FStreamLink;

class FileServer : public Singleton<FileServer>{
private:
    struct NeededFolder{
        FStreamLink link;
        bool need_hidden;

        NeededFolder(std::initializer_list<FString> p_link, bool p_need_hidden)
            : link(p_link), need_hidden(p_need_hidden) {}
    };

    FStreamNode m_root;
    std::vector<NeededFolder> m_needed_folder;
    FStreamFolder* m_root_ptr = nullptr;
    void m_rebuild_tree();

public:
    FileServer(){
        m_needed_folder.push_back(NeededFolder({FString("assets")}, false));
        m_needed_folder.push_back(NeededFolder({FString("config")}, false));
        m_needed_folder.push_back(NeededFolder({FString("fonts")}, false));
        m_needed_folder.push_back(NeededFolder({FString("theme")}, false));
        m_needed_folder.push_back(NeededFolder({FString(".temp")}, true));
    }
    ~FileServer() = default;

    void init();
    void process();

    OID get_fstream_obj(FStreamLink p_link);
    bool is_file_hidden(FPath p_path);
    bool is_file_exists(FPath p_path);
    bool is_in_programme_folder(FPath p_path);

    void set_file_to_hidden(FPath p_path);
};

/*
Should lock every controled files
Which have inserted into the files tree
When everytime it craet a FStream file
*/