#include "TimeThread.hpp"
#include <cstring>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>

void TimeThread::getTime(int64_t* time_buffer) {
    int64_t val = current_time.load();
    std::memcpy(time_buffer, &val, sizeof(int64_t));
}

void TimeThread::setTime(int64_t* time_buffer) {
    int64_t new_val;
    std::memcpy(&new_val, time_buffer, sizeof(int64_t));
    
    pending_time.store(new_val);
    reset_flag.store(true);

    std::cout << "New time set: " << std::to_string(new_val) << std::endl;
}

void TimeThread::timeLoop() {
    using namespace std::chrono;

    int64_t virtual_anchor = 0;
    auto real_anchor = steady_clock::now();
    
    while(true) {
        auto now = steady_clock::now();
    
        if(reset_flag.load()) {
            virtual_anchor = pending_time.load();
            real_anchor = now;
            reset_flag.store(false);
        }

        auto elapsed_ns = duration_cast<nanoseconds>(now - real_anchor).count();
        int64_t calculated_time = virtual_anchor + elapsed_ns;

        current_time.store(calculated_time);
        // std::cout << "New time calculated: " << std::to_string(calculated_time) << std::endl;
        std::this_thread::sleep_for(milliseconds(10));
    }
}