#include "server/timer_server.h"

#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"

#include <algorithm>
#include <cstring>
#include <vector>

TimerServer::TimerServer() {}

TimerServer::~TimerServer() {
}

void TimerServer::reset_clear_timer() {
    m_clear_timer = 0;
}

void TimerServer::update_clear_timer(double p_delta) {
    m_clear_timer = std::clamp(m_clear_timer + p_delta, 0.0, m_clear_full_time);
}

bool TimerServer::is_clear_timer_timeout() {
    bool is_clear_timeup = m_clear_timer >= m_clear_full_time;
    return is_clear_timeup;
}

void TimerServer::clear_garbage() {
    if (m_timer_list.size() > 0) {
        auto new_end = std::remove_if(
            m_timer_list.begin(),
            m_timer_list.end(),
            [this](std::shared_ptr<Timer> p_timer) {
                bool ret = p_timer->is_ready_to_free();
                if (ret) {
                    this->m_id_set.erase(p_timer->get_id().id);
                    return true;
                } else {
                    return false;
                }
            });

        m_timer_list.erase(new_end, m_timer_list.end());
    }
}

void TimerServer::update(double p_delta) {
    update_clear_timer(p_delta);

    if (is_clear_timer_timeout()) {
        clear_garbage();
    }

    for (auto& timer : m_timer_list) {
        timer->update(p_delta);
    }
}

Timer* TimerServer::create_timer(TimeUnit p_time_unit, bool p_is_finished) {
    m_timer_list.push_back(std::shared_ptr<Timer>(new Timer(p_time_unit.get_delta())));
    m_id_set.emplace(m_timer_list.back()->get_id().id);

    if (p_is_finished) {
        m_timer_list.back()->finish();
    }

    return m_timer_list.back().get();
}

Timer* TimerServer::create_timer(double p_full_time, bool p_is_finished) {
    m_timer_list.push_back(std::shared_ptr<Timer>(new Timer(p_full_time)));
    m_id_set.emplace(m_timer_list.back()->get_id().id);

    if (p_is_finished) {
        m_timer_list.back()->finish();
    }

    return m_timer_list.back().get();
}

Timer* TimerServer::create_timer(double* p_full_time_ptr, bool p_is_finished) {
    m_timer_list.push_back(std::shared_ptr<Timer>(new Timer(p_full_time_ptr)));
    m_id_set.emplace(m_timer_list.back()->get_id().id);

    if (p_is_finished) {
        m_timer_list.back()->finish();
    }

    return m_timer_list.back().get();
}

void TimerServer::free_all() {
    clear_garbage();
}

int TimerServer::get_timer_count() {
    return static_cast<int>(m_timer_list.size());
}

void TimerServer::shutdown() {
    if(m_is_shutdown){
        return;
    }

    INFO_MSG("Timer Count (Before Free) : " << this->get_timer_count());
    this->free_all();
    INFO_MSG("Timer Count (After Free) : " << this->get_timer_count());

    m_is_shutdown = true;
}
bool TimerServer::is_already_shutdown(){
    return m_is_shutdown;
}

bool TimerId::operator==(const TimerId& p_other) const {
    std::string id_s = this->id;
    std::string other_s = p_other.id;
    return id_s == other_s;
}

bool TimerId::operator==(const std::string& p_other) const {
    std::string id_s = this->id;
    return id_s == p_other;
}

TimerId::TimerId() {
    std::string new_id = RandomCode(12).get() + '\0';
    memcpy(id, new_id.c_str(), new_id.size());
}

TimeUnit::TimeUnit(const TimeUnit& p_other)
    : m_type(p_other.m_type)
    , m_value(p_other.m_value)
    , m_delta(p_other.m_delta) {
}

TimeUnit& TimeUnit::operator=(const TimeUnit& p_other) {
    if (this != &p_other) {
        m_type = p_other.m_type;
        m_value = p_other.m_value;
        m_delta = p_other.m_delta;
    }
    return *this;
}

TimeUnit::TimeUnit(TimeUnit&& p_other) noexcept
    : m_type(p_other.m_type)
    , m_value(p_other.m_value)
    , m_delta(p_other.m_delta) {
}

