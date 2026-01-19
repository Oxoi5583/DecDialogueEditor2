#include "editor/components/detail_window.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "glm/geometric.hpp"
#include "system/graph/camera.h"
#include "glm/ext/vector_float2.hpp"
#include "system/graph/selection.h"
#include "system/graph/viewport.h"
#include "imgui/imgui.h"
#include "system/obj/abstract/movable.h"
#include "system/obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include "struct/shape/rect2.h"
#include "system/obj/graph/base.h"
#include "theme/theme_loader.h"
#include <cstddef>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <vector>


EditorDetailsWindow::EditorDetailsWindow(){
    BIND_CLASS(EditorDetailsWindow);
    Rect2 rect;
    rect.set_size(vec2(600.0f, 300.0f));
    this->set_shape(rect);
}
EditorDetailsWindow::~EditorDetailsWindow(){
    this->save();
}

void EditorDetailsWindow::ready(){
    this->disable_align_grid();
}
void EditorDetailsWindow::pre_process(){
    if(!ObjectServer::Ref()->is_id_valid(m_parent_id)){
        this->queue_free();
        return;
    }

    if(m_opened && this->is_on_camera()){
        vec2 world_lt_pos = this->get_shape<Rect2>().get_left_top();
        vec2 world_rd_pos = this->get_shape<Rect2>().get_right_down();

        GraphCamera* gc = GraphCamera::Ref();
        GraphViewport* gv = GraphViewport::Ref();
        vec2 screen_lt_pos = gv->viewport_to_screen(gc->world_to_viewport(world_lt_pos));
        vec2 screen_rd_pos = gv->viewport_to_screen(gc->world_to_viewport(world_rd_pos));
        
        vec2 size = screen_rd_pos - screen_lt_pos;

        ImGui::SetNextWindowCollapsed(m_collapsed);
        ImGui::SetNextWindowPos({screen_lt_pos.x, screen_lt_pos.y});
        ImGui::SetNextWindowSize({size.x, size.y});


        ImGui::Begin((m_obj_name + " " + m_name).c_str(),&m_opened, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImVec2 curr_size = ImGui::GetWindowSize();
        ImVec2 curr_pos = ImGui::GetWindowPos();
        m_draw_fields();
        ImGui::End();
    }
}
void EditorDetailsWindow::process(){
    
}
void EditorDetailsWindow::post_process(){
    if(!m_opened){
        this->queue_free();
        return;
    }
    ObjectServer::Ref()->move_to_front(this->get_id());
}
void EditorDetailsWindow::draw(){
    if(m_opened){
        Rect2 shadow_rect = this->get_shape<Rect2>();
        shadow_rect.set_position(shadow_rect.get_position() + vec2(-15.0f, 15.0f) );
        EngineRenderer::Ref()->draw_rect(shadow_rect, vec4(0.0f,0.0f,0.0f,0.2f), -1);
    }

    if(ObjectServer::Ref()->is_id_valid(m_parent_id) && m_opened){
        vec2 parent_pos = ObjectServer::Ref()->get_instance<MovableObject>(m_parent_id)->get_position();
        std::vector<vec2> points = this->get_shape<Rect2>().get_points();
        
        float min_dist = std::numeric_limits<float>::max();
        vec2 cloest_pos;
        for(size_t i = 0; i < points.size(); i++){
            vec2 pos = points[i];
            float dist = glm::distance(pos, parent_pos);
            if(dist < min_dist){
                min_dist = dist;
                cloest_pos = pos;
            }
        }
        EngineRenderer::Ref()->draw_line(cloest_pos, parent_pos, ThemeLoader::Ref()->get_color("AccentColour1"), 2.0f / GraphCamera::Ref()->get_zoom());
    }
}

#include <cmath>
std::vector<std::string> EditorDetailsWindow::static_str_pipeline = {};
const std::string EditorDetailsWindow::lb = "\t\n";
int EditorDetailsWindow::max_cols = 80;

void EditorDetailsWindow::m_replace_all_substring(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
std::string EditorDetailsWindow::m_draw_fields_auto_wrap(std::string& p_raw_str, int p_original_len){
    std::string ret = p_raw_str;

    if(strlen(ret.data()) <= max_cols){
        return ret;
    }

    bool searching = true;

    int last_lb_pos = 0;
    int run = 0;
    while(searching){
        for(size_t pos = last_lb_pos ;pos < p_original_len; pos++){
            if(!searching){
                break;
            }

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
int EditorDetailsWindow::m_draw_fields_auto_wrap_callback(ImGuiInputTextCallbackData* p_data){
    std::string data_str = p_data->Buf;
    int original_len = std::strlen(data_str.c_str());

    if(p_data->BufTextLen <= max_cols){
        return 0;
    }

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
            if(!searching){
                break;
            }

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

    return 0;
}
void EditorDetailsWindow::m_draw_fields(){
    ImVec4 color1 = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 color2 = ThemeLoader::Ref()->get_imgui_color("SecondaryColour2");

    ImGui::PushStyleColor(ImGuiCol_FrameBg, color1);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, color2);

    for (Field& f : m_fields) {
        std::string name = f.name;
        ImGui::Text("%s", name.c_str());
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
            ImVec2(-1, rows * (ImGui::GetTextLineHeight() + 5.0f)),
            ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_NoHorizontalScroll,
            m_draw_fields_auto_wrap_callback
        );

        std::string no_lb = f.value;
        m_replace_all_substring(no_lb, lb, "");
        f.raw_value = no_lb;
        f.raw_value.resize(std::strlen(f.raw_value.c_str()));

        if(ImGui::IsItemHovered()){
            EventServer::Ref()->emit(EventLockedAll());
        }
    }

    ImGui::PopStyleColor(2);
}
void EditorDetailsWindow::open_for(OID p_id){
    GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(p_id);
    if(!obj){
        return;
    }

    m_parent_id = p_id;

    m_opened = true;

    std::vector<Field>().swap(m_fields);
    auto properties = obj->get_properties();

    m_obj_name = obj->get_name();

    for(auto it : properties){
        std::string property_name = it.name;
        std::string property_value = it.value;
        uint property_max_size = it.max_size;

        m_fields.push_back({
            property_name,
            property_value,
            property_value,
            property_max_size
        });
    }
}

void EditorDetailsWindow::save(){
    if(!ObjectServer::Ref()->is_id_valid(m_parent_id)){
        return;
    }

    GraphBase* ptr = ObjectServer::Ref()->get_instance<GraphBase>(m_parent_id);
    if(!ptr){
        return;
    }

    for(auto& f : m_fields){
        ptr->set_property(f.name, f.raw_value, f.max_size);
    }
}
