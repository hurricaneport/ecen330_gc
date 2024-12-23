#include <stdio.h>
#include "soc/reg_base.h" // DR_REG_GPIO_BASE, DR_REG_IO_MUX_BASE
#include "driver/rtc_io.h" // rtc_gpio_*
#include "pin.h"

// TODO: GPIO Matrix Registers - GPIO_OUT_REG, GPIO_OUT_W1TS_REG, ...
// NOTE: Remember to enclose the macro values in parenthesis, as below
#define GPIO_PIN_BASE         (DR_REG_GPIO_BASE + 0x88)
#define GPIO_FUNC_OUT_SEL_CFG_BASE (DR_REG_GPIO_BASE + 0x530)

#define GPIO_OUT_REG          (DR_REG_GPIO_BASE + 0x04)
#define GPIO_OUT_W1TS_REG     (DR_REG_GPIO_BASE + 0x08)
#define GPIO_OUT_W1TC_REG     (DR_REG_GPIO_BASE + 0x0c)
#define GPIO_OUT1_REG         (DR_REG_GPIO_BASE + 0x10)
#define GPIO_OUT1_W1TS_REG    (DR_REG_GPIO_BASE + 0x14)
#define GPIO_OUT1_W1TC_REG    (DR_REG_GPIO_BASE + 0x18)
#define GPIO_ENABLE_REG       (DR_REG_GPIO_BASE + 0x20)
#define GPIO_ENABLE_W1TS_REG  (DR_REG_GPIO_BASE + 0x24)
#define GPIO_ENABLE_W1TC_REG  (DR_REG_GPIO_BASE + 0x28)
#define GPIO_ENABLE1_REG      (DR_REG_GPIO_BASE + 0x2c)
#define GPIO_ENABLE1_W1TS_REG (DR_REG_GPIO_BASE + 0x30)
#define GPIO_ENABLE1_W1TC_REG (DR_REG_GPIO_BASE + 0x34)
#define GPIO_IN_REG           (DR_REG_GPIO_BASE + 0x3c)
#define GPIO_IN1_REG          (DR_REG_GPIO_BASE + 0x40)

#define GPIO_PIN_REG(n) (GPIO_PIN_BASE + (n * 0x04))

#define GPIO_FUNC_OUT_SEL_CFG_REG(n) (GPIO_FUNC_OUT_SEL_CFG_BASE + (n * 0x04))

//GPIO Fields
#define GPIO_FUNC_OUT_SEL 8
#define GPIO_FUNC_OUT_SEL_W 8
#define GPIO_PIN_PAD_DRIVER 2
//GPIO Field Values
#define GPIO_FUNC_OUT_SEL_OUT 0x100

// TODO: IO MUX Registers
// HINT: Add DR_REG_IO_MUX_BASE with PIN_MUX_REG_OFFSET[n]
#define IO_MUX_REG(n)         (DR_REG_IO_MUX_BASE + PIN_MUX_REG_OFFSET[n])

// TODO: IO MUX Register Fields - FUN_WPD, FUN_WPU, ...
#define FUN_WPD 7
#define FUN_WPU 8
#define MCU_SEL_2 14
#define MCU_SEL_1 13
#define MCU_SEL_0 12
#define FUN_DRV_1 11
#define FUN_DRV_0 10
#define FUN_DRV_W 2
#define FUN_IE 9
#define MCU_IE 4

//IO MUX Register Values
#define MCU_SEL_OUT 0x2
#define FUN_DRV_NORMAL 0x2

#define REG(r) (*(volatile uint32_t *)(r))
#define REG_BITS 32

#define REG_SET_BIT(r,b) (REG(r) |= (1 << (b)))
#define REG_CLR_BIT(r,b) (REG(r) &= ~(1 << (b)))
#define REG_GET_BIT(r,b) (REG(r) & (1 << (b)))
#define REG_WRT_BITS(r,b,v,w) (REG(r) = ((REG(r) & ~(((0b1 << w) - 1) << b)) | (v << (b - w)))

// Gives byte offset of IO_MUX Configuration Register
// from base address DR_REG_IO_MUX_BASE
static const uint8_t PIN_MUX_REG_OFFSET[] = {
    0x44, 0x88, 0x40, 0x84, 0x48, 0x6c, 0x60, 0x64, // pin  0- 7
    0x68, 0x54, 0x58, 0x5c, 0x34, 0x38, 0x30, 0x3c, // pin  8-15
    0x4c, 0x50, 0x70, 0x74, 0x78, 0x7c, 0x80, 0x8c, // pin 16-23
    0x90, 0x24, 0x28, 0x2c, 0xFF, 0xFF, 0xFF, 0xFF, // pin 24-31
    0x1c, 0x20, 0x14, 0x18, 0x04, 0x08, 0x0c, 0x10, // pin 32-39
};


