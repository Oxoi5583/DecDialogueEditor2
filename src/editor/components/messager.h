#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "core/timer_server.h"
#include "glm/ext/vector_float2.hpp"
#include <queue>
#include <vector>

class EditorMessager : public Singleton<EditorMessager>{
private:
    struct Message{
        std::string uid;
        std::string content;
        double delta;
    };
    std::vector<Message> m_messages;

    const double m_max_dlt = TimeUnit(TimeUnit::Type::SECOND, 15.0f).get_delta();

    void m_update_msg_dlt();
    void m_clear_msg();
public:
    EditorMessager() = default;
    ~EditorMessager() = default;

    void add_message(std::string p_cnt);

    void process();
    void draw();
};