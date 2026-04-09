#pragma once

#include "server/project_server.h"
#include "glm/ext/vector_float2.hpp"
#include "system/obj/abstract/selectable.h"
#include "system/obj/graph/manager.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"
#include <glm/fwd.hpp>
#include <string>
#include <theme/theme_loader.h>
#include <vector>

#include "DecToolsBox/container/ordered_map.h"

using namespace glm;

class GraphBase : public SelectableObject{
public:
    struct Property{
        std::string name;
        std::string value;
        uint max_size;
    };

private:
    std::vector<std::string> m_signals;
    std::set<OID> m_children;
    std::set<OID> m_parent;

    vec4 m_selected_border_color = ThemeLoader::Ref()->get_color("SelectableHighlightColour");
    vec4 m_connectable_border_color = ThemeLoader::Ref()->get_color("AccentColour2");
    ImVec4  m_rect_colour;

    std::string m_name;
    std::string m_content;

    std::string m_uid;

    std::string m_workspace_id;

    OrderedMap<std::string,Property> m_properties = {
        {"Node Name", {"Node Name","", 50}},
    };

    Rect2& m_init_shape();

    void m_handle_event_connect();

    double m_mouse_on_time = 0.0f;
    void m_update_mouse_on_time();

    bool m_is_expanded = false;

    ProjectPayload m_get_root_project_data_payload();
    void m_update_project_data();
    void m_remove_project_data();
    void m_check_if_upload_project_data_needed();

    void m_upload_property(std::string& key, std::string value, uint max_size);
public:
    GraphBase();
    ~GraphBase();

    std::string get_uid();
    void set_uid(std::string p_uid);   

    double get_mouse_on_time() const { return m_mouse_on_time; }

    vec2 get_position() const;
    vec2 get_size() const;
    void set_position(vec2& p_position);
    void set_size(vec2& p_size);

    bool is_point_intersect(vec2& p_point); 

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    virtual GraphManager::NodeTypeId get_type();
    std::string get_type_name();

    std::string get_name();
    std::vector<std::string> get_signals();

    void set_name(std::string p_name);
    void set_name_forced(std::string p_name);
    void set_workspace(std::string p_workspace);
    void add_signals();
    void remove_signals(int p_index);
    void set_signal(int p_index, std::string p_signal);

    void set_property(std::string name, std::string value, uint max_size);
    std::string get_property(std::string p_name);

    void add_children(OID p_id);
    void remove_children(OID p_id);

    void add_parent(OID p_id);
    void remove_parent(OID p_id);

    std::vector<OID> skip_from_repeater();
    std::vector<OID> skip_to_repeater();

    std::vector<OID> get_children(bool is_pass_repeater = false, bool is_all = false);
    std::vector<OID> get_parent(bool is_pass_repeater = false, bool is_all = false);
    std::set<OID> get_children_set(bool is_pass_repeater = false, bool is_all = false);
    std::set<OID> get_parent_set(bool is_pass_repeater = false, bool is_all = false);

    std::vector<OID> get_children_direct();

    std::vector<Property> get_properties();

    void open_details_window();

    std::string get_workspace_id();

    bool is_expanded();
    void expand_on_list();
    void collapse_on_list();

};