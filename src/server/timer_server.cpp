#include "server/timer_server.h"

#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"
#include "DecToolsBox/server/random_server.h"
#include <cstring>
#include <vector>
#include <math.h>
#include <algorithm>




 
TimerServer::TimerServer(){}

TimerServer::~TimerServer(){
}

void TimerServer::reset_clear_timer(){
    clear_timer = 0;
}
void TimerServer::update_clear_timer(double delta){
    clear_timer = std::clamp(clear_timer + delta ,(double)0 , this->clear_full_time);
}
bool TimerServer::is_clear_timer_timeout(){
    bool is_clear_timeup = clear_timer >= clear_full_time;
    return is_clear_timeup;
}
void TimerServer::clear_garbage(){
    if(timer_list.size() > 0){
    auto new_end = std::remove_if(timer_list.begin(), timer_list.end(),
                                    [](std::shared_ptr<Timer>  _timer){ 
                                        bool ret = _timer->is_ready_to_free();
                                        if(ret){
                                            return true;
                                        }else{
                                            return false;
                                        }
                                    });
    timer_list.erase(new_end, timer_list.end());
    }
}
void TimerServer::update(double delta){
    update_clear_timer(delta);
    if(is_clear_timer_timeout()){
        clear_garbage();
    }
    for(auto& timer : timer_list){
        timer->update(delta);
    }
}



Timer* TimerServer::create_timer(TimeUnit _time_unit, bool is_finished){
    timer_list.push_back(std::shared_ptr<Timer>(new Timer(_time_unit.get_delta())));

    if(is_finished){
        timer_list.back()->finish();
    }
    
    return timer_list.back().get();
}
Timer* TimerServer::create_timer(double _full_time, bool is_finished){
    timer_list.push_back(std::shared_ptr<Timer>(new Timer(_full_time)));

    if(is_finished){
        timer_list.back()->finish();
    }
    
    return timer_list.back().get();
}
Timer* TimerServer::create_timer(double* _full_time_ptr, bool is_finished){
    timer_list.push_back(std::shared_ptr<Timer>(new Timer((double*)_full_time_ptr)));

    if(is_finished){
        timer_list.back()->finish();
    }
    
    return timer_list.back().get();
}

void TimerServer::free_all(){
    for(size_t i = 0; i < timer_list.size(); i++){
        timer_list[i]->queue_free();
    }
    clear_garbage();
}

int TimerServer::get_timer_count(){
    return timer_list.size();
}

void TimerServer::shutdown(){
    INFO_MSG("Timer Count (Before Free) : " << this->get_timer_count());
    this->free_all();
    INFO_MSG("Timer Count (After Free) : " << this->get_timer_count());
}

bool TimerId::operator ==(const TimerId& other) const{
    std::string id_s = this->id;
    std::string other_s = other.id;
    return id_s == other_s;
}
bool TimerId::operator ==(const std::string& other) const{
    std::string id_s = this->id;
    return id_s == other;
}

TimerId::TimerId(){
    std::string new_id = RandomCode(12).get() + '\0';
    memcpy(id, new_id.c_str(), new_id.size());
}

TimeUnit::TimeUnit(const TimeUnit& other)
    : m_type(other.m_type)
    , m_value(other.m_value)
    , m_delta(other.m_delta) {}

TimeUnit& TimeUnit::operator=(const TimeUnit& other) {
    if (this != &other) {
        m_type = other.m_type;
        m_value = other.m_value;
        m_delta = other.m_delta;
    }
    return *this;
}

TimeUnit::TimeUnit(TimeUnit&& other) noexcept
    : m_type(other.m_type)
    , m_value(other.m_value)
    , m_delta(other.m_delta) {}

TimeUnit& TimeUnit::operator=(TimeUnit&& other) noexcept {
    if (this != &other) {
        m_type = other.m_type;
        m_value = other.m_value;
        m_delta = other.m_delta;
    }
    return *this;
}

uint32_t TimeUnit::get_delta() const{
    return m_delta;
}

uint32_t TimeUnit::m_calculate_delta() const{
    switch (m_type) {
        case Type::MILLISECOND:
            return m_value;
            break;
        case Type::SECOND:
            return m_value * 1000; 
            break;
        case Type::MINUTE:
            return m_value * 60000;
            break;
        case Type::HOUR:
            return m_value * 3600000;
            break;
        case Type::DAY:
            return m_value * 86400000;
            break;
    }
    return m_value;
}


// Init Class with pointer
// In case, Timer created before the origin value was not init.
Timer::Timer(double* _t_ptr){
    full_time_store = _t_ptr;
    full_time = 0;
    time = 0;
}
Timer::Timer(double _t){
    full_time_store = nullptr;
    full_time = _t;
    time = 0;
}
Timer::Timer(Timer& t){
    id              = t.id;
    state           = t.state;
    time            = t.time ;
    full_time       = t.full_time;
    full_time_store = t.full_time_store;
}

Timer::~Timer(){
    //INFO_MSG("Timer (id : " << id.id << ") Freed.");
}

void Timer::update(double delta){
    if(full_time_store != nullptr){
        full_time = *full_time_store;
        full_time_store = nullptr;
        INFO_MSG("Timer full time Init.");
    }

    if(this->is_enabled()){
        time = std::clamp(time + delta ,(double)0 , this->full_time);
    }
    if(this->is_enabled() && is_cycle_interval_start){
        cycle_interval = std::clamp(cycle_interval + delta ,(double)0 , this->max_cycle_interval);
    }
}

void Timer::reset(){
    time = 0;
}
void Timer::finish(){
    time = full_time;
}

double Timer::get_timeleft(){
    return full_time-time;
}
double Timer::get_time(){
    return time;
}
double Timer::get_full_time(){
    return full_time;
}

bool Timer::is_timeout(){
    return time >= full_time;
}

void Timer::modify_full_time_and_reset(double _t){
    full_time = _t;
    this->reset();
}
void Timer::modify_full_time_and_finish(double _t){
    full_time = _t;
    this->finish();
}

bool Timer::timeout_and_reset(){
    if(time >= full_time){
        this->reset();
        return true;
    }else{
        return false;
    }
}
bool Timer::is_enabled(){
    if(this->state == Timer::Mode::ST_TIMER_ENABLE){
        return true;
    }else{
        return false;
    }
}
bool Timer::is_ready_to_free(){
    if(this->state == Timer::Mode::ST_TIMER_GARBAGE || this->is_marked_garbage){
        return true;
    }else{
        return false;
    }
}
void Timer::start(){
    state = Timer::Mode::ST_TIMER_ENABLE;
}
void Timer::stop(){
    state = Timer::Mode::ST_TIMER_DISABLE;
}
void Timer::queue_free(){
    state = Timer::Mode::ST_TIMER_GARBAGE;
    is_marked_garbage = true;
}
TimerId Timer::get_id(){
    return this->id;
}
void Timer::jump_to(double _t){
    this->time = std::clamp<double>(_t, 0, full_time);
}


bool Timer::timeout_and_reset_in_cycle(uint32_t cycle, double interval){
    if(max_run_cycle == default_max_run_cycle){
        max_run_cycle = (int)cycle;
        reset_cycle();
        max_cycle_interval = interval;
        reset_interval();
    }

    if(!is_timeout()){
        return false;
    }

    is_cycle_interval_start = true;

    if(is_cycle_done()){
        return false;
    }
    
    if(is_interval_done()){
        this->reset();
        this->reset_interval();
        run_cycle++;
    }
    
    return true;
}
bool Timer::timeout_and_reset_in_cycle(uint32_t cycle, TimeUnit interval){
    return timeout_and_reset_in_cycle(cycle, interval.get_delta());
}
bool Timer::is_cycle_done(){
    return (run_cycle > max_run_cycle &&  max_run_cycle != default_max_run_cycle);
}
void Timer::reset_cycle(){
    run_cycle = 1;
}
void Timer::finish_cycle(){
    run_cycle = max_run_cycle + 100;
}
int Timer::get_current_cycle(){
    return run_cycle;
}

bool Timer::is_interval_done(){
    return cycle_interval >= max_cycle_interval;
}
void Timer::reset_interval(){
    cycle_interval = 0.0f;
    is_cycle_interval_start = false;
}
