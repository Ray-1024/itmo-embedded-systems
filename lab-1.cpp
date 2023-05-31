#include <cinttypes>
#include "hal.h"

////////////////////////////////   vd_color   ////////////////////////////////

const uint8_t vd_pins_size{3};

typedef enum {
    GREEN = 0, YELLOW, RED
} vd_color;

inline void vd_update_by_index(const uint8_t &index, const GPIO_PinState &state) {
    static const uint64_t vd_pins[vd_pins_size]{GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};
    HAL_GPIO_WritePin(GPIOD, vd_pins[index], state);
}

inline void vd_reset_all() {
    for (uint8_t i = 0; i < vd_pins_size; ++i)
        vd_update_by_index(i, GPIO_PIN_RESET);
}

inline void vd_set_color(const vd_color &value) {
    vd_update_by_index(value, GPIO_PIN_SET);
}

////////////////////////////////   led   ////////////////////////////////

const uint8_t led_size{8};


inline void led_update_by_index(const uint8_t &index, const GPIO_PinState &state) {
    static const uint64_t led_pins[]{GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5,
                                     GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9,
                                     GPIO_PIN_11, GPIO_PIN_12};
    HAL_GPIO_WritePin(GPIOD, led_pins[index], state);
}

inline void led_reset_all() {
    for (uint8_t index = 0; index < led_size; ++index)
        led_update_by_index(index, GPIO_PIN_RESET);
}

////////////////////////////////   sw   ////////////////////////////////

const uint8_t sw_size{4};

inline GPIO_PinState sw_read(const uint8_t &index) {
    static const uint64_t sw_pins[]{GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};
    return HAL_GPIO_ReadPin(GPIOE, sw_pins[index]);
}

inline void sw_draw() {
    for (uint8_t index = 0; index < sw_size; ++index)
        led_update_by_index(index, sw_read(index));
}

////////////////////////////////   animation   ////////////////////////////////

inline GPIO_PinState bit_to_state(const uint8_t &mask, const uint8_t &index) {
    return (mask & ((uint8_t) 1 << index)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

inline bool animation_check_condition_by_mask(const uint8_t &mask) {
    for (uint8_t i = 0; i < sw_size; ++i)
        if (sw_read(sw_size - 1 - i) != bit_to_state(mask, i))
            return false;
    return true;
}

inline void animation_draw(uint8_t &current_frame) {
    static const uint8_t frames{10};
    uint8_t left = (current_frame <= (frames >> 1)) ? current_frame : frames - current_frame;
    led_update_by_index(left, GPIO_PIN_SET);
    led_update_by_index(left + 1, GPIO_PIN_SET);
    led_update_by_index(left + 2, GPIO_PIN_SET);
    current_frame = (current_frame + 1) % frames;
}

inline void animation_update_state(bool &current_animation_state) {
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET)
        current_animation_state = !current_animation_state;
}

////////////////////////////////   umain   ////////////////////////////////

int umain() {

    const uint64_t delay_ms{500};
    const uint8_t animation_condition_mask{0x9};
    uint8_t current_frame_number{};
    bool is_animation = false;

    while (true) {
        if (animation_check_condition_by_mask(animation_condition_mask)) {
            animation_update_state(is_animation);
            if (is_animation) {
                vd_set_color(GREEN);
                led_reset_all();
                animation_draw(current_frame_number);
            } else {
                vd_set_color(RED);
            }
        } else {
            vd_set_color(YELLOW);
            current_frame_number = 0;
            is_animation = true;
            led_reset_all();
            sw_draw();
        }
        HAL_Delay(delay_ms);
    }
}
