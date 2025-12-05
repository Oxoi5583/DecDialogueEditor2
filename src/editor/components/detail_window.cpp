#include "editor/components/detail_window.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/renderer.h"
#include "graph/camera.h"
#include "glm/ext/vector_float2.hpp"
#include "graph/viewport.h"
#include "imgui/imgui.h"
#include "obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include "struct/shape/rect2.h"
#include "obj/graph/base.h"
#include "theme/theme_loader.h"
#include <cstddef>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <vector>


EditorDetailWindow::EditorDetailWindow(){
    BIND_CLASS(EditorDetailWindow);
}
EditorDetailWindow::~EditorDetailWindow(){
    
}

void EditorDetailWindow::ready(){
    Rect2 rect;
    rect.set_position(vec2(0.0f,0.0f));
    rect.set_size(vec2(100.0f, 100.0f));
    this->set_shape(rect);
    this->disable_align_grid();
}
void EditorDetailWindow::pre_process(){
    ObjectServer::Ref()->move_to_front(this->get_id());

    if(m_opened){
        vec2 world_lt_pos = this->get_shape<Rect2>().get_left_top();
        vec2 world_rd_pos = this->get_shape<Rect2>().get_right_down();

        GraphCamera* gc = GraphCamera::Ref();
        GraphViewport* gv = GraphViewport::Ref();
        vec2 screen_lt_pos = gc->world_to_viewport(world_lt_pos);
        vec2 screen_rd_pos = gc->world_to_viewport(world_rd_pos);
        vec2 size = screen_rd_pos - screen_lt_pos;

        ImGui::SetNextWindowCollapsed(m_collapsed);
        ImGui::SetNextWindowPos({screen_lt_pos.x, screen_lt_pos.y});
        ImGui::SetNextWindowSize({size.x, size.y});

        ImGui::Begin((m_obj_name + " " + m_name).c_str(),&m_opened);

        m_draw_fields();

        /*
        ImVec2 new_lt_pos_imgui = ImGui::GetWindowPos();
        ImVec2 new_size_imgui = ImGui::GetWindowSize();

        vec2 new_lt_pos = {new_lt_pos_imgui.x, new_lt_pos_imgui.y};
        vec2 new_size = {new_size_imgui.x, new_size_imgui.y};
        vec2 new_rd_pos = new_lt_pos + new_size;

        vec2 new_world_lt_pos = gc->viewport_to_world(gv->screen_to_viewport(new_lt_pos));
        vec2 new_world_rd_pos = gc->viewport_to_world(gv->screen_to_viewport(new_rd_pos));

        this->get_shape<Rect2>().set_AABB(new_world_lt_pos, new_world_rd_pos);
        */

        ImGui::End();
    }
}
void EditorDetailWindow::process(){
    
}
void EditorDetailWindow::post_process(){
    
}
void EditorDetailWindow::draw(){
    EngineRenderer::Ref()->draw_circle(this->get_shape<Rect2>().get_center(), 5.0f, vec4(1.0f,1.0f,0.0f,1.0f), -1);
    EngineRenderer::Ref()->draw_circle(GraphCamera::Ref()->get_zoomed_rect().get_left_top(), 5.0f, vec4(1.0f,1.0f,0.0f,1.0f), -1);
}

#include <cmath>
std::vector<std::string> EditorDetailWindow::static_str_pipeline = {};
const std::string EditorDetailWindow::lb = "\t\n";
int EditorDetailWindow::max_cols = 80;

void EditorDetailWindow::m_replace_all_substring(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
std::string EditorDetailWindow::m_draw_fields_auto_wrap(std::string& p_raw_str, int p_original_len){
    std::string ret = p_raw_str;

    bool searching = true;

    int last_lb_pos = 0;
    int run = 0;
    while(searching){
        for(size_t pos = last_lb_pos ;pos < p_original_len; pos++){
            if(ret.data()[pos] == '\n' || ret.data()[pos] == '\t') last_lb_pos = pos;
            if(pos >= p_original_len - 1){
                searching = false;
                break;
            }
            if(pos - last_lb_pos > max_cols){
                ret.insert(pos, lb.c_str());
                p_original_len += lb.length();
                break;
            }
        }
    }

    return ret;
}
int EditorDetailWindow::m_draw_fields_auto_wrap_callback(ImGuiInputTextCallbackData* p_data){
    std::string data_str = p_data->Buf;
    int original_len = std::strlen(data_str.c_str());
    
    if (data_str.ends_with('\t')){
        p_data->DeleteChars(original_len - 1, 1);

        data_str = p_data->Buf;
        original_len = std::strlen(data_str.c_str());
    }

    bool searching = true;

    int last_lb_pos = 0;
    int run = 0;
    while(searching){
        for(size_t pos = last_lb_pos ;pos < original_len; pos++){
            if(p_data->Buf[pos] == '\n' || p_data->Buf[pos] == '\t') last_lb_pos = pos;
            if(pos >= original_len - 1){
                searching = false;
                break;
            }
            if(pos - last_lb_pos > max_cols){
                p_data->InsertChars(pos, lb.c_str());
                original_len += lb.length();
                break;
            }
        }
    }


}
void EditorDetailWindow::m_draw_fields(){
    ImVec4 color1 = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 color2 = ThemeLoader::Ref()->get_imgui_color("SecondaryColour2");

    ImGui::PushStyleColor(ImGuiCol_FrameBg, color1);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, color2);

    for (Field& f : m_fields) {
        ImGui::Text("%s", f.name.c_str());
        ImGui::SameLine();

        float available_width = ImGui::GetContentRegionAvail().x;
        ImVec2 text_size = ImGui::CalcTextSize("A");
        float text_max_width = text_size.x * f.max_size;
        float rows = std::max(ceil(text_max_width / available_width), 1.0f);
        max_cols = (available_width - 20.0f) / text_size.x;

        int original_len = f.raw_value.length();
        
        f.value = m_draw_fields_auto_wrap(f.raw_value, original_len);
        f.value.resize(f.max_size);
        ImGui::InputTextMultiline(
            ("##" + f.name).c_str(),
            f.value.data(),
            f.max_size,
            ImVec2(-1, rows * ImGui::GetTextLineHeight()),
            ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_NoHorizontalScroll,
            m_draw_fields_auto_wrap_callback
        );

        std::string no_lb = f.value;
        m_replace_all_substring(no_lb, lb, "");
        f.raw_value = no_lb;
        f.raw_value.resize(std::strlen(f.raw_value.c_str()));   
    }

    ImGui::PopStyleColor(2);
}
void EditorDetailWindow::open_for(OID p_id){
    GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(p_id);
    if(!obj){
        return;
    }

    m_opened = true;

    std::vector<Field>().swap(m_fields);
    auto properties = obj->get_properties();

    m_obj_name = obj->get_name();

    for(auto it : properties){
        std::string property_name = it.first;
        std::string property_value = it.second.value;
        uint property_max_size = it.second.max_size;

        m_fields.push_back({
            property_name,
            property_value,
            property_value,
            property_max_size
        });
    }
}