#include "core/timer_server.h"

#include "DecToolsBox/debug/messenger.h"
#include <vector>
#include <math.h>



 
TimerServer::TimerServer(){}

TimerServer::~TimerServer(){
    INFO_MSG("Timer Count (Before Free) : " << this->get_timer_count());
    this->free_all();
    INFO_MSG("Timer Count (After Free) : " << this->get_timer_count());
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
    int i = 0;
    for(auto& timer : timer_list){
        timer->update(delta);
        i++;
    }
}



Timer* TimerServer::CreateTimer(double _full_time, bool is_finished){
    timer_list.push_back(std::shared_ptr<Timer>(new Timer(_full_time)));

    if(is_finished){
        timer_list.back()->finish();
    }
    
    return timer_list.back().get();
}
Timer* TimerServer::CreateTimer(double* _full_time_ptr, bool is_finished){
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