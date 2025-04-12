#include "clock.hpp"

void SYSTICK_handler() {
    Clock::Callback();
}

Clock::callback Clock::cb = nullptr;

void Clock::Callback() {
    if (cb != nullptr) {
        cb();
    }
}

void Clock::RegisterCallback(Clock::callback fn) {
    cb = fn;
}
