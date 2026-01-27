#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/debug/messenger.h"
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <unordered_map>
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
    };

    uintmax_t value;
    Type type;
    
    double get();
};


struct FPathWrapper{
    FPath path;
    FPathWrapper* parent;
    std::map<FString ,FPathWrapper> children;

    void build_tree();
    void remove(FString p_target = "");
    void clear();
    
    void create_dir(FString p_dir);
    void create_file(FString p_name);

    void append_text(FString p_text);
    void truncate_text();

    FString get_name();
    FString get_extension();
    FString get_content();
    nlohmann::json get_json();
    uintmax_t get_size();

    bool is_directory();
    bool is_file();
    bool is_json();

    bool has_child();
    
    bool contains(FString p_path);
    FPathWrapper& operator[](FString p_index);
};

class FileServer : public Singleton<FileServer>{
private:
    FPathWrapper m_root = {std::filesystem::absolute("./"), nullptr, {}};

    std::vector<FString> m_needed_folder = {
        "assets",
        "config",
        "fonts",
        "theme",
        "projects",
        "projects/temp"
    };
    void m_folder_checking();
public:
    void init();
    void refresh_tree();

    FPathWrapper& get_root();
};