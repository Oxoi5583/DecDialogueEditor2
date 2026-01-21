#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/abstract/singleton.h"
#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include <cstddef>
#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include "DecToolsBox/core/random_code.h"

class PopupWindowDataPipeline : public Singleton<PopupWindowDataPipeline>{
private:
    std::unordered_map<std::string, std::string> m_str_data;
    std::unordered_map<std::string, float> m_num_data;
    std::unordered_map<std::string, bool> m_bool_data;
public:
    PopupWindowDataPipeline() = default;
    ~PopupWindowDataPipeline() = default;

    void set_value(std::string p_id, std::string p_val);
    void set_value(std::string p_id, int p_val);
    void set_value(std::string p_id, float p_val);
    void set_value(std::string p_id, bool p_val);

    std::string get_string(std::string p_id);
    int get_int(std::string p_id);
    float get_float(std::string p_id);
    bool get_bool(std::string p_id);

    void try_add(std::string p_id);
    void try_remove(std::string p_id);
};


class PopupWindow : public ObjectBase{
public:
    enum InputType{
        STRING,
        INTEGER,
        FLOAT,
        BOOL,
    };
private:
    std::string m_uid = RandomCode(25).get();

    std::string m_title;
    std::string m_content;

    struct Option{
        std::string uid;
        std::string name;
        std::function<void()> action;
    };
    std::vector<Option> m_options;

    struct Input{
        std::string uid;
        std::string name;
        InputType type;
    };
    std::vector<Input> m_inputs;

    bool m_show = true;
    glm::vec2 m_size = {300.0f, 200.0f };
    bool m_is_first_process  = true;
    void m_pop_window_process();
    void m_set_window_paramter();
    void m_emit_event_if_window_hovered();
    void m_inputs_process();
    void m_buttons_process();
    void m_close_button_process();


    struct{
        glm::vec2 restore_pos;
        glm::vec2 on_screen_ratio;
        glm::vec2 this_frame_window_size;
        glm::vec2 last_frame_window_size;
        bool is_restore_needed = false;
    } m_ratio_data;

    void m_update_on_screen_ratio();
public:
    PopupWindow();
    ~PopupWindow();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    void set_title(std::string p_title);
    void set_content(std::string p_content);
    
    void add_option(std::string p_option_name, std::function<void()> p_action);
    void add_input(std::string p_option_name, InputType p_type);

    void set_size(glm::vec2 p_size);
    
    std::string get_input_string(size_t p_index);
    int get_input_int(size_t p_index);
    float get_input_float(size_t p_index);
    bool get_input_bool(size_t p_index);

    void show();
    void hide();
    void close();

    void restore_screen_ratio();

    std::string get_uid();
};

class PopupWindowManager : public Singleton<PopupWindowManager>{
private:
    std::unordered_map<std::string, PopupWindow*> m_instances;
public:
    PopupWindowManager() = default;
    ~PopupWindowManager() = default;

    void add_instance(std::string p_uid, PopupWindow* p_ptr);
    void remove_instance(std::string p_uid);

    void restore_all_pos();

    bool is_window_exists(std::string p_uid);
};