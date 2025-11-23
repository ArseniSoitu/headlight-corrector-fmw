#include <stdio.h>
#include "pico/stdlib.h"
#include "square_wave.pio.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 100
#endif

//#define PICO_DEFAULT_LED_PIN 7

// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#endif
}

void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
#ifdef CLOCKS_FC0_SRC_VALUE_CLK_RTC
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);
#endif

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
#ifdef CLOCKS_FC0_SRC_VALUE_CLK_RTC
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);
#endif

    // Can't measure clk_ref / xosc as it is the ref
}


static void program_pio_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_sm_config c = square_wave_program_get_default_config(offset);

    sm_config_set_clkdiv(&c, 12500.0f);

    sm_config_set_set_pins(&c, pin, 1);

    // Map the state machine's OUT pin group to one pin, namely the `pin`
    // parameter to this function.
    sm_config_set_out_pins(&c, pin, 1);
    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, pin);
    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    //pio_sm_set_clkdiv (pio, sm, 12500.0f);

    // Load our configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);
    // Set the state machine running
    pio_sm_set_enabled(pio, sm, true);
}

int main() {
    stdio_init_all();
    printf("Hello, headlight corrector!\n");

    measure_freqs();

    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    // Pick one PIO instance arbitrarily. We're also arbitrarily picking state
    // machine 0 on this PIO instance (the state machines are numbered 0 to 3
    // inclusive).
    PIO pio;
    uint sm;
    uint offset;


    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&square_wave_program, &pio, &sm, &offset, 7, 1, true);
    hard_assert(success);

    program_pio_init(pio, sm, offset, 7);
/*
    /// \tag::load_program[]
    // Load the assembled program directly into the PIO's instruction memory.
    // Each PIO instance has a 32-slot instruction memory, which all 4 state
    // machines can see. The system has write-only access.
    for (uint i = 0; i < count_of(square_wave_program_instructions); ++i)
        pio->instr_mem[i] = square_wave_program_instructions[i];
    /// \end::load_program[]

    /// \tag::clock_divider[]
    // Configure state machine 0 to run at sysclk/2.5. The state machines can
    // run as fast as one instruction per clock cycle, but we can scale their
    // speed down uniformly to meet some precise frequency target, e.g. for a
    // UART baud rate. This register has 16 integer divisor bits and 8
    // fractional divisor bits.
    pio->sm[0].clkdiv = (uint32_t) (12500 << 16);
    /// \end::clock_divider[]

    /// \tag::setup_pins[]
    // There are five pin mapping groups (out, in, set, side-set, jmp pin)
    // which are used by different instructions or in different circumstances.
    // Here we're just using SET instructions. Configure state machine 0 SETs
    // to affect GPIO 0 only; then configure GPIO0 to be controlled by PIO0,
    // as opposed to e.g. the processors.
    pio->sm[0].pinctrl =
            (1 << PIO_SM0_PINCTRL_SET_COUNT_LSB) |
            (7 << PIO_SM0_PINCTRL_SET_BASE_LSB);
    gpio_set_function(7, pio_get_funcsel(pio));
    /// \end::setup_pins[]
    

    /// \tag::start_sm[]
    // Set the state machine running. The PIO CTRL register is global within a
    // PIO instance, so you can start/stop multiple state machines
    // simultaneously. We're using the register's hardware atomic set alias to
    // make one bit high without doing a read-modify-write on the register.
    hw_set_bits(&pio->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + 0));
    /// \end::start_sm[]
    
    // Put data into state machine TX FIFO
//    pio_sm_put(pio, 0, 0xFFFFFFFF);
  */  
    printf("Goodbye, headlight corrector!\n");

    while (1) {
        pico_set_led(true);
	//pio_sm_put_blocking(pio, sm, 1);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        //pio_sm_put_blocking(pio, sm, 0);
        sleep_ms(LED_DELAY_MS);
    }
}
