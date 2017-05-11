#include <stdio.h>
#include <string.h>

#include "py/mpstate.h"
#include "py/mphal.h"
#include "pin.h"
#include "usb.h"
#include "uart.h"
#include "Arduino.h"

mp_uint_t mp_hal_ticks_ms(void) {
  return millis();
}

void mp_hal_delay_ms(mp_uint_t ms) {
  delay(ms);
}

void mp_hal_set_interrupt_char(int c) {
    usb_vcp_set_interrupt_char(c);
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        byte c;
        if (usb_vcp_recv_byte(&c) != 0) {
            return c;
        } else if (MP_STATE_PORT(pyb_stdio_uart) != NULL && uart_rx_any(MP_STATE_PORT(pyb_stdio_uart))) {
            return uart_rx_char(MP_STATE_PORT(pyb_stdio_uart));
        }
        __WFI();
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
    if (usb_vcp_is_enabled()) {
        usb_vcp_send_strn(str, len);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    // send stdout to UART and USB CDC VCP
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn_cooked(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
    if (usb_vcp_is_enabled()) {
        usb_vcp_send_strn_cooked(str, len);
    }
}

bool mp_hal_gpio_set_af(const pin_obj_t *pin, GPIO_InitTypeDef *init, uint8_t fn, uint8_t unit) {
    mp_hal_gpio_clock_enable(pin->gpio);

    const pin_af_obj_t *af = pin_find_af(pin, fn, unit);
    if (af == NULL) {
        return false;
    }
    init->Pin = pin->pin_mask;
    init->Alternate = af->idx;
    HAL_GPIO_Init(pin->gpio, init);

    return true;
}
