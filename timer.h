//
// Created by Clay Shields on 7/4/22.
//

#ifndef ORDER_COMPARE_TIMER_H
#define ORDER_COMPARE_TIMER_H


class Timer {
public:
    Timer();
    void start();
    void stop();
    double time();

private:
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> end_time;
    bool started;
};



#endif //ORDER_COMPARE_TIMER_H
