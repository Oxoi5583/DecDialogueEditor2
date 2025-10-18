#pragma once

#include "DecToolsBox/abstract./singleton.h"

class Timer;

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





