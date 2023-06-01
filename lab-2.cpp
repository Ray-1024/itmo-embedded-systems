#include <cinttypes>
#include "hal.h"

////////////////////////////////   led   ////////////////////////////////

const uint8_t led_size{8};

inline void led_write(uint8_t index, GPIO_PinState state) {
    static const uint64_t led_pins[]{GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5,
                                     GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9,
                                     GPIO_PIN_11, GPIO_PIN_12};
    HAL_GPIO_WritePin(GPIOD, led_pins[index], state);
}

inline void led_fill(uint8_t l = 0, uint8_t r = led_size - 1, GPIO_PinState state = GPIO_PIN_RESET) {
    for (; l <= r; ++l)
        led_write(l, state);
}

////////////////////////////////   sw   ////////////////////////////////

const uint8_t sw_size{4};

inline GPIO_PinState sw_read(const uint8_t &index) {
    static const uint64_t sw_pins[]{GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};
    return HAL_GPIO_ReadPin(GPIOE, sw_pins[index]);
}

inline uint64_t sw_as_number() {
    uint64_t number{};
    for (uint8_t i = 0; i < sw_size; ++i)
        if (sw_read(i) == GPIO_PIN_SET)number |= (((uint64_t) 1) << (sw_size - i - 1));
    return number;
}

////////////////////////////////   animation   ////////////////////////////////

inline void animation_draw(uint8_t &current_frame) {
    static const uint8_t frames{14};
    uint8_t width = (current_frame <= (frames >> 1)) ? current_frame : frames - current_frame;
    led_fill(0, width, GPIO_PIN_SET);
    led_fill(width + 1);
    current_frame = (current_frame + 1) % frames;
}

////////////////////////////////   TIM 6   ////////////////////////////////

const uint64_t tim6_default_time_ms{500};

void TIM6_handler() {
    static uint8_t current_frame{};
    static uint64_t T{250};
    animation_draw(current_frame);
    WRITE_REG(TIM6_ARR, tim6_default_time_ms + sw_as_number() * T);
}

void TIM6_init() {
    registerTIM6_IRQHandler(TIM6_handler);
    __enable_irq();
    WRITE_REG(TIM6_ARR, tim6_default_time_ms);
    WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM6_PSC, 0);
    WRITE_REG(TIM6_CR1, TIM_CR1_CEN);
}

////////////////////////////////   umain   ////////////////////////////////

int umain() {
    TIM6_init();
    return 0;
}