// Reset the configuration of a pin to not be an input or an output.
// Pull-up is enabled so the pin does not float.
int32_t pin_reset(pin_num_t pin) {
    if (rtc_gpio_is_valid_gpio(pin)) {
        // hand-off work to RTC subsystem
        rtc_gpio_deinit(pin);
        rtc_gpio_pullup_en(pin);
        rtc_gpio_pulldown_dis(pin);
    }
    //Reset GPIO_PINn_REG: All fields zero
    REG(GPIO_PIN_REG(pin)) = 0x00;

    //Reset GPIO_FUNCn_OUT_SEL_CFG_REG: GPIO_FUNCn_OUT_SEL=0x100
    REG_WRT_BITS(GPIO_FUNC_OUT_SEL_CFG_REG(pin), GPIO_FUNC_OUT_SEL, GPIO_FUNC_OUT_SEL_OUT, GPIO_FUNC_OUT_SEL_W));

    //Reset IO_MUX_x_REG: MCU_SEL=2, FUN_DRV=2, FUN_WPU=1
    REG_CLR_BIT(IO_MUX_REG(pin), MCU_SEL_2);
    REG_SET_BIT(IO_MUX_REG(pin), MCU_SEL_1);
    REG_CLR_BIT(IO_MUX_REG(pin), MCU_SEL_0);


    //REG_WRT_BITS(IO_MUX_REG(pin), FUN_DRV, FUN_DRV_NORMAL, FUN_DRV_NORMAL));
    REG_SET_BIT(IO_MUX_REG(pin), FUN_DRV_1);
    REG_CLR_BIT(IO_MUX_REG(pin), FUN_DRV_0);

    REG_SET_BIT(IO_MUX_REG(pin), FUN_WPU);
    REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPD);
    // NOTE: By default, pin should not float, save power with FUN_WPU=1

    // Now that the pin is reset, set the output level to zero
    return pin_set_level(pin, 0);
}

// Enable or disable a pull-up on the pin.
int32_t pin_pullup(pin_num_t pin, bool enable) {
    if (rtc_gpio_is_valid_gpio(pin)) {
        // hand-off work to RTC subsystem
        if (enable) return rtc_gpio_pullup_en(pin);
        else return rtc_gpio_pullup_dis(pin);
    }
    //Set or clear the FUN_WPU bit in an IO_MUX register
    if (enable) REG_SET_BIT(IO_MUX_REG(pin), FUN_WPU);
    else REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPU);
    return 0;
}

// Enable or disable a pull-down on the pin.
int32_t pin_pulldown(pin_num_t pin, bool enable) {
    if (rtc_gpio_is_valid_gpio(pin)) {
        // hand-off work to RTC subsystem
        if (enable) return rtc_gpio_pulldown_en(pin);
        else return rtc_gpio_pulldown_dis(pin);
    }
    if (enable) REG_SET_BIT(IO_MUX_REG(pin), FUN_WPD);
    else REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPD);
    return 0;
}

// Enable or disable the pin as an input signal.
int32_t pin_input(pin_num_t pin, bool enable) {
    if (enable) { //set
        REG_SET_BIT(IO_MUX_REG(pin), FUN_IE);
    }
    else { //clear
        REG_CLR_BIT(IO_MUX_REG(pin), FUN_IE);
    }
    return 0;
}

// Enable or disable the pin as an output signal.
int32_t pin_output(pin_num_t pin, bool enable) {
    if (pin < REG_BITS) { //reg out1
        if (enable) REG_SET_BIT(GPIO_ENABLE_REG, pin);
        else REG_CLR_BIT(GPIO_ENABLE_REG, pin);
    } else { //reg out2
        if (enable) REG_SET_BIT(GPIO_ENABLE1_REG, pin - REG_BITS);
        else REG_CLR_BIT(GPIO_ENABLE1_REG, pin - REG_BITS);
    }
    return 0;
}

// Enable or disable the pin as an open-drain signal.
int32_t pin_odrain(pin_num_t pin, bool enable) {
    if (enable) REG_SET_BIT(GPIO_PIN_REG(pin), GPIO_PIN_PAD_DRIVER);
    else REG_CLR_BIT(GPIO_PIN_REG(pin), GPIO_PIN_PAD_DRIVER);
    return 0;
}

// Sets the output signal level if the pin is configured as an output.
int32_t pin_set_level(pin_num_t pin, int32_t level) {

    //reg out1
    if (pin < REG_BITS) {
        if (level) REG_SET_BIT(GPIO_OUT_REG, pin);
        else REG_CLR_BIT(GPIO_OUT_REG, pin);
    } else { //reg out2
        if (level) REG_SET_BIT(GPIO_OUT1_REG, pin - REG_BITS);
        else REG_CLR_BIT(GPIO_OUT1_REG, pin - REG_BITS);
    }

    return 0;
}

// Gets the input signal level if the pin is configured as an input.
int32_t pin_get_level(pin_num_t pin) {
    if (pin < REG_BITS) {
        return REG_GET_BIT(GPIO_IN_REG, pin) ? 1 : 0;
    } else {
        return REG_GET_BIT(GPIO_IN1_REG, pin - REG_BITS) ? 1 : 0;
    }
}

// Get the value of the input registers, one pin per bit.
// The two 32-bit input registers are concatenated into a uint64_t.
uint64_t pin_get_in_reg(void) {
    uint64_t in_values = REG(GPIO_IN1_REG);
    in_values = in_values << REG_BITS;
    in_values += REG(GPIO_IN_REG);

    return in_values;
}

// Get the value of the output registers, one pin per bit.
// The two 32-bit output registers are concatenated into a uint64_t.
uint64_t pin_get_out_reg(void) {
    uint64_t in_values = REG(GPIO_OUT1_REG);
    in_values = in_values << REG_BITS;
    in_values += REG(GPIO_OUT_REG);

    return in_values;
}
