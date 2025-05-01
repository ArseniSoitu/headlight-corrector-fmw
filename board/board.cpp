#include "board.hpp"
#include "clock.hpp"
#include "sysclock/sysclock.hpp"
#include "stm32f0xx.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_adc.h"
#include <cstdio>

using namespace board;

std::array<uint16_t, 200> Board::adcData;

Board::Board()
{
}

void Board::Init()
{
    // Register Systick callback for sys clock.
    Clock::RegisterCallback(sysclock::SysClock::IncValue);

    adcData.fill(0);

    serialInit();

    //pwmInit();

    dmaADCInit(adcData.data(), adcData.size());
    ADCInit();
};

void Board::MotorEnable()
{
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
}

void Board::MotorDisable()
{
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
}

void Board::MotorCW()
{
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
}

void Board::MotorCCW()
{
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
}

void Board::MotorSteps(std::size_t steps)
{

}

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
    uint32_t periodUs = 50000; // 20Hz

    timInitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    timInitStruct.Prescaler = 47; // 1MHz
    timInitStruct.Autoreload = clocks.PCLK1_Frequency /
        static_cast<uint32_t>(timInitStruct.Prescaler + 1) /
        static_cast<uint32_t>(1e6) * periodUs;

    timInitStruct.CounterMode = LL_TIM_COUNTERMODE_DOWN;
    LL_TIM_EnableARRPreload(TIM3);
    LL_TIM_Init(TIM3, &timInitStruct);

    timOCInitStruct.CompareValue = timInitStruct.Autoreload / (timInitStruct.Autoreload / 100); // 100 us
    timOCInitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    timOCInitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &timOCInitStruct);

    // Prepare CCR1 for dma transfer.
    dmaPwmLoadValue = timOCInitStruct.CompareValue;
    dmaPWMInit(&dmaPwmLoadValue);
    LL_TIM_SetUpdateSource(TIM3, LL_TIM_UPDATESOURCE_COUNTER);
    LL_TIM_EnableIT_UPDATE(TIM3);
    LL_TIM_EnableDMAReq_UPDATE(TIM3);
//     LL_TIM_ConfigDMABurst(TIM3, LL_TIM_DMABURST_BASEADDR_CCR1, LL_TIM_DMABURST_LENGTH_1TRANSFER);

    // Enable configured timer channel.
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);

    // Enable outputs.
    LL_TIM_EnableAllOutputs(TIM3);

    // Enable configured timer counter.
    LL_TIM_EnableCounter(TIM3);

    printf("PCLK1_Frequency: %u\r\n", clocks.PCLK1_Frequency);
    printf("timInitStruct.Prescaler: %u\r\n", timInitStruct.Prescaler);
    printf("ARR: %u\r\n", READ_REG(TIM3->ARR));
    printf("CCR: %u\r\n", READ_REG(TIM3->CCR1));
}

void Board::opInit()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    LL_TIM_InitTypeDef timInitStruct;
    LL_TIM_OC_InitTypeDef timOCInitStruct;

    // Enable TIM1 clock.
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);

    // Enable GPIOA clock.
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);


    GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_8, LL_GPIO_AF_2);

    // Default parameters for timer configuration.
    LL_TIM_StructInit(&timInitStruct);

    // Default parameters for timer channel configuration.
    LL_TIM_OC_StructInit(&timOCInitStruct);

    LL_RCC_ClocksTypeDef clocks;
    LL_RCC_GetSystemClocksFreq(&clocks);
    uint32_t periodUs = 500; // 20Hz

    timInitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    timInitStruct.Prescaler = 47; // 1MHz
    timInitStruct.Autoreload = clocks.PCLK1_Frequency /
        static_cast<uint32_t>(timInitStruct.Prescaler + 1) /
        static_cast<uint32_t>(1e6) * periodUs;

    timInitStruct.CounterMode = LL_TIM_COUNTERMODE_DOWN;
    timInitStruct.RepetitionCounter = 7; // 8 pulses (7 + 1)
    LL_TIM_Init(TIM1, &timInitStruct);

    LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_SetOnePulseMode(TIM1, LL_TIM_ONEPULSEMODE_SINGLE);

    timOCInitStruct.CompareValue = timInitStruct.Autoreload / (timInitStruct.Autoreload / 100); // 100 us
    timOCInitStruct.OCMode = LL_TIM_OCMODE_PWM2;
    timOCInitStruct.OCPolarity = LL_TIM_OCPOLARITY_LOW;
    timOCInitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &timOCInitStruct);

    // Enable configured timer channel.
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);

    // Enable outputs.
    LL_TIM_EnableAllOutputs(TIM1);

    // Enable configured timer counter.
    LL_TIM_EnableCounter(TIM1);
}

void Board::motorDirPinInit()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    // Enable GPIOA clock.
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);
}

void Board::motorEnPinInit()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    // Enable GPIOA clock.
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
}

void Board::dmaPWMInit(uint16_t* data)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_DMA_InitTypeDef DMA_InitStruct;
    LL_DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    DMA_InitStruct.MemoryOrM2MDstAddress = (uint32_t)&data;
    DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)(&(TIM3->CCR1));
    DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    DMA_InitStruct.NbData = 0xFFFF;
    DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
    DMA_InitStruct.Priority = LL_DMA_PRIORITY_HIGH;
    DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_NOINCREMENT;
    DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;

    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_3, &DMA_InitStruct);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}

void Board::ADCInit()
{
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    LL_ADC_InitTypeDef adcInitStruct;
    LL_ADC_REG_InitTypeDef adcREGInitStruct;
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_ADC_StructInit(&adcInitStruct);
    LL_ADC_REG_StructInit(&adcREGInitStruct);

    adcInitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    adcInitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    adcInitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    adcInitStruct.Resolution = LL_ADC_RESOLUTION_12B;

    adcREGInitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
    adcREGInitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    adcREGInitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    adcREGInitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    adcREGInitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;

    ADCCalibrate();

    LL_ADC_ClearFlag_ADRDY(ADC1);

    LL_ADC_Init(ADC1, &adcInitStruct);
    LL_ADC_REG_Init(ADC1, &adcREGInitStruct);

    LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_1);

    LL_ADC_Enable(ADC1);
}

void Board::ADCStart()
{
    LL_ADC_REG_StartConversion(ADC1);
//     while (!LL_DMA_IsActiveFlag_TC1(DMA1)) {
//         __NOP();
//     }
}

uint32_t Board::ADCFinished()
{
    return LL_DMA_IsActiveFlag_TC1(DMA1);
}

void Board::dmaADCInit(uint16_t* buf, size_t sz)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_DMA_InitTypeDef DMA_InitStruct;
    LL_DMA_StructInit(&DMA_InitStruct);

    DMA_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    DMA_InitStruct.MemoryOrM2MDstAddress = (uint32_t)&buf[0];
    DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)(&(ADC1->DR));
    DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    DMA_InitStruct.NbData = sz;
    DMA_InitStruct.Mode = LL_DMA_MODE_CIRCULAR;
    DMA_InitStruct.Priority = LL_DMA_PRIORITY_HIGH;
    DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;

    NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &DMA_InitStruct);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}

void Board::ADCCalibrate()
{
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN) != 0) {
        SET_BIT(ADC1->CR, ADC_CR_ADDIS);
    }

    while (READ_BIT(ADC1->CR, ADC_CR_ADEN) != 0) {
        __NOP();
    }

    CLEAR_BIT(ADC1->CFGR1, ADC_CFGR1_DMAEN);
    SET_BIT(ADC1->CR, ADC_CR_ADCAL);

    while (READ_BIT(ADC1->CR, ADC_CR_ADCAL) != 0) {
        __NOP();
    }

    adcCalibFactor = LL_ADC_REG_ReadConversionData32(ADC1);
}
