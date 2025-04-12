#include "stm32f0xx_ll_usart.h"

extern "C" {

int _write(int fd, char *ptr, int len)
{
    (void)fd;

    for (int i = 0; i < len; i++) {
        LL_USART_TransmitData8(USART1, *ptr++);

        while (!LL_USART_IsActiveFlag_TXE(USART1)){
            __asm__("nop");
        }

        while (!LL_USART_IsActiveFlag_TC(USART1)){
            __asm__("nop");
        }
    }

    return len;
}

}