TimeUnit& TimeUnit::operator=(TimeUnit&& p_other) noexcept {
    if (this != &p_other) {
        m_type = p_other.m_type;
        m_value = p_other.m_value;
        m_delta = p_other.m_delta;
    }
    return *this;
}

uint32_t TimeUnit::get_delta() const {
    return m_delta;
}

uint32_t TimeUnit::m_calculate_delta() const {
    switch (m_type) {
        case Type::MILLISECOND:
            return m_value;
        case Type::SECOND:
            return m_value * 1000;
        case Type::MINUTE:
            return m_value * 60000;
        case Type::HOUR:
            return m_value * 3600000;
        case Type::DAY:
            return m_value * 86400000;
    }

    return m_value;
}

// Init Class with pointer
// In case, Timer created before the origin value was not init.
Timer::Timer(double* p_t_ptr) {
    m_full_time_store = p_t_ptr;
    m_full_time = 0;
    m_time = 0;
}

Timer::Timer(double p_t) {
    m_full_time_store = nullptr;
    m_full_time = p_t;
    m_time = 0;
}

Timer::Timer(Timer& p_t) {
    m_id = p_t.m_id;
    m_state = p_t.m_state;
    m_time = p_t.m_time;
    m_full_time = p_t.m_full_time;
    m_full_time_store = p_t.m_full_time_store;
}

Timer::~Timer() {
    // INFO_MSG("Timer (id : " << m_id.id << ") Freed.");
}

void Timer::update(double p_delta) {
    if (m_full_time_store != nullptr) {
        m_full_time = *m_full_time_store;
        m_full_time_store = nullptr;
        INFO_MSG("Timer full time Init.");
    }

    if (this->is_enabled()) {
        m_time = std::clamp(m_time + p_delta, 0.0, m_full_time);
    }

    if (this->is_enabled() && m_is_cycle_interval_start) {
        m_cycle_interval = std::clamp(m_cycle_interval + p_delta, 0.0, m_max_cycle_interval);
    }
}

void Timer::reset() {
    m_time = 0;
}

void Timer::finish() {
    m_time = m_full_time;
}

double Timer::get_timeleft() {
    return m_full_time - m_time;
}

double Timer::get_time() {
    return m_time;
}

double Timer::get_full_time() {
    return m_full_time;
}

bool Timer::is_timeout() {
    return m_time >= m_full_time;
}

void Timer::modify_full_time_and_reset(double p_t) {
    m_full_time = p_t;
    this->reset();
}

void Timer::modify_full_time_and_finish(double p_t) {
    m_full_time = p_t;
    this->finish();
}

bool Timer::timeout_and_reset() {
    if (m_time >= m_full_time) {
        this->reset();
        return true;
    } else {
        return false;
    }
}

bool Timer::is_enabled() {
    if (this->m_state == Timer::Mode::ST_TIMER_ENABLE) {
        return true;
    } else {
        return false;
    }
}

bool Timer::is_ready_to_free() {
    if (this->m_state == Timer::Mode::ST_TIMER_GARBAGE || this->m_is_marked_garbage) {
        return true;
    } else {
        return false;
    }
}

void Timer::start() {
    m_state = Timer::Mode::ST_TIMER_ENABLE;
}

void Timer::stop() {
    m_state = Timer::Mode::ST_TIMER_DISABLE;
}

void Timer::queue_free() {
    m_state = Timer::Mode::ST_TIMER_GARBAGE;
    m_is_marked_garbage = true;
}

TimerId Timer::get_id() {
    return this->m_id;
}

void Timer::jump_to(double p_t) {
    this->m_time = std::clamp<double>(p_t, 0, m_full_time);
}

bool Timer::timeout_and_reset_in_cycle(uint32_t p_cycle, double p_interval) {
    if (m_max_run_cycle == m_default_max_run_cycle) {
        m_max_run_cycle = static_cast<int>(p_cycle);
        reset_cycle();
        m_max_cycle_interval = p_interval;
        reset_interval();
    }

    if (!is_timeout()) {
        return false;
    }

    m_is_cycle_interval_start = true;

    if (is_cycle_done()) {
        return false;
    }

    if (is_interval_done()) {
        this->reset();
        this->reset_interval();
        m_run_cycle++;
    }

    return true;
}

