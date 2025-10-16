#pragma once

#include <string>

#include <math.h>
#include <vector>
#include <memory>

#include "core/random_server.h"


class TimerServer;

class TimerId{
    private:
    public:
        char id[13];
        TimerId();
        ~TimerId();
        bool operator ==(const TimerId& other) const;
        bool operator ==(const std::string& other) const;
};


class Timer{

public:
    ~Timer();

    bool is_enabled();
    bool is_ready_to_free();
    void start();
    void stop();
    void queue_free();

    void update(double delta);
    void reset();
    void finish();
    double get_timeleft();
    double get_time();
    double get_full_time();
    bool is_timeout();
    bool timeout_and_reset();
    void modify_full_time_and_reset(double _t);
    void modify_full_time_and_finish(double _t);
    void jump_to(double _t);

    Timer(Timer& t);

    enum class Mode{
        ST_TIMER_GARBAGE,
        ST_TIMER_ENABLE,
        ST_TIMER_DISABLE,
    };

    TimerId get_id();
    friend class TimerServer;
    friend class std::vector<std::unique_ptr<Timer>>;

private:
    TimerId id = TimerId();
    Timer::Mode state = Timer::Mode::ST_TIMER_ENABLE;
    double time = 0;
    double full_time = 0;
    double* full_time_store;
    Timer(double* _t);
    Timer(double _t);

    bool is_marked_garbage = false;
 
};


