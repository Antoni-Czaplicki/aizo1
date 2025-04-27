#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer {
public:
    Timer();

    void reset();

    void start();

    void stop();

    int result() const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::nanoseconds elapsed;
    bool running;
};

#endif
