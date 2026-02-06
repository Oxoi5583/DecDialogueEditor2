#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/object_base.h"
#include "system/obj/fstream/folder.h"
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <nlohmann/json.hpp>

typedef std::filesystem::path FPath;
typedef std::string FString;

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

namespace std{
    string to_string(FSizeUnit p_size);
}

struct FStreamNode{
    OID id;
    std::map<FString, FStreamNode> children;
    
    void build_tree();
};

typedef std::vector<FString> FStreamLink;

class FileServer : public Singleton<FileServer>{
private:
    std::vector<FStreamLink> m_needed_folder = {
        {"assets"},
        {"config"},
        {"fonts"},
        {"theme"},
        {"projects"},
        {"projects" ,"temp"}
    };

    FStreamFolder* m_root_ptr;
    FStreamNode m_root;
    void m_rebuild_tree();

public:
    void init();
    void process();

    OID get_fstream_obj(FStreamLink p_link);
};