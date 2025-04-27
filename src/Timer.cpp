#include "Timer.hpp"

Timer::Timer() : elapsed(0), running(false) {
}

void Timer::reset() {
    elapsed = std::chrono::nanoseconds(0);
    running = false;
}

void Timer::start() {
    if (!running) {
        start_time = std::chrono::high_resolution_clock::now();
        running = true;
    }
}

void Timer::stop() {
    if (running) {
        auto end_time = std::chrono::high_resolution_clock::now();
        elapsed += std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        running = false;
    }
}

int Timer::result() const {
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
}
