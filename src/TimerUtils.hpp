#pragma once
#include <chrono>
#include <ctime>

namespace TimerUtils {
    const auto system_start_time = std::chrono::steady_clock::now();
    long long getUptimeSeconds();

    struct Timer{
        Timer(long long length)
            : start_time(getUptimeSeconds()),
              duration(length),
              end_time(start_time + duration)
        {}

        long long start_time;
        long long duration;
        long long end_time;

        bool expired();
        void reset();
        void reset(long long length);
        void setExpired();
    };

}
