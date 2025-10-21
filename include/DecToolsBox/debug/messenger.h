#pragma once

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
#include <source_location>
#include <sstream>
#include <chrono>

namespace DecToolsBox{


struct CallerInfo{
public:
    CallerInfo(const std::source_location location){
        std::string file_name = location.file_name();
        file_name = file_name.replace(0,file_name.rfind("/")+1,"");
        std::stringstream ss;
        ss << file_name <<  ":" << std::to_string(location.line());
        _func_n = ss.str();
    };
    std::string _func_n;
};


inline CallerInfo GetCallerInfo(const std::source_location location =
                    std::source_location::current()){
    return CallerInfo(location);
};


inline const char* GetSystemTime(){
    static std::string time_str;
    const auto now = std::chrono::system_clock::now();
    const auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm;
    #ifdef _MSC_VER
        localtime_s(&local_tm, &now_c);
    #else
        local_tm = *std::localtime(&now_c);
    #endif
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%H:%M:%S");
    time_str = oss.str();
    return time_str.c_str();
};

inline const std::string to_upper_case(std::string p_str){
    std::string ret = p_str;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
    return ret;
};

class OutputWrapper{
public:
    virtual void print(std::string p_str){
        std::cout << p_str << std::endl;
    }
};

class ErrorOutputWrapper : public OutputWrapper{
public:
    void print(std::string p_str) override{
        std::cout << "\033[1;31m" << p_str << "\033[0m" << std::endl;
    }
};
class DebugOutputWrapper : public OutputWrapper{
public:
    void print(std::string p_str) override{
        std::cout << "\033[34m" << p_str << "\033[0m" << std::endl;
    }
};
class SuccessOutputWrapper : public OutputWrapper{
public:
    void print(std::string p_str) override{
        std::cout << "\033[92m" << p_str << "\033[0m" << std::endl;
    }
};

enum class Flag{
    FLAG__END,
};

class Messenger{
public:
    Messenger(std::unique_ptr<OutputWrapper> p_output = std::make_unique<OutputWrapper>(), std::string p_type = "DEBUG")
    : m_output(std::move(p_output))
    , m_type(to_upper_case(p_type)){};
    ~Messenger(){};


   friend Messenger& operator<<(Messenger& p_self, Flag p_flag) {
        switch (p_flag){
            case Flag::FLAG__END:{
                p_self.print();
                break;
            }
        }
        return p_self;
    }
    friend Messenger& operator<<(Messenger& p_self, CallerInfo caller_info) {
        std::string caller_str(caller_info._func_n);

        #ifdef __APPLE__
            int last_slash_pos = caller_str.find_last_of("/");
        #elif _WIN32
            int last_slash_pos = caller_str.find_last_of("\\");
        #elif __linux__
           int last_slash_pos = caller_str.find_last_of("/");
        #endif

        p_self.m_caller = caller_str.substr(last_slash_pos + 1);

        return p_self;
    }
    friend Messenger& operator<<(Messenger& p_self, bool p_data) {
        if(p_data){
            p_self.m_stream << "true";
        }else{
            p_self.m_stream << "false";
        }
        return p_self;
    }
    friend Messenger& operator<<(Messenger& p_self, char p_data) {
        std::string str = std::string(1,p_data);
        p_self.m_stream << str;
        return p_self;
    }
    friend Messenger& operator<<(Messenger& p_self, int p_data) {
        std::string str = std::to_string(p_data);
        p_self.m_stream << str;
        return p_self;
    }
    friend Messenger& operator<<(Messenger& p_self, char* p_data) noexcept {
        std::string str = p_data;
        p_self.m_stream << str;
        return p_self;
    }
    
    template<typename  T>
    friend Messenger& operator<<(Messenger& p_self, T p_data) {
        if constexpr (std::is_convertible_v<T, std::string>){
            p_self.m_stream << std::string(p_data);
        }else{
            p_self.m_stream << std::to_string(p_data);
        }
        return p_self;
    }

private:
    std::stringstream m_stream;
    std::unique_ptr<OutputWrapper> m_output;

    const std::string m_type;
    std::string m_caller = "";

    inline void init(){
        m_stream = std::stringstream("");
        m_caller = "";
    }
    inline void print(){
        std::string ret;
        ret += "[";
        ret += m_type;
        ret += "]";
        ret += "[";
        ret += GetSystemTime();
        ret += "]";
        if(m_caller != ""){
            ret += "[";
            ret += m_caller;
            ret += "] ";
        }else{
            ret+= " ";
        }
        ret += "|| ";
        ret += m_stream.str();

        m_output->print(ret);

        init();
    }
};

static Messenger debug_messenger = {std::make_unique<DebugOutputWrapper>(), "DEBUG"};
static Messenger info_messenger = {std::make_unique<OutputWrapper>(), "INFO"};
static Messenger error_messenger = {std::make_unique<ErrorOutputWrapper>(), "ERROR"};
static Messenger success_messenger = {std::make_unique<SuccessOutputWrapper>(), "SUCCESS"};


}



#define DEBUG_MSG(P_ARGS) DecToolsBox::debug_messenger << DecToolsBox::GetCallerInfo(std::source_location::current())  << P_ARGS << DecToolsBox::Flag::FLAG__END
#define INFO_MSG(P_ARGS) DecToolsBox::info_messenger << DecToolsBox::GetCallerInfo(std::source_location::current())  << P_ARGS << DecToolsBox::Flag::FLAG__END
#define ERROR_MSG(P_ARGS) DecToolsBox::error_messenger << DecToolsBox::GetCallerInfo(std::source_location::current())  << P_ARGS << DecToolsBox::Flag::FLAG__END
#define SUCCESS_MSG(P_ARGS) DecToolsBox::success_messenger << DecToolsBox::GetCallerInfo(std::source_location::current())  << P_ARGS << DecToolsBox::Flag::FLAG__END