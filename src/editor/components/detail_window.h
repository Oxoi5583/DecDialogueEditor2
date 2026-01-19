#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "editor/components/base.h"
#include "glm/detail/qualifier.hpp"
#include "glm/fwd.hpp"
#include "imgui/imgui.h"
#include "system/obj/abstract/dragable.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"
#include <string>
#include <vector>

class EditorDetailsWindow : public DragableObject{
private:
    struct Field{
        std::string name;
        std::string raw_value;
        std::string value;
        uint max_size;
    };

    std::vector<Field> m_fields = {
        {"a field", "b1", "b1", 100},
        {"b field", "b2", "b2", 500},
        {"c field", "b3", "b3", 1000},
    };

    OID m_parent_id = -1;

    std::string m_obj_name = "TEST 123";
    std::string m_name = "Details";
    bool m_opened = true;
    bool m_collapsed = false;

    ImVec2 m_prev_size = ImVec2(0, 0);

    static const std::string lb;
    static std::vector<std::string> static_str_pipeline;
    static int max_cols;
    void m_draw_fields();
    static void m_replace_all_substring(std::string& str, const std::string& from, const std::string& to);
    std::string m_draw_fields_auto_wrap(std::string& p_raw_str, int p_original_len);
    static int m_draw_fields_auto_wrap_callback(ImGuiInputTextCallbackData* p_data);
public:
    EditorDetailsWindow();
    ~EditorDetailsWindow();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    void open_for(OID p_id);

    void save();
};