#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "SDL3/SDL_stdinc.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Timer;
class TimeUnit;

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
    Timer* create_timer(TimeUnit _time_unit, bool is_finished = false);
    Timer* create_timer(double _full_time, bool is_finished = false);
    Timer* create_timer(double* _full_time_ptr, bool is_finished = false);

    void free_all();
    int get_timer_count();

    void shutdown();
};


class TimerId{
    private:
    public:
        char id[13];
        TimerId();
        ~TimerId() = default;
        bool operator ==(const TimerId& other) const;
        bool operator ==(const std::string& other) const;
};

class TimeUnit{
public:
    enum class Type{
        MILLISECOND,
        SECOND,
        MINUTE,
        HOUR,
        DAY
    };

    TimeUnit(Type p_type, double p_value)
    : m_type(p_type)
    , m_value(p_value)
    , m_delta(m_calculate_delta()){};
    ~TimeUnit(){};
    TimeUnit(const TimeUnit& other);
    TimeUnit& operator=(const TimeUnit& other);
    TimeUnit(TimeUnit&& other) noexcept;
    TimeUnit& operator=(TimeUnit&& other) noexcept;


    uint32_t get_delta() const;
private:
    Type m_type;
    double m_value;
    uint32_t m_delta;

    uint32_t m_calculate_delta() const;
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

    bool timeout_and_reset_in_cycle(uint32_t cycle, double interval = 0);
    bool timeout_and_reset_in_cycle(uint32_t cycle, TimeUnit interval);
    bool is_cycle_done();
    void reset_cycle();
    void finish_cycle();
    int get_current_cycle();


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
    static constexpr int default_max_run_cycle = -1;
    int max_run_cycle = -1;
    int run_cycle = -1;

    bool is_interval_done();
    void reset_interval();
    bool is_cycle_interval_start = false;
    double cycle_interval = 0;
    double max_cycle_interval = 0;


    TimerId id = TimerId();
    Timer::Mode state = Timer::Mode::ST_TIMER_ENABLE;
    double time = 0;
    double full_time = 0;
    double* full_time_store;
    Timer(double* _t);
    Timer(double _t);

    bool is_marked_garbage = false;
 
};





