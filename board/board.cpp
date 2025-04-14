#include "board.hpp"
#include "clock.hpp"
#include "sysclock/sysclock.hpp"
#include "stm32f0xx.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_tim.h"
#include <cstdio>

using namespace board;

Board::Board()
{
}

void Board::Init()
{
    // Register Systick callback for sys clock.
    Clock::RegisterCallback(sysclock::SysClock::IncValue);

    serialInit();
    pwmInit();
};

void Board::serialInit()
{
    LL_USART_InitTypeDef USART_InitStruct;
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    // Enable USART1 clock.
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

    // Enable GPIOA clock.
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_1);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_1);

    USART_InitStruct.BaudRate = 9600;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    LL_USART_Init(USART1, &USART_InitStruct);

    LL_USART_Enable(USART1);
};

void Board::pwmInit()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    LL_TIM_InitTypeDef timInitStruct;
    LL_TIM_OC_InitTypeDef timOCInitStruct;

    // Enable TIM3 clock.
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    // Enable GPIOA clock.
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);


    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_6, LL_GPIO_AF_1);

    // Default parameters for timer configuration.
    LL_TIM_StructInit(&timInitStruct);

    // Default parameters for timer channel configuration.
    LL_TIM_OC_StructInit(&timOCInitStruct);

    LL_RCC_ClocksTypeDef clocks;
    LL_RCC_GetSystemClocksFreq(&clocks);
    uint32_t periodUs = 100; // 10kHz

    timInitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    timInitStruct.Prescaler = 0; // 48MHz
    timInitStruct.Autoreload = clocks.PCLK1_Frequency /
        static_cast<uint32_t>(timInitStruct.Prescaler + 1) /
        static_cast<uint32_t>(1e6) * periodUs;

    timInitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    LL_TIM_Init(TIM3, &timInitStruct);
    LL_TIM_EnableARRPreload(TIM3);

    timOCInitStruct.CompareValue = timInitStruct.Autoreload / 2;
    timOCInitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &timOCInitStruct);
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);

    // Enable configured timer channel.
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);

    // Enable outputs.
    LL_TIM_EnableAllOutputs(TIM3);

    // Enable configured timer counter.
    LL_TIM_EnableCounter(TIM3);

    // Initialize shadow registers by setting UG bit.
    LL_TIM_GenerateEvent_UPDATE(TIM3);

//     printf("PCLK1_Frequency: %u\r\n", clocks.PCLK1_Frequency);
//     printf("timInitStruct.Prescaler: %u\r\n", timInitStruct.Prescaler);
//     printf("ARR: %u\r\n", timInitStruct.Autoreload);
//     printf("CCR: %u\r\n", timOCInitStruct.CompareValue);
}
