#include "TimerUtils.hpp"

namespace TimerUtils{

    long long getUptimeSeconds(){
        auto uptime = std::chrono::steady_clock::now() - system_start_time;
        return std::chrono::duration_cast<std::chrono::seconds>(uptime).count();
    }

    bool Timer::expired(){
        if(getUptimeSeconds() >= end_time)
        {
            return true;
        }
        return false;
    }

    void Timer::reset(){
        end_time = getUptimeSeconds() + duration;
    }

    void Timer::reset(long long length){
        duration = length;
        end_time = getUptimeSeconds() + duration;
    }

    void Timer::setExpired(){
        end_time = start_time + duration + 1;
    }

}