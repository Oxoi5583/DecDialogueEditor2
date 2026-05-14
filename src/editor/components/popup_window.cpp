#include "editor/components/popup_window.h"
#include "DecToolsBox/core/random_code.h"
#include "imgui_internal.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/font_loader.h"
#include "engine/window.h"
#include "glm/ext/vector_float2.hpp"
#include "imgui/imgui.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>

#include "ext/debug/messenger_ext.h"
#include "server/ui_icon_unicode.h"


std::string PopupWindowDataPipeline::get_string(std::string p_id){
    if(!m_str_data.contains(p_id)){
        return "NULL";
    }
    return m_str_data[p_id];
}
int PopupWindowDataPipeline::get_int(std::string p_id){
    if(!m_num_data.contains(p_id)){
        return -1;
    }
    return (int)m_num_data[p_id];
}
float PopupWindowDataPipeline::get_float(std::string p_id){
    if(!m_num_data.contains(p_id)){
        return -1;
    }
    return (float)m_num_data[p_id];
}
bool PopupWindowDataPipeline::get_bool(std::string p_id){
    if(!m_bool_data.contains(p_id)){
        return false;
    }
    return m_bool_data[p_id];
}
void PopupWindowDataPipeline::set_value(std::string p_id, std::string p_val){
    if(!m_str_data.contains(p_id)){
        return;
    }
    m_str_data[p_id] = p_val;
}
void PopupWindowDataPipeline::set_value(std::string p_id, int p_val){
    if(!m_num_data.contains(p_id)){
        return;
    }
    m_num_data[p_id] = p_val;
}
void PopupWindowDataPipeline::set_value(std::string p_id, float p_val){
    if(!m_num_data.contains(p_id)){
        return;
    }
    m_num_data[p_id] = p_val;
}
void PopupWindowDataPipeline::set_value(std::string p_id, bool p_val){
    if(!m_bool_data.contains(p_id)){
        return;
    }
    m_bool_data[p_id] = p_val;
}

void PopupWindowDataPipeline::try_add(std::string p_id){
    m_str_data.try_emplace(p_id, "");
    m_num_data.try_emplace(p_id, 0);
    m_bool_data.try_emplace(p_id, false);
}
void PopupWindowDataPipeline::try_remove(std::string p_id){
    if(m_str_data.contains(p_id)) m_str_data.erase(p_id);
    if(m_num_data.contains(p_id)) m_num_data.erase(p_id);
    if(m_bool_data.contains(p_id)) m_bool_data.erase(p_id);
}

PopupWindow::PopupWindow(){
    BIND_CLASS(PopupWindow);
    PopupWindowManager::Ref()->add_instance(m_uid, this);
}
PopupWindow::~PopupWindow(){
    for(auto input : m_inputs){
        std::string data_id = m_uid + input.uid;
        PopupWindowDataPipeline::Ref()->try_remove(data_id);
    }

    for(auto& action : m_close_callback){
        action();
    }
    
    PopupWindowManager::Ref()->remove_instance(m_uid);
}

void PopupWindow::ready(){
}
void PopupWindow::pre_process(){
    ObjectServer::Ref()->move_to_front(this->get_id());
    m_pop_window_process();
}
void PopupWindow::process(){
}
void PopupWindow::m_pop_window_process(){
    m_set_window_paramter();
    if(ImGui::Begin(m_uid.c_str(),&m_show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNav)){
        m_emit_event_if_window_hovered();
        m_close_button_process();

        if(m_content.length() >= 0){
            float left_spacing = 15.0f;
            float up_spacing = 35.0f;
            ImVec2 cursor_pos = ImVec2(left_spacing, up_spacing);
            ImGui::SetCursorPos(cursor_pos);
            ImGui::Text("%s", m_content.c_str());
        }

        m_inputs_process();
        m_buttons_process();

        m_update_on_screen_ratio();

        ImGui::End();
    }
}
void PopupWindow::m_set_window_paramter(){
    if(m_is_first_process){
        glm::vec2 pos = (EngineWindow::Ref()->get_window_size() / 2.0f) - (m_size / 2.0f);
        ImGui::SetNextWindowPos({pos.x, pos.y});
        m_is_first_process = false;
    }
    if(m_ratio_data.is_restore_needed){
        ImGui::SetNextWindowPos({m_ratio_data.restore_pos.x, m_ratio_data.restore_pos.y});
        m_ratio_data.is_restore_needed = false;
    }
    ImGui::SetNextWindowSize({m_size.x, m_size.y});

    ImGuiWindow* window = ImGui::FindWindowByName(m_uid.c_str());
    if (window) {
        ImGui::BringWindowToDisplayFront(window);
    }
}
void PopupWindow::m_emit_event_if_window_hovered(){
    {
        EventLockedAll event;
        EventServer::Ref()->emit(event);
    }
    if(ImGui::IsWindowHovered()){
        {
            EventMouseHoverObj event;
            event.hovering_pos = MouseServer::Ref()->get_mouse_screen_position();
            event.is_pointer_cursor = false;
            event.obj_id = this->get_id();
            EventServer::Ref()->emit(event);
        }
    }
}
void PopupWindow::m_inputs_process(){
    ImVec4 color1 = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 color2 = ThemeLoader::Ref()->get_imgui_color("SecondaryColour2");

    ImGui::PushStyleColor(ImGuiCol_FrameBg, color1);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, color2);

    float left_spacing = 15.0f;
    float up_spacing = 35.0f + ImGui::CalcTextSize(m_content.c_str()).y;
    float one_input_height = 20.0f;
    ImVec2 cursor_pos = ImVec2(left_spacing, up_spacing);

    size_t i = 0;
    for(auto it : m_inputs){
        cursor_pos.y = cursor_pos.y + (one_input_height + ImGui::GetStyle().ItemSpacing.y);
        ImGui::SetCursorPos(cursor_pos);
        std::string item_name = "##" + it.name + this->m_uid +  it.uid;

        switch (it.type) {
            case STRING:{
                std::string buf = this->get_input_string(i);
                buf.resize(500);
                ImGui::Text("%s", it.name.c_str());
                ImGui::SameLine();
                ImGui::InputText(item_name.c_str(), buf.data(), buf.size(), ImGuiWindowFlags_NoFocusOnAppearing);
                buf.shrink_to_fit();

                std::string data_id = m_uid + it.uid;
                PopupWindowDataPipeline::Ref()->set_value(data_id.c_str(), buf);
                break;
            }
            case INTEGER:{
                int buf = this->get_input_int(i);
                ImGui::Text("%s", it.name.c_str());
                ImGui::SameLine();
                ImGui::InputInt(item_name.c_str(), &buf, ImGuiWindowFlags_NoFocusOnAppearing);

                std::string data_id = m_uid + it.uid;
                PopupWindowDataPipeline::Ref()->set_value(data_id.c_str(), buf);
                break;
            }
            case FLOAT:{
                float buf = this->get_input_float(i);
                ImGui::Text("%s", it.name.c_str());
                ImGui::SameLine();
                ImGui::InputFloat(item_name.c_str(), &buf, ImGuiWindowFlags_NoFocusOnAppearing);

                std::string data_id = m_uid + it.uid;
                PopupWindowDataPipeline::Ref()->set_value(data_id.c_str(), buf);
                break;
            }
            case BOOL:{
                bool buf = this->get_input_bool(i);
                ImGui::Text("%s", it.name.c_str());
                ImGui::SameLine();
                ImGui::Checkbox(item_name.c_str(), &buf);

                std::string data_id = m_uid + it.uid;
                PopupWindowDataPipeline::Ref()->set_value(data_id.c_str(), buf);
                break;
            }
        }

        if(ImGui::IsItemHovered()){
            {
                EventMouseHoverObj event;
                event.hovering_pos = MouseServer::Ref()->get_mouse_screen_position();
                event.is_pointer_cursor = false;
                event.obj_id = this->get_id();
                EventServer::Ref()->emit(event);
            }
            {
                EventLockedAll event;
                EventServer::Ref()->emit(event);
            }
        }
        one_input_height = ImGui::GetItemRectSize().y;

        i++;
    }
    ImGui::PopStyleColor(2);
}
void PopupWindow::m_buttons_process(){
    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 button_size = ImVec2(100, 30);
    float button_y = window_size.y - button_size.y - ImGui::GetStyle().WindowPadding.y;
    float total_button_width = (button_size.x * m_options.size());
    float total_button_spacing = ImGui::GetStyle().ItemSpacing.x * (m_options.size() - 1);
    float cursor_x = (window_size.x - (total_button_width + total_button_spacing)) / 2.0f;
    ImVec2 cursor_pos = ImVec2(cursor_x, button_y);

    size_t i = 0;
    for(auto it : m_options){
        ImGui::SetCursorPos(cursor_pos);
        std::string item_name = it.name + "##" + this->m_uid +  it.uid;
        ImGui::Button(item_name.c_str() , button_size);
        if(ImGui::IsItemClicked()){
            it.action();
        }
        if(ImGui::IsItemHovered()){
            {
                EventMouseHoverObj event;
                event.hovering_pos = MouseServer::Ref()->get_mouse_screen_position();
                event.is_pointer_cursor = false;
                event.obj_id = this->get_id();
                EventServer::Ref()->emit(event);
            }
            {
                EventLockedAll event;
                EventServer::Ref()->emit(event);
            }
        }

        cursor_pos.x = cursor_pos.x + (button_size.x + ImGui::GetStyle().ItemSpacing.x);
        i++;
    }
}
void PopupWindow::m_close_button_process(){
    float content_width = ImGui::GetContentRegionAvail().x + 10;
    float button_width = 20.0f;
    float button_height = 0.0f;
    float button_x = content_width - button_width;
    ImGui::SetCursorPosX(button_x);

    std::string button_id = ICON_WIN_CLOSE;
    button_id += "##";
    button_id += m_uid;

    ImGui::PushFont(EngineFontLoader::Ref()->get(12));
    ImGui::Button(button_id.c_str(), ImVec2(button_width, button_height));
    ImGui::PopFont();
    if(ImGui::IsItemHovered()){
        {
            EventMouseHoverObj event;
            event.hovering_pos = MouseServer::Ref()->get_mouse_screen_position();
            event.is_pointer_cursor = false;
            event.obj_id = this->get_id();
            EventServer::Ref()->emit(event);
        }
    }
    if(ImGui::IsItemClicked()){
        this->queue_free();
    }
}
void PopupWindow::post_process(){
}
void PopupWindow::draw(){
}

void PopupWindow::set_title(std::string p_title){
    m_title = p_title;
}
void PopupWindow::set_content(std::string p_content){
    m_content = p_content;
}
void PopupWindow::add_option(std::string p_option_name, std::function<void()> p_action){
    m_options.push_back({RandomCode(20).get(),p_option_name, p_action});
}
void PopupWindow::add_input(std::string p_option_name, InputType p_type){
    m_inputs.push_back({RandomCode(20).get(),p_option_name, p_type});
    std::string data_id = m_uid + m_inputs.back().uid;
    PopupWindowDataPipeline::Ref()->try_add(data_id);
}
void PopupWindow::add_close_fallback(std::function<void()> p_action){
    m_close_callback.push_back(p_action);
}
void PopupWindow::set_size(glm::vec2 p_size){
    m_size = p_size;
}

std::string PopupWindow::get_input_string(size_t p_index){
    if(p_index >= m_inputs.size()){
        return "NULL";
    }

    Input input = this->m_inputs[p_index];
    std::string data_id = m_uid + input.uid;

    std::string str = PopupWindowDataPipeline::Ref()->get_string(data_id);
    std::string ret;
    std::copy_if(
        str.begin(),
        str.end(),
        std::back_inserter(ret),
        [](char c){ return c != '\0'; }
    );
    
    return ret;
}
int PopupWindow::get_input_int(size_t p_index){
    if(p_index >= m_inputs.size()){
        return -1;
    }

    Input input = this->m_inputs[p_index];
    std::string data_id = m_uid + input.uid;

    return PopupWindowDataPipeline::Ref()->get_int(data_id);
}
float PopupWindow::get_input_float(size_t p_index){
    if(p_index >= m_inputs.size()){
        return -1;
    }

    Input input = this->m_inputs[p_index];
    std::string data_id = m_uid + input.uid;

    return PopupWindowDataPipeline::Ref()->get_float(data_id);
}
bool PopupWindow::get_input_bool(size_t p_index){
    if(p_index >= m_inputs.size()){
        return false;
    }

    Input input = this->m_inputs[p_index];
    std::string data_id = m_uid + input.uid;

    return PopupWindowDataPipeline::Ref()->get_bool(data_id);
}

void PopupWindow::show(){
    m_show = true;
}
void PopupWindow::hide(){
    m_show = false;
}
void PopupWindow::close(){
    this->queue_free();
}

void PopupWindow::m_update_on_screen_ratio(){
    ImVec2 imgui_pos = ImGui::GetWindowPos();
    vec2 pos = vec2(imgui_pos.x, imgui_pos.y) + (m_size / 2.0f);

    m_ratio_data.last_frame_window_size = m_ratio_data.this_frame_window_size;
    m_ratio_data.this_frame_window_size = EngineWindow::Ref()->get_window_size();
    
    m_ratio_data.on_screen_ratio = pos / m_ratio_data.last_frame_window_size;
}

void PopupWindow::restore_screen_ratio(){
    vec2 window_size = m_ratio_data.this_frame_window_size;
    m_ratio_data.restore_pos = window_size * m_ratio_data.on_screen_ratio - (m_size / 2.0f);
    m_ratio_data.is_restore_needed = true;
}

std::string PopupWindow::get_uid(){
    return m_uid;
}

void PopupWindowManager::add_instance(std::string p_uid, PopupWindow* p_ptr){
    m_instances.try_emplace(p_uid, p_ptr);
}
void PopupWindowManager::remove_instance(std::string p_uid){
    m_instances.erase(p_uid);
}

void PopupWindowManager::restore_all_pos(){
    for(auto it : m_instances){
        it.second->restore_screen_ratio();
    }
}

bool PopupWindowManager::is_window_exists(std::string p_uid){
    return m_instances.contains(p_uid.c_str());
}
