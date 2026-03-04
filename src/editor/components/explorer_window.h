#pragma once

#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include <DecToolsBox/abstract/singleton.h>
#include <filesystem>
#include <imgui.h>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include "DecToolsBox/core/random_code.h"

class ExplorerWindowDataPipeline : public Singleton<ExplorerWindowDataPipeline>{
public:
    std::unordered_set<std::string> opened_folder = {};
    std::string dir_path = ".";
};

class ExplorerWindow : public ObjectBase{
private:
    struct FObjectRow{
        int index;
        std::filesystem::path abs_path;
        std::string str_path;
        std::string name;
        std::string display_name;
        bool is_hidden;
        bool is_directory;
        bool is_opened;
        std::vector<FObjectRow> children;
    };

    struct Page{
        static constexpr int size = 25;
        int from = 1;
        int to = size;
        
        int min;
        int max;

        void last_page();
        void next_page();
        void reset();
        bool is_in_page(int p_index);
        bool is_at_first_page();
        bool is_at_last_page();
    };

    Page page;

    std::string m_uid = RandomCode(25).get();
    ImDrawList* m_draw_list = nullptr;
    std::string m_selected_path = "";

    std::vector<FObjectRow> m_display_list;

    void m_generate_display_list();
    void m_close_button_process();

    FObjectRow m_main_row;
    FObjectRow m_sub_row;

    glm::vec2 m_engine_win_size;
    glm::vec2 m_win_size;
    glm::vec2 m_win_pos;
    glm::vec2 m_window_size;
    glm::vec2 m_explorer_area_size;
    glm::vec2 m_explorer_area_pos;

    void m_update_render_data();
    void m_update_opened_folder();
    void m_explorer_area_process();
    void m_explorer_area_process_draw_area_background();
    void m_explorer_area_process_create_main_selectable();
    void m_explorer_area_process_draw_split_line();
    void m_explorer_area_process_create_sub_selectable();
    void m_explorer_area_process_draw_folder_line();

    bool m_is_refresh_needed = true;

    void m_go_to_dir(std::filesystem::path p_path);
public:
    ExplorerWindow();
    ~ExplorerWindow();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};