bool Timer::timeout_and_reset_in_cycle(uint32_t p_cycle, TimeUnit p_interval) {
    return timeout_and_reset_in_cycle(p_cycle, p_interval.get_delta());
}

bool Timer::is_cycle_done() {
    return (m_run_cycle > m_max_run_cycle && m_max_run_cycle != m_default_max_run_cycle);
}

void Timer::reset_cycle() {
    m_run_cycle = 1;
}

void Timer::finish_cycle() {
    m_run_cycle = m_max_run_cycle + 100;
}

int Timer::get_current_cycle() {
    return m_run_cycle;
}

bool Timer::is_interval_done() {
    return m_cycle_interval >= m_max_cycle_interval;
}

void Timer::reset_interval() {
    m_cycle_interval = 0.0f;
    m_is_cycle_interval_start = false;
}

TimerWrapper::TimerWrapper(TimeUnit p_full_time, bool p_is_finished) {
    m_ptr = TimerServer::Ref()->create_timer(p_full_time, p_is_finished);
    m_id = m_ptr->get_id();
}

TimerWrapper::TimerWrapper(double p_full_time, bool p_is_finished) {
    m_ptr = TimerServer::Ref()->create_timer(p_full_time, p_is_finished);
    m_id = m_ptr->get_id();
}

bool TimerServer::is_timer_exists(TimerId p_id){
    return m_id_set.contains(p_id.id);
}

TimerWrapper::~TimerWrapper() {
    if(TimerServer::Ref()->is_timer_exists(m_id)){
        m_ptr->queue_free();
    }
}

bool TimerWrapper::is_enabled() {
    return m_ptr->is_enabled();
}

bool TimerWrapper::is_ready_to_free() {
    return m_ptr->is_ready_to_free();
}

void TimerWrapper::start() {
    m_ptr->start();
}

void TimerWrapper::stop() {
    m_ptr->stop();
}

void TimerWrapper::update(double p_delta) {
    m_ptr->update(p_delta);
}

void TimerWrapper::reset() {
    m_ptr->reset();
}

void TimerWrapper::finish() {
    m_ptr->finish();
}

double TimerWrapper::get_timeleft() {
    return m_ptr->get_timeleft();
}

double TimerWrapper::get_time() {
    return m_ptr->get_time();
}

double TimerWrapper::get_full_time() {
    return m_ptr->get_full_time();
}

bool TimerWrapper::is_timeout() {
    return m_ptr->is_timeout();
}

bool TimerWrapper::timeout_and_reset() {
    return m_ptr->timeout_and_reset();
}

void TimerWrapper::modify_full_time_and_reset(double p_t) {
    m_ptr->modify_full_time_and_reset(p_t);
}

void TimerWrapper::modify_full_time_and_finish(double p_t) {
    m_ptr->modify_full_time_and_finish(p_t);
}

void TimerWrapper::jump_to(double p_t) {
    m_ptr->jump_to(p_t);
}

bool TimerWrapper::timeout_and_reset_in_cycle(uint32_t p_cycle, double p_interval) {
    return m_ptr->timeout_and_reset_in_cycle(p_cycle, p_interval);
}

bool TimerWrapper::timeout_and_reset_in_cycle(uint32_t p_cycle, TimeUnit p_interval) {
    return m_ptr->timeout_and_reset_in_cycle(p_cycle, p_interval);
}

bool TimerWrapper::is_cycle_done() {
    return m_ptr->is_cycle_done();
}

void TimerWrapper::reset_cycle() {
    m_ptr->reset_cycle();
}

void TimerWrapper::finish_cycle() {
    m_ptr->finish_cycle();
}

int TimerWrapper::get_current_cycle() {
    return m_ptr->get_current_cycle();
}

void TimerWrapper::queue_free(){
    if(m_freed){
        return;
    }
    m_ptr->queue_free();
    m_freed = true;
}