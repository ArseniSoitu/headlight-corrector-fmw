#include <cstdio>
#include <chrono>
#include <array>
#include "board.hpp"
#include "sysclock/sysclock.hpp"
#include "stm32f0xx.h"
#include "stm32f0xx_ll_dma.h"

using namespace std::chrono_literals;

uint32_t adcFilter(uint16_t *buf, size_t sz)
{
//     uint32_t adcVal = 0;
//     for (int i = 0; i < sz; i++) {
//         adcVal += uint32_t(buf[i]);
//     }
//     adcVal /= sz;
//     adcVal = adcVal * 33 / 10;
//     adcVal /= 4096;
//
//     return adcVal;
    return buf[sz/2];
}

uint32_t adcVal;

void DMA1_Channel1_handler()
{
    if (LL_DMA_IsActiveFlag_TC1(DMA1)) {
//         board::Board::adcData.fill(0);
        LL_DMA_ClearFlag_TC1(DMA1);
        adcVal = board::Board::adcData[board::Board::adcData.size() / 2];
    }
}

void main_app() {
    auto board = board::Board{};
    board.Init();

    sysclock::SysClock::Reset();

    printf("Hello headlight corrector!\r\n");

    auto t  = sysclock::SysClock::now();

    auto delta = std::chrono::duration<uint64_t, std::milli>(1s);
    uint32_t s = 0;

    board.ADCStart();

    while(1) {
        auto tmstmp = sysclock::SysClock::now();
        if ((tmstmp - t) > delta) {
            t  = sysclock::SysClock::now();
            printf("%u second(s) last\r\n", ++s);

            printf("adc value %u \r\n", adcVal);
        }
    }
}
