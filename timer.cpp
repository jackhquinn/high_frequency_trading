//
// Created by Clay Shields on 7/4/22.
//

#include <chrono>
#include <iostream>
#include "timer.h"

Timer::Timer() {
    started = false;
}

void Timer::start() {
    start_time =  std::chrono::steady_clock::now();
    started = true;
}

void Timer::stop() {
    end_time =  std::chrono::steady_clock::now();
    started = false;
}

double Timer::time() {
    if (started) {
        std::cerr << "Timer not stopped" << std::endl;
        return 0;
    }
    auto diff = std::chrono::duration<double> (end_time - start_time);
    return diff.count();
}
