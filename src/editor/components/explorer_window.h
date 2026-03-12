#pragma once

#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include <DecToolsBox/abstract/singleton.h>
#include <filesystem>
#include <imgui.h>
#include <mutex>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>
#include "DecToolsBox/core/random_code.h"

class ExplorerWindowDataPipeline : public Singleton<ExplorerWindowDataPipeline>{
public:
    std::unordered_set<std::string> opened_folder = {};
    std::string dir_path = std::filesystem::absolute(std::filesystem::path(".")).string();
    std::set<OID> all_explorer_windows;

    std::string selected_path;
};

class ExplorerWindow : public ObjectBase{
public:
    enum ExplorerMode{
        NORAML,
        FOLDER_ONLY,
    };

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
        bool is_expand_row;
        std::vector<FObjectRow> children;
    };

    struct Page{
        private:
            static constexpr int size = 25;
            int from = 1;
            int to = size;
            
            int min;
            int max;

            std::mutex mtx;

        public:
            void last_page();
            void next_page();
            void reset();
            bool is_in_page(int p_index) const;
            bool is_at_first_page() const;
            bool is_at_last_page() const;

            void set_max(int p_val);
            void set_min(int p_val);
            int get_max() const;
            int get_min() const;
            int get_from() const;
            int get_to() const;
    };


    std::string m_uid = RandomCode(25).get();
    ImDrawList* m_draw_list = nullptr;

    std::string m_selected_path = ExplorerWindowDataPipeline::Ref()->dir_path;
    std::string m_target_path;
    const int m_target_path_length = 500;

    ImVec2 m_target_path_text_input_area_min;
    ImVec2 m_target_path_text_input_area_max;

    Page page;
    struct DisplayListRunner{
        bool m_is_regenerate_needed = true;

        int page_max = 0;
        int page_min = 0;

        ExplorerMode explorer_mode = ExplorerMode::NORAML;

        std::string m_dir = ExplorerWindowDataPipeline::Ref()->dir_path;

        std::mutex m_display_list_mtx;
        std::thread m_display_list_thread = std::thread(&ExplorerWindow::DisplayListRunner::m_generate_one_row_of_raw_files_list, this);
        std::vector<FObjectRow> m_raw_files_list;
        std::vector<FObjectRow> m_display_list;
        std::queue<std::filesystem::directory_entry> m_dir_iterator;
        
        int m_generate_one_row_of_raw_files_list_index = 0;
        void m_refresh_display_list(ExplorerWindow* p_window);
        void m_generate_one_row_of_raw_files_list();
        void m_generate_one_row_of_raw_files_list_single(int p_index, const std::filesystem::directory_entry& p_entry);
        void m_fetch_dir_iterator();
        void m_regenerate_display_list();

        void stop_runner();
        void terminate_runner();
    } m_display_list_runner;
    
    std::vector<FObjectRow> m_display_list_buf;

    void m_close_button_process();
    FObjectRow* m_main_row;
    FObjectRow* m_sub_row;

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
    void m_explorer_area_confirm_button_process();
    void m_explorer_area_target_text_input_process();

    bool m_is_refresh_needed = true;
    bool m_is_confirm_pressed = false;

    void m_go_to_dir(std::filesystem::path p_path);
public:
    ExplorerWindow();
    ~ExplorerWindow();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    void close();
    std::string get_selected();
    bool is_confirm_pressed();

    void set_explorer_mode(ExplorerMode p_mode);

    friend struct DisplayListRunner;
};
