#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "core/timer.h"


typedef std::vector<std::shared_ptr<Timer>> TimerList;


class TimerServer : public Singleton<TimerServer>{
private:
    TimerList timer_list;

    const double clear_full_time = 120;
    double clear_timer = 0;
    void reset_clear_timer();
    void update_clear_timer(double delta);
    bool is_clear_timer_timeout();
    void clear_garbage();

public:
    TimerServer();
    ~TimerServer();

    void update(double delta);
    Timer* CreateTimer(double _full_time, bool is_finished = false);
    Timer* CreateTimer(double* _full_time_ptr, bool is_finished = false);

    void free_all();
    int get_timer_count();
};





