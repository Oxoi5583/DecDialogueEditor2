#pragma once

#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include <functional>
#include <string>
#include <map>
#include "DecToolsBox/struct/random_code.h"

class PopupWindow : public ObjectBase{
private:
    std::string m_uid = RandomCode(25).get();

    std::string m_title;
    std::string m_content;

    std::map<std::string, std::function<void()>> m_options;

    bool m_show = true;

    glm::vec2 m_size = {300.0f, 200.0f };

    bool m_is_first_process  = true;
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
    void set_size(glm::vec2 p_size);
    
    void show();
    void hide();
    void close();
};