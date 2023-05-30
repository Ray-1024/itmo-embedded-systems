#include <cinttypes>
#include "hal.h"

const uint32_t delay_ms{1000};
const uint32_t led_pins[]{GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5,
                          GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9,
                          GPIO_PIN_11, GPIO_PIN_12};
const uint32_t sw_pins[]{GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};

const uint32_t n{9};

int umain() {
    while (true) {
        for (const uint32_t &i: led_pins) {
            HAL_GPIO_WritePin(GPIOD, i, GPIO_PIN_SET);
            HAL_Delay(delay_ms);
            HAL_GPIO_WritePin(GPIOD, i, GPIO_PIN_RESET);
            HAL_Delay(delay_ms);
        }
    }

    return 0;
}
