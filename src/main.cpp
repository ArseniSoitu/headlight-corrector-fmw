#include <cstdio>
#include <chrono>
#include <array>
#include "board.hpp"
#include "sysclock/sysclock.hpp"
#include "stm32f0xx.h"
#include "stm32f0xx_ll_dma.h"

using namespace std::chrono_literals;

bool lock;
std::array<uint16_t, 200> adcData;

bool motor_disable;
uint32_t motor_position = UINT32_MAX;

void DMA1_Channel1_handler()
{
    if (LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);
        if (!lock) {
            lock = true;
            adcData = board::Board::adcData;
            lock = false;
        }
    }
}

void DMA1_Channel2_3_handler()
{
    if (LL_DMA_IsActiveFlag_TC3(DMA1)) {
        LL_DMA_ClearFlag_TC3(DMA1);
        motor_disable = true;
    }
}

uint32_t filter()
{
    std::array<uint16_t, 200> data;

    while(lock) {
        __NOP();
    }

    if (!lock) {
        lock = true;
        data = adcData;
        lock = false;
    }

    uint32_t res  = 0;
    for (int i = 10; i < (data.size() - 10); i++) {
        res += data[i];
    }

    res /= (data.size() - 20);

    return res;
}

uint32_t convert(uint32_t raw)
{
    return raw * 3300 / 4095 * 4; // 3300 means 3.3V analog, 4095 max raw adc, 4 voltage divider coeff
}

typedef struct
{
    std::size_t minV;
    std::size_t maxV;
    std::size_t position;
} VoltageWindow;

int positionChange(uint32_t voltage_mV)
{
    std::array<VoltageWindow, 7> vWindows{
        {
            {1900, 3100, 0},
            {3200, 4300, 100},
            {4400, 5600, 200},
            {5700, 6900, 300},
            {7000, 8100, 400},
            {8200, 9500, 500},
            {9600, 12000, 600},
        },
    };

    uint32_t goal = UINT32_MAX;

    for (int i = 0; i < vWindows.size(); i++) {
        if (voltage_mV <= vWindows[i].maxV) {
            if (voltage_mV >= vWindows[i].minV) {
                goal = vWindows[i].position;
                break;
            }
        }
    }

    if ((goal != UINT32_MAX) && (motor_position != goal)) {
        if (motor_position == UINT32_MAX) {
            motor_position = goal;

            return 0;
        }

        int delta = goal - motor_position;
        motor_position = goal;

        return delta;
    }

    return 0;
}

void main_app()
{

    auto board = board::Board{};
    board.Init();

    sysclock::SysClock::Reset();

    printf("Hello headlight corrector!\r\n");

    auto t  = sysclock::SysClock::now();

    auto delta = std::chrono::duration<uint64_t, std::milli>(1s);
    uint32_t s = 0;

    board.ADCStart();

    int deltaSteps = 0;

    while(1) {
        auto tmstmp = sysclock::SysClock::now();
        if ((tmstmp - t) > delta) {
            t  = sysclock::SysClock::now();
            printf("%u second(s) last\r\n", ++s);

            printf("adc value %u \r\n", convert(filter()));
        }

        deltaSteps = positionChange(convert(filter()));
        board.MotorSteps(deltaSteps);

        if (motor_disable) {
            motor_disable = false;
            board.MotorDisable();
        }
    }
}
