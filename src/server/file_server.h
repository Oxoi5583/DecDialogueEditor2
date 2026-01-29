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

struct FPathWrapper{
    FPath path;
    FPathWrapper* parent;
    long long last_write_time;
    std::map<FString ,FPathWrapper> children;
    std::vector<std::function<void()>> modified_callback;

    void build_tree();
    void remove(FString p_target = "");
    void clear();

    void create_dir(FString p_dir);
    void create_file(FString p_name);

    void append_text(FString p_text);
    void truncate_text();

    void run_modified_callback();
    void add_modified_callback(std::function<void()> p_callback);

    void duplicate(std::string p_target, std::string p_name);

    void update_last_write_epoch();

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