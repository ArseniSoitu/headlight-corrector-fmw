#include <cstdio>
#include <chrono>
#include "board.hpp"
#include "sysclock/sysclock.hpp"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_system.h"

using namespace std::chrono_literals;

void main_app() {
    auto board = board::Board{};
    board.Init();

    sysclock::SysClock::Reset();

    printf("Hello headlight corrector!\r\n");

    auto t  = sysclock::SysClock::now();

    auto delta = std::chrono::duration<uint64_t, std::milli>(1s);
    uint32_t i = 0;

    while(1) {
        auto tmstmp = sysclock::SysClock::now();
        if ((tmstmp - t) > delta) {
            t  = sysclock::SysClock::now();
            printf("%u second(s) last\r\n", ++i);
        }
    }
}
