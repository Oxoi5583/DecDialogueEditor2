#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

class Timer;
class TimeUnit;
class TimerId;

typedef std::vector<std::shared_ptr<Timer>> TimerList;

class TimerServer : public Singleton<TimerServer> {
private:
    TimerList m_timer_list;
    std::unordered_set<std::string> m_id_set;

    const double m_clear_full_time = 120;
    double m_clear_timer = 0;

    bool m_is_shutdown = false;
    
    void reset_clear_timer();
    void update_clear_timer(double p_delta);
    bool is_clear_timer_timeout();
    void clear_garbage();

public:
    TimerServer();
    ~TimerServer();

    void update(double p_delta);
    Timer* create_timer(TimeUnit p_time_unit, bool p_is_finished = false);
    Timer* create_timer(double p_full_time, bool p_is_finished = false);
    Timer* create_timer(double* p_full_time_ptr, bool p_is_finished = false);

    bool is_timer_exists(TimerId p_id);

    void free_all();
    int get_timer_count();

    void shutdown();

    bool is_already_shutdown();
};

class TimerId {
private:
public:
    char id[13];
    TimerId();
    ~TimerId() = default;

    bool operator==(const TimerId& p_other) const;
    bool operator==(const std::string& p_other) const;
};

class TimeUnit {
public:
    enum class Type {
        MILLISECOND,
        SECOND,
        MINUTE,
        HOUR,
        DAY
    };

    TimeUnit(Type p_type, double p_value)
        : m_type(p_type)
        , m_value(p_value)
        , m_delta(m_calculate_delta()) {
    }

    ~TimeUnit() {}
    TimeUnit(const TimeUnit& p_other);
    TimeUnit& operator=(const TimeUnit& p_other);
    TimeUnit(TimeUnit&& p_other) noexcept;
    TimeUnit& operator=(TimeUnit&& p_other) noexcept;

    uint32_t get_delta() const;

private:
    Type m_type;
    double m_value;
    uint32_t m_delta;

    uint32_t m_calculate_delta() const;
};

class Timer {
public:
    ~Timer();

    bool is_enabled();
    bool is_ready_to_free();
    void start();
    void stop();
    void queue_free();

    void update(double p_delta);
    void reset();
    void finish();
    double get_timeleft();
    double get_time();
    double get_full_time();
    bool is_timeout();
    bool timeout_and_reset();
    void modify_full_time_and_reset(double p_t);
    void modify_full_time_and_finish(double p_t);
    void jump_to(double p_t);

    bool timeout_and_reset_in_cycle(uint32_t p_cycle, double p_interval = 0);
    bool timeout_and_reset_in_cycle(uint32_t p_cycle, TimeUnit p_interval);
    bool is_cycle_done();
    void reset_cycle();
    void finish_cycle();
    int get_current_cycle();

    Timer(Timer& p_t);

    enum class Mode {
        ST_TIMER_GARBAGE,
        ST_TIMER_ENABLE,
        ST_TIMER_DISABLE,
    };

    TimerId get_id();

    friend class TimerServer;
    friend class std::vector<std::unique_ptr<Timer>>;

private:
    static constexpr int m_default_max_run_cycle = -1;
    int m_max_run_cycle = -1;
    int m_run_cycle = -1;

    bool is_interval_done();
    void reset_interval();

    bool m_is_cycle_interval_start = false;
    double m_cycle_interval = 0;
    double m_max_cycle_interval = 0;

    TimerId m_id = TimerId();
    Timer::Mode m_state = Timer::Mode::ST_TIMER_ENABLE;
    double m_time = 0;
    double m_full_time = 0;
    double* m_full_time_store = nullptr;

    Timer(double* p_t_ptr);
    Timer(double p_t);

    bool m_is_marked_garbage = false;
};

class TimerWrapper {
private:
    Timer* m_ptr = nullptr;
    TimerId m_id;

    bool m_freed = false;
public:
    TimerWrapper(TimeUnit p_full_time, bool p_is_finished);
    TimerWrapper(double p_full_time, bool p_is_finished);
    ~TimerWrapper();

    bool is_enabled();
    bool is_ready_to_free();
    void start();
    void stop();

    void update(double p_delta);
    void reset();
    void finish();
    double get_timeleft();
    double get_time();
    double get_full_time();
    bool is_timeout();
    bool timeout_and_reset();
    void modify_full_time_and_reset(double p_t);
    void modify_full_time_and_finish(double p_t);
    void jump_to(double p_t);

    bool timeout_and_reset_in_cycle(uint32_t p_cycle, double p_interval = 0);
    bool timeout_and_reset_in_cycle(uint32_t p_cycle, TimeUnit p_interval);
    bool is_cycle_done();
    void reset_cycle();
    void finish_cycle();
    int get_current_cycle();

    void queue_free();
};