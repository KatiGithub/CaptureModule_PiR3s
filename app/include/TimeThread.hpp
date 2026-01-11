#pragma once
#include <thread>
#include <iostream>
#include <atomic>


class TimeThread {
    private:
        std::atomic<int64_t> current_time{0};
        std::atomic<bool> reset_flag{false};
        std::atomic<int64_t> pending_time{0};
    public:
        void getTime(int64_t* time_buffer);
        void setTime(int64_t* new_time);
        void timeLoop();
};