#include "TimerUtils.hpp"

namespace TimerUtils{

    long long getUptimeMillis(){
        auto uptime = std::chrono::steady_clock::now() - system_start_time;
        return std::chrono::duration_cast<std::chrono::milliseconds>(uptime).count();
    }

    bool Timer::expired(){
        if(getUptimeMillis() > end_time)
        {
            return true;
        }
        return false;
    }

    void Timer::reset(){
        end_time = getUptimeMillis() + duration;
    }

    void Timer::reset(long long length){
        duration = length;
        end_time = getUptimeMillis() + duration;
    }

    void Timer::setExpired(){
        end_time = start_time + duration + 1;
    }

}