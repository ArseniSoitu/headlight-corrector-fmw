#include "stm32f0xx.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_utils.h"

/** @addtogroup STM32F0xx_System_Private_Variables
  * @{
  */
  /* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock there is no need to
               call the 2 first functions listed above, since SystemCoreClock variable is
               updated automatically.
  */
uint32_t SystemCoreClock = 8000000;

const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};

/**
  * @}
  */

/** @addtogroup STM32F0xx_System_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F0xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  *         Initialize the default HSI clock source, vector table location and the PLL configuration is reset.
  * @param  None
  * @retval None
  */
void system_init()
{
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001U;

    #if defined (STM32F051x8) || defined (STM32F058x8)
    /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE and MCOSEL[2:0] bits */
    RCC->CFGR &= (uint32_t)0xF8FFB80CU;
    #else
    /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
    RCC->CFGR &= (uint32_t)0x08FFB80CU;
    #endif /* STM32F051x8 or STM32F058x8 */

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFFU;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFFU;

    /* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
    RCC->CFGR &= (uint32_t)0xFFC0FFFFU;

    /* Reset PREDIV[3:0] bits */
    RCC->CFGR2 &= (uint32_t)0xFFFFFFF0U;

    #if defined (STM32F072xB) || defined (STM32F078xx)
    /* Reset USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW, USBSW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFFCFE2CU;
    #elif defined (STM32F071xB)
    /* Reset USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFFFCEACU;
    #elif defined (STM32F091xC) || defined (STM32F098xx)
    /* Reset USART3SW[1:0], USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFF0FEACU;
    #elif defined (STM32F030x6) || defined (STM32F030x8) || defined (STM32F031x6) || defined (STM32F038xx) || defined (STM32F030xC)
    /* Reset USART1SW[1:0], I2C1SW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFFFFEECU;
    #elif defined (STM32F051x8) || defined (STM32F058xx)
    /* Reset USART1SW[1:0], I2C1SW, CECSW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFFFFEACU;
    #elif defined (STM32F042x6) || defined (STM32F048xx)
    /* Reset USART1SW[1:0], I2C1SW, CECSW, USBSW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFFFFE2CU;
    #elif defined (STM32F070x6) || defined (STM32F070xB)
    /* Reset USART1SW[1:0], I2C1SW, USBSW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFFFFE6CU;
    /* Set default USB clock to PLLCLK, since there is no HSI48 */
    RCC->CFGR3 |= (uint32_t)0x00000080U;
    #else
    #warning "No target selected"
    #endif

    /* Reset HSI14 bit */
    RCC->CR2 &= (uint32_t)0xFFFFFFFEU;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000U;
}

/**
   * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *
  *         (*) HSI_VALUE is a constant defined in stm32f0xx_hal.h file (default value
  *             8 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  *
  *         (**) HSE_VALUE is a constant defined in stm32f0xx_hal.h file (default value
  *              8 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *
  * @param  None
  * @retval None
  */
void system_core_clock_update()
{
    uint32_t tmp = 0, pllmull = 0, pllsource = 0, predivfactor = 0;

    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFGR & RCC_CFGR_SWS;

    switch (tmp)
    {
        case RCC_CFGR_SWS_HSI:  /* HSI used as system clock */
            SystemCoreClock = HSI_VALUE;
            break;
        case RCC_CFGR_SWS_HSE:  /* HSE used as system clock */
            SystemCoreClock = HSE_VALUE;
            break;
        case RCC_CFGR_SWS_PLL:  /* PLL used as system clock */
            /* Get PLL clock source and multiplication factor ----------------------*/
            pllmull = RCC->CFGR & RCC_CFGR_PLLMUL;
            pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
            pllmull = ( pllmull >> 18) + 2;
            predivfactor = (RCC->CFGR2 & RCC_CFGR2_PREDIV) + 1;

            if (pllsource == RCC_CFGR_PLLSRC_HSE_PREDIV)
            {
                /* HSE used as PLL clock source : SystemCoreClock = HSE/PREDIV * PLLMUL */
                SystemCoreClock = (HSE_VALUE/predivfactor) * pllmull;
            }
    #if defined(STM32F042x6) || defined(STM32F048xx) || defined(STM32F072xB) || defined(STM32F078xx) || defined(STM32F091xC) || defined(STM32F098xx)
            else if (pllsource == RCC_CFGR_PLLSRC_HSI48_PREDIV)
            {
                /* HSI48 used as PLL clock source : SystemCoreClock = HSI48/PREDIV * PLLMUL */
                SystemCoreClock = (HSI48_VALUE/predivfactor) * pllmull;
            }
    #endif /* STM32F042x6 || STM32F048xx || STM32F072xB || STM32F078xx || STM32F091xC || STM32F098xx */
            else
            {
    #if defined(STM32F042x6) || defined(STM32F048xx)  || defined(STM32F070x6) \
    || defined(STM32F078xx) || defined(STM32F071xB)  || defined(STM32F072xB) \
    || defined(STM32F070xB) || defined(STM32F091xC) || defined(STM32F098xx)  || defined(STM32F030xC)
                /* HSI used as PLL clock source : SystemCoreClock = HSI/PREDIV * PLLMUL */
                SystemCoreClock = (HSI_VALUE/predivfactor) * pllmull;
    #else
                /* HSI used as PLL clock source : SystemCoreClock = HSI/2 * PLLMUL */
                SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
    #endif /* STM32F042x6 || STM32F048xx || STM32F070x6 ||
            STM32F071xB || STM32F072xB || STM32F078xx || STM32F070xB ||
            STM32F091xC || STM32F098xx || STM32F030xC */
            }
            break;
        default: /* HSI used as system clock */
            SystemCoreClock = HSI_VALUE;
            break;
    }
    /* Compute HCLK clock frequency ----------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    /* HCLK clock frequency */
    SystemCoreClock >>= tmp;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 8000000
  *            PREDIV                         = 2
  *            PLLMUL                         = 12
  *            Flash Latency(WS)              = 1
  * @param  None
  * @retval None
  */
void clock_config()
{
    /* Set FLASH latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

    /* Enable HSI and wait for activation*/
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };

    /* Main PLL configuration and activation */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_12, LL_RCC_PREDIV_DIV_2);

    LL_RCC_PLL_Enable();
    while(LL_RCC_PLL_IsReady() != 1)
    {
    };

    /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
    };

    /* Set APB1 prescaler */
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    /* Set systick to 1ms in using frequency set to 48MHz */
    /* This frequency can be calculated through LL RCC macro */
    /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_12, LL_RCC_PREDIV_DIV_2) */
    LL_Init1msTick(48000000);

    /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(48000000);

    NVIC_SetPriority(SysTick_IRQn, 0);
    __NVIC_EnableIRQ(SysTick_IRQn);
}
