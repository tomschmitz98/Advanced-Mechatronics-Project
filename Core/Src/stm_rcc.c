/*
 * peripheral_clocks.c
 *
 *  Created on: Jun 8, 2023
 *      Author: Tom
 */

#include "stm_rcc.h"
#include "stm_utils.h"
#include <assert.h>
#include <stdint.h>

#define RCC_BASE(n)    *(((uint32_t *)0x40023800) + n)

#define RCC_CR         0x0
#define RCC_PLLCFGR    0x1
#define RCC_CFGR       0x2
#define RCC_CIR        0x3
#define RCC_AHB1RSTR   0x4
#define RCC_AHB2RSTR   0x5
#define RCC_AHB3RSTR   0x6
#define RCC_APB1RSTR   0x8
#define RCC_APB2RSTR   0x9
#define RCC_AHB1ENR    0xC
#define RCC_AHB2ENR    0xD
#define RCC_AHB3ENR    0xE
#define RCC_APB1ENR    0x10
#define RCC_APB2ENR    0x11
#define RCC_AHB1LPENR  0x14
#define RCC_AHB2LPENR  0x15
#define RCC_AHB3LPENR  0x16
#define RCC_APB1LPENR  0x18
#define RCC_APB2LPENR  0x19
#define RCC_BDCR       0x1C
#define RCC_CSR        0x1D
#define RCC_SSCGR      0x20
#define RCC_PLLI2SCFGR 0x21
#define RCC_PLLSAICFGR 0x22
#define RCC_DCKCFGR    0x23
#define CKGATENR       0x24
#define DCKCFGR2       0x25

#define VALID_CLK_INTERRUPTS                                                   \
    (INTERNAL_LOW_SPEED_CLK | EXTERNAL_LOW_SPEED_CLK |                         \
     INTERNAL_HIGH_SPEED_CLK | EXTERNAL_HIGH_SPEED_CLK | MAIN_PLL_CLK |        \
     I2S_PLL_CLK | SAI_PLL_CLK | CLOCK_SECURITY_SYSTEM)

#define VALID_RCC_AHB1ENR_MASK 0x60641000UL
#define VALID_RCC_AHB2ENR_MASK BIT7 | BIT0
#define VALID_RCC_AHB3ENR_MASK BIT1 | BIT0
#define VALID_RCC_APB1ENR_MASK 0x3FFFC9FFUL
#define VALID_RCC_APB2ENR_MASK 0xC77F33UL

#define INCREMENT_MASK         0xFFFE000
#define MODULATION_MASK        0x1FFF

#define MAX_RESET_ITERATIONS   10000

/* Clock control */
void enable_PLL(uint32_t pll) {
    assert(pll == SERIAL_AUDIO_INTERFACE_PLL || pll == I2S_PLL ||
           pll == MAIN_PLL);
    RCC_BASE(RCC_CR) |= ((uint32_t)1 << pll);
}

void disable_PLL(uint32_t pll) {
    assert(pll == SERIAL_AUDIO_INTERFACE_PLL || pll == I2S_PLL ||
           pll == MAIN_PLL);
    RCC_BASE(RCC_CR) &= ~((uint32_t)1 << pll);
}

bool is_PLL_enabled(uint32_t pll) {
    assert(pll == SERIAL_AUDIO_INTERFACE_PLL || pll == I2S_PLL ||
           pll == MAIN_PLL);
    return (RCC_BASE(RCC_CR) & ((uint32_t)1 << pll)) != 0;
}

bool check_PLL_locked(uint32_t pll) {
    assert(pll == SERIAL_AUDIO_INTERFACE_PLL || pll == I2S_PLL ||
           pll == MAIN_PLL);
    return (RCC_BASE(RCC_CR) & (1 << (pll + 1))) != 0;
}

void update_clock_security_system(bool enable) {
    if (enable) {
        SET_BIT(RCC_BASE(RCC_CR), 19);
        return;
    }
    CLEAR_BIT(RCC_BASE(RCC_CR), 19);
}

bool clock_security_system_enabled(void) {
    return (bool)CHECK_BIT(RCC_BASE(RCC_CR), 19);
}

void bypass_hse_oscillator(bool bypass) {
    if (!CHECK_BIT(RCC_BASE(RCC_CR), 16)) {
        if (bypass) {
            SET_BIT(RCC_BASE(RCC_CR), 18);
            return;
        }
        CLEAR_BIT(RCC_BASE(RCC_CR), 18);
    }
}

bool hse_ready(void) { return (bool)CHECK_BIT(RCC_BASE(RCC_CR), 17); }

void update_hse_status(bool on) {
    if (on) {
        SET_BIT(RCC_BASE(RCC_CR), 16);
        return;
    }
    CLEAR_BIT(RCC_BASE(RCC_CR), 16);
}

uint32_t get_high_speed_clock_calibration(void) {
    return (RCC_BASE(RCC_CR) >> 8) & 0xFF;
}

uint32_t get_high_speed_clock_trim(void) {
    return (RCC_BASE(RCC_CR) >> 3) & 0x1F;
}

void set_high_speed_clock_trim(uint32_t trim) {
    RCC_BASE(RCC_CR) &= ~((uint32_t)0x1F << 3);
    RCC_BASE(RCC_CR) |= ((trim & 0x1F) << 3);
}

void update_internal_high_speed_clock(bool enable) {
    if (enable) {
        SET_BIT(RCC_BASE(RCC_CR), 0);
        return;
    }
    CLEAR_BIT(RCC_BASE(RCC_CR), 0);
}

bool check_internal_high_speed_clock_ready(void) {
    return (bool)CHECK_BIT(RCC_BASE(RCC_CR), 1);
}

uint32_t read_clock_control_reg(uint32_t bitfield) {
    return RCC_BASE(RCC_CR) & bitfield;
}

/* PLL configuration */
static void configure_PLLR(uint16_t bitfield, uint32_t offset) {
    if (!(bitfield & BITF))
        return;
    bitfield &= 0x7;
    assert(bitfield != 0 && bitfield != 1);
    RCC_BASE(offset) &= ~(0x7 << 28);
    RCC_BASE(offset) |= (uint32_t)bitfield << 28;
}

static void configure_PLLQ(uint16_t bitfield, uint32_t offset) {
    if (!(bitfield & BITF))
        return;
    bitfield &= 0xF;
    assert(bitfield != 0 && bitfield != 1);
    RCC_BASE(offset) &= ~(0xF << 24);
    RCC_BASE(offset) |= (uint32_t)bitfield << 24;
}

static void configure_PLLSRC(uint16_t bitfield) {
    if (!(bitfield & BITF))
        return;
    if (CHECK_BIT(RCC_BASE(RCC_CR), 26))
        return;
    bitfield &= 0x1;
    RCC_BASE(RCC_PLLCFGR) &= ~(0x1 << 22);
    RCC_BASE(RCC_PLLCFGR) |= (uint32_t)bitfield << 22;
}

static void configure_PLLP(uint16_t bitfield, uint32_t offset) {
    if (!(bitfield & BITF))
        return;
    bitfield &= 0x3;
    RCC_BASE(offset) &= ~(0x3 << 16);
    RCC_BASE(offset) |= (uint32_t)bitfield << 16;
}

static void configure_PLLN(uint16_t bitfield, uint32_t offset) {
    if (!(bitfield & BITF))
        return;
    bitfield &= 0x1FF;
    assert(bitfield > 49 && bitfield < 433);
    RCC_BASE(offset) &= ~(0x1FF << 6);
    RCC_BASE(offset) |= (uint32_t)bitfield << 6;
}

static void configure_PLLM(uint16_t bitfield, uint32_t offset) {
    if (!(bitfield & BITF))
        return;
    bitfield &= 0x3F;
    assert(bitfield > 1);
    RCC_BASE(offset) &= ~0x3F;
    RCC_BASE(offset) |= (uint32_t)bitfield;
}

void configure_main_pll(pll_config_t configs) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 24))
        return;
    configure_PLLR(configs.PLLR, RCC_PLLCFGR);
    configure_PLLQ(configs.PLLQ, RCC_PLLCFGR);
    configure_PLLSRC(configs.PLLSRC);
    configure_PLLP(configs.PLLP, RCC_PLLCFGR);
    configure_PLLN(configs.PLLN, RCC_PLLCFGR);
    configure_PLLM(configs.PLLM, RCC_PLLCFGR);
}

void read_main_pll_configs(pll_config_t *configs) {
    configs->PLLR = (RCC_BASE(RCC_PLLCFGR) >> 28) & 0x7;
    configs->PLLQ = (RCC_BASE(RCC_PLLCFGR) >> 24) & 0xF;
    configs->PLLSRC = (RCC_BASE(RCC_PLLCFGR) >> 22) & 0x1;
    configs->PLLP = (RCC_BASE(RCC_PLLCFGR) >> 16) & 0x3;
    configs->PLLN = (RCC_BASE(RCC_PLLCFGR) >> 6) & 0x1FF;
    configs->PLLM = RCC_BASE(RCC_PLLCFGR) & 0x3F;
}

/* Clock Configuration */
void update_MCO2(uint8_t bitfield) {
    bitfield &= 0x3;
    RCC_BASE(RCC_CFGR) &= ~(0x3 << 30);
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield << 30;
}

void update_MCO2PRE(uint8_t bitfield) {
    bitfield &= 0x7;
    RCC_BASE(RCC_CFGR) &= ~(0x7 << 27);
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield << 27;
}

void update_MCO1(uint8_t bitfield) {
    bitfield &= 0x3;
    RCC_BASE(RCC_CFGR) &= ~(0x3 << 21);
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield << 21;
}

void update_MCO1PRE(uint8_t bitfield) {
    bitfield &= 0x7;
    RCC_BASE(RCC_CFGR) &= ~(0x7 << 24);
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield << 24;
}

void update_RTCPRE(uint8_t bitfield) {
    bitfield &= 0x1F;
    RCC_BASE(RCC_CFGR) &= ~(0x1F << 16);
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield << 16;
}

void update_PPRE2(uint8_t bitfield) {
    bitfield &= 0x7;
    RCC_BASE(RCC_CFGR) &= ~(0x7 << 13);
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield << 13;
}

void update_PPRE1(uint8_t bitfield) {
    bitfield &= 0x7;
    RCC_BASE(RCC_CFGR) &= ~(0x7 << 10);
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield << 10;
}

void update_HPRE(uint8_t bitfield) {
    bitfield &= 0xF;
    RCC_BASE(RCC_CFGR) &= ~(0xF << 4);
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield << 4;
}

void update_SW(uint8_t bitfield) {
    bitfield &= 0x3;
    RCC_BASE(RCC_CFGR) &= ~0x3;
    RCC_BASE(RCC_CFGR) |= (uint32_t)bitfield;
}

uint32_t read_clk_configs(uint32_t bitfield) {
    return RCC_BASE(RCC_CFGR) & bitfield;
}

/* Clock interrupts */
void clear_clock_interrupt(uint32_t mask) {
    RCC_BASE(RCC_CIR) |= (mask & VALID_CLK_INTERRUPTS) << 16;
}

void enable_clock_interrupt(uint32_t mask) {
    uint32_t valid_mask = VALID_CLK_INTERRUPTS ^ CLOCK_SECURITY_SYSTEM;
    RCC_BASE(RCC_CIR) |= (mask & valid_mask) << 8;
}

uint32_t check_clock_interrupt(uint32_t mask) {
    return RCC_BASE(RCC_CIR) & (mask & VALID_CLK_INTERRUPTS);
}

/* Peripheral reset */
void reset_gpio_clock(gpio_bank_t bank, bool hold_reset) {
    RCC_BASE(RCC_AHB1RSTR) |= 1 << (((uint32_t)bank) >> 8);
    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB1RSTR) &= ~(1 << (((uint32_t)bank) >> 8));
}

void reset_usb_otg_hs(bool hold_reset) {
    RCC_BASE(RCC_AHB1RSTR) |= 1 << 29;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB1RSTR) &= ~(1 << 29);
}

void reset_dma1(bool hold_reset) {
    RCC_BASE(RCC_AHB1RSTR) |= 1 << 21;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB1RSTR) &= ~(1 << 21);
}

void reset_dma2(bool hold_reset) {
    RCC_BASE(RCC_AHB1RSTR) |= 1 << 22;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB1RSTR) &= ~(1 << 22);
}

void reset_crc(bool hold_reset) {
    RCC_BASE(RCC_AHB1RSTR) |= 1 << 12;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB1RSTR) &= ~(1 << 12);
}

void reset_usb_otg_fs(bool hold_reset) {
    RCC_BASE(RCC_AHB2RSTR) |= 1 << 7;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB2RSTR) &= ~(1 << 7);
}

void reset_camera_interface(bool hold_reset) {
    RCC_BASE(RCC_AHB2RSTR) |= 1;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB2RSTR) &= ~1;
}

void reset_quad_spi(bool hold_reset) {
    RCC_BASE(RCC_AHB3RSTR) |= 1 << 1;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB3RSTR) &= ~(1 << 1);
}

void reset_flexible_memory_controller(bool hold_reset) {
    RCC_BASE(RCC_AHB3RSTR) |= 1;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_AHB3RSTR) &= ~1;
}

void reset_dac(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 29;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 29);
}

void reset_pwr(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 28;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 28);
}

void reset_cec(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 27;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 27);
}

void reset_can2(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 26;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 26);
}

void reset_can1(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 25;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 25);
}

void reset_fmpi2c(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 24;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 24);
}

void reset_i2c3(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 23;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 23);
}

void reset_i2c2(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 22;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 22);
}

void reset_i2c1(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 21;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 21);
}

void reset_uart5(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 20;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 20);
}

void reset_uart4(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 19;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 19);
}

void reset_usart3(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 18;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 18);
}

void reset_usart2(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 17;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 17);
}

void reset_spdif_rx(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 16;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 16);
}

void reset_spi3(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 15;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 15);
}

void reset_spi2(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 14;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 14);
}

void reset_window_watchdog(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 11;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 11);
}

void reset_timer14(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 8;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 8);
}

void reset_timer13(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 7;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 7);
}

void reset_timer12(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 6;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 6);
}

void reset_timer7(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 5;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 5);
}

void reset_timer6(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 4;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 4);
}

void reset_timer5(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 3;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 3);
}

void reset_timer4(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 2;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 2);
}

void reset_timer3(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1 << 1;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~(1 << 1);
}

void reset_timer2(bool hold_reset) {
    RCC_BASE(RCC_APB1RSTR) |= 1;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB1RSTR) &= ~1;
}

void reset_sai2(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 23;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 23);
}

void reset_sai1(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 22;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 22);
}

void reset_timer11(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 18;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 18);
}

void reset_timer10(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 17;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 17);
}

void reset_timer9(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 16;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 16);
}

void reset_sys_config(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 14;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 14);
}

void reset_spi4(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 13;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 13);
}

void reset_spi1(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 12;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 12);
}

void reset_sdio(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 11;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 11);
}

void reset_adc(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 8;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 8);
}

void reset_usart6(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 5;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 5);
}

void reset_usart1(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 4;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 4);
}

void reset_timer8(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1 << 1;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~(1 << 1);
}

void reset_timer1(bool hold_reset) {
    RCC_BASE(RCC_APB2RSTR) |= 1;

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_APB2RSTR) &= ~1;
}

/* Peripheral clock enable */
void enable_gpio_clock(gpio_bank_t bank) {
    RCC_BASE(RCC_AHB1ENR) |= 1 << (((uint32_t)bank) >> 8);
}

void disable_gpio_clock(gpio_bank_t bank) {
    RCC_BASE(RCC_AHB1ENR) &= ~(1 << (((uint32_t)bank) >> 8));
}

bool check_gpio_clock(gpio_bank_t bank) {
    return (bool)(RCC_BASE(RCC_AHB1ENR) >> (((uint32_t)bank) >> 8) & 0x1);
}

void enable_peripheral_clock(uint32_t reg, uint32_t peripheral) {
    uint32_t valid_mask, offset;

    switch (reg) {
    case 0:
        offset = RCC_AHB1ENR;
        valid_mask = VALID_RCC_AHB1ENR_MASK;
        break;
    case 1:
        offset = RCC_AHB2ENR;
        valid_mask = VALID_RCC_AHB2ENR_MASK;
        break;
    case 2:
        offset = RCC_AHB3ENR;
        valid_mask = VALID_RCC_AHB3ENR_MASK;
        break;
    case 3:
        offset = RCC_APB1ENR;
        valid_mask = VALID_RCC_APB1ENR_MASK;
        break;
    case 4:
        offset = RCC_APB2ENR;
        valid_mask = VALID_RCC_APB2ENR_MASK;
        break;
    default:
        assert(0);
        break;
    }

    assert(peripheral & valid_mask);

    RCC_BASE(offset) |= peripheral;
}

void disable_peripheral_clock(uint32_t reg, uint32_t peripheral) {
    uint32_t valid_mask, offset;

    switch (reg) {
    case 0:
        offset = RCC_AHB1ENR;
        valid_mask = VALID_RCC_AHB1ENR_MASK;
        break;
    case 1:
        offset = RCC_AHB2ENR;
        valid_mask = VALID_RCC_AHB2ENR_MASK;
        break;
    case 2:
        offset = RCC_AHB3ENR;
        valid_mask = VALID_RCC_AHB3ENR_MASK;
        break;
    case 3:
        offset = RCC_APB1ENR;
        valid_mask = VALID_RCC_APB1ENR_MASK;
        break;
    case 4:
        offset = RCC_APB2ENR;
        valid_mask = VALID_RCC_APB2ENR_MASK;
        break;
    default:
        assert(0);
        break;
    }

    assert(peripheral & valid_mask);

    RCC_BASE(offset) &= ~peripheral;
}

bool check_peripheral_clock(uint32_t reg, uint32_t peripheral) {
    uint32_t valid_mask, offset;

    switch (reg) {
    case 0:
        offset = RCC_AHB1ENR;
        valid_mask = VALID_RCC_AHB1ENR_MASK;
        break;
    case 1:
        offset = RCC_AHB2ENR;
        valid_mask = VALID_RCC_AHB2ENR_MASK;
        break;
    case 2:
        offset = RCC_AHB3ENR;
        valid_mask = VALID_RCC_AHB3ENR_MASK;
        break;
    case 3:
        offset = RCC_APB1ENR;
        valid_mask = VALID_RCC_APB1ENR_MASK;
        break;
    case 4:
        offset = RCC_APB2ENR;
        valid_mask = VALID_RCC_APB2ENR_MASK;
        break;
    default:
        assert(0);
        break;
    }

    assert(peripheral & valid_mask);

    return (RCC_BASE(offset) & peripheral) != 0;
}

void enable_peripheral_low_power_clock(uint32_t reg, uint32_t peripheral) {
    uint32_t valid_mask, offset;

    switch (reg) {
    case 0:
        offset = RCC_AHB1LPENR;
        valid_mask = VALID_RCC_AHB1ENR_MASK;
        break;
    case 1:
        offset = RCC_AHB2LPENR;
        valid_mask = VALID_RCC_AHB2ENR_MASK;
        break;
    case 2:
        offset = RCC_AHB3LPENR;
        valid_mask = VALID_RCC_AHB3ENR_MASK;
        break;
    case 3:
        offset = RCC_APB1LPENR;
        valid_mask = VALID_RCC_APB1ENR_MASK;
        break;
    case 4:
        offset = RCC_APB2LPENR;
        valid_mask = VALID_RCC_APB2ENR_MASK;
        break;
    default:
        assert(0);
        break;
    }

    assert(peripheral & valid_mask);

    RCC_BASE(offset) |= peripheral;
}

void disable_peripheral_low_power_clock(uint32_t reg, uint32_t peripheral) {
    uint32_t valid_mask, offset;

    switch (reg) {
    case 0:
        offset = RCC_AHB1LPENR;
        valid_mask = VALID_RCC_AHB1ENR_MASK;
        break;
    case 1:
        offset = RCC_AHB2LPENR;
        valid_mask = VALID_RCC_AHB2ENR_MASK;
        break;
    case 2:
        offset = RCC_AHB3LPENR;
        valid_mask = VALID_RCC_AHB3ENR_MASK;
        break;
    case 3:
        offset = RCC_APB1LPENR;
        valid_mask = VALID_RCC_APB1ENR_MASK;
        break;
    case 4:
        offset = RCC_APB2LPENR;
        valid_mask = VALID_RCC_APB2ENR_MASK;
        break;
    default:
        assert(0);
        break;
    }

    assert(peripheral & valid_mask);

    RCC_BASE(offset) &= ~peripheral;
}

bool check_peripheral_low_power_clock(uint32_t reg, uint32_t peripheral) {
    uint32_t valid_mask, offset;

    switch (reg) {
    case 0:
        offset = RCC_AHB1LPENR;
        valid_mask = VALID_RCC_AHB1ENR_MASK;
        break;
    case 1:
        offset = RCC_AHB2LPENR;
        valid_mask = VALID_RCC_AHB2ENR_MASK;
        break;
    case 2:
        offset = RCC_AHB3LPENR;
        valid_mask = VALID_RCC_AHB3ENR_MASK;
        break;
    case 3:
        offset = RCC_APB1LPENR;
        valid_mask = VALID_RCC_APB1ENR_MASK;
        break;
    case 4:
        offset = RCC_APB2LPENR;
        valid_mask = VALID_RCC_APB2ENR_MASK;
        break;
    default:
        assert(0);
        break;
    }

    assert(peripheral & valid_mask);

    return (RCC_BASE(offset) & peripheral) != 0;
}

/* Backup domain */
void reset_backup_domain(bool hold_reset) {
    // TODO: Check power registers
    RCC_BASE(RCC_BDCR) |= UPPER16BITS(BIT0);

    if (hold_reset)
        return;

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_BDCR) &= ~UPPER16BITS(BIT0);
}

void enable_rtc_clock(void) {
    // TODO: Check power registers
    RCC_BASE(RCC_BDCR) |= BITF;
}

void disable_rtc_clock(void) {
    // TODO: Check power registers
    RCC_BASE(RCC_BDCR) &= ~BITF;
}

void select_rtc_clock(rtc_clocks_t selection) {
    // TODO: Check power registers
    RCC_BASE(RCC_BDCR) &= ~(BIT9 | BIT8);
    RCC_BASE(RCC_BDCR) |= ((uint32_t)selection) << 8;
}

void set_lse_mode(bool mode) {
    // TODO: Check power registers
    RCC_BASE(RCC_BDCR) &= ~BIT3;

    if (mode) {
        RCC_BASE(RCC_BDCR) |= BIT3;
    }
}

void set_lse_bypass(bool bypass) {
    // TODO: Check power registers
    RCC_BASE(RCC_BDCR) &= ~BIT2;

    if (bypass) {
        RCC_BASE(RCC_BDCR) |= BIT2;
    }
}

bool lse_ready(void) { return (RCC_BASE(RCC_BDCR) & BIT1) != 0; }

void enable_lse(void) {
    // TODO: Check power registers
    RCC_BASE(RCC_BDCR) |= BIT0;
}

void disable_lse(void) {
    // TODO: Check power registers
    RCC_BASE(RCC_BDCR) &= ~BIT0;
}

uint32_t read_backup_domain(uint32_t mask) { return RCC_BASE(RCC_BDCR) & mask; }

/* Clock control and status */
bool check_clock_flag(uint32_t clk_flag) {
    return (RCC_BASE(RCC_CSR) & clk_flag) != 0;
}

void clear_clock_flags(void) {
    RCC_BASE(RCC_CSR) |= UPPER16BITS(BIT8);

    for (uint32_t i = 0; i < MAX_RESET_ITERATIONS; i++)
        ;

    RCC_BASE(RCC_CSR) &= ~UPPER16BITS(BIT8);
}

void enable_low_speed_oscillator(void) { RCC_BASE(RCC_CSR) |= BIT0; }

void disable_low_speed_oscillator(void) { RCC_BASE(RCC_CSR) &= ~BIT0; }

uint32_t read_clock_control_and_status_register(uint32_t mask) {
    return RCC_BASE(RCC_CSR) & mask;
}

/* spread spectrum clock generation */
void enable_spread_spectrum(void) {
    if (is_PLL_enabled(MAIN_PLL))
        return;
    RCC_BASE(RCC_SSCGR) |= UPPER16BITS(BITF);
}

void disable_spread_spectrum(void) {
    if (is_PLL_enabled(MAIN_PLL))
        return;
    RCC_BASE(RCC_SSCGR) &= ~UPPER16BITS(BITF);
}

bool is_spread_spectrum_enabled(void) {
    return (RCC_BASE(RCC_SSCGR) & UPPER16BITS(BITF)) != 0;
}

void update_step_increment(uint16_t step_size) {
    uint32_t step = (((uint32_t)step_size) << 13) & INCREMENT_MASK;
    if (is_PLL_enabled(MAIN_PLL))
        return;
    RCC_BASE(RCC_SSCGR) &= ~INCREMENT_MASK;
    RCC_BASE(RCC_SSCGR) |= step;
}

void update_modulation_period(uint16_t period) {
    if (is_PLL_enabled(MAIN_PLL))
        return;
    RCC_BASE(RCC_SSCGR) &= ~MODULATION_MASK;
    RCC_BASE(RCC_SSCGR) |= ((uint32_t)period) & MODULATION_MASK;
}

uint32_t check_spread_spectrum_reg(uint32_t mask) {
    return RCC_BASE(RCC_SSCGR) & mask;
}

/* PLLI2S Configuration */
void configure_12s_pll(pll_config_t configs) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 26))
        return;

    configure_PLLR(configs.PLLR, RCC_PLLI2SCFGR);
    configure_PLLQ(configs.PLLQ, RCC_PLLI2SCFGR);
    configure_PLLP(configs.PLLP, RCC_PLLI2SCFGR);
    configure_PLLN(configs.PLLN, RCC_PLLI2SCFGR);
    configure_PLLM(configs.PLLM, RCC_PLLI2SCFGR);
}

void read_i2s_pll_configs(pll_config_t *configs) {
    configs->PLLR = (RCC_BASE(RCC_PLLI2SCFGR) >> 28) & 0x7;
    configs->PLLQ = (RCC_BASE(RCC_PLLI2SCFGR) >> 24) & 0xF;
    configs->PLLP = (RCC_BASE(RCC_PLLI2SCFGR) >> 16) & 0x3;
    configs->PLLN = (RCC_BASE(RCC_PLLI2SCFGR) >> 6) & 0x1FF;
    configs->PLLM = RCC_BASE(RCC_PLLI2SCFGR) & 0x3F;
}

/* PLLSAI Configuration */
void configure_sai_pll(pll_config_t configs) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 28))
        return;

    configure_PLLR(configs.PLLR, RCC_PLLSAICFGR);
    configure_PLLQ(configs.PLLQ, RCC_PLLSAICFGR);
    configure_PLLP(configs.PLLP, RCC_PLLSAICFGR);
    configure_PLLN(configs.PLLN, RCC_PLLSAICFGR);
    configure_PLLM(configs.PLLM, RCC_PLLSAICFGR);
}

void read_sai_pll_configs(pll_config_t *configs) {
    configs->PLLR = (RCC_BASE(RCC_PLLSAICFGR) >> 28) & 0x7;
    configs->PLLQ = (RCC_BASE(RCC_PLLSAICFGR) >> 24) & 0xF;
    configs->PLLP = (RCC_BASE(RCC_PLLSAICFGR) >> 16) & 0x3;
    configs->PLLN = (RCC_BASE(RCC_PLLSAICFGR) >> 6) & 0x1FF;
    configs->PLLM = RCC_BASE(RCC_PLLSAICFGR) & 0x3F;
}

void update_i2s2_source(uint32_t src) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 28) || CHECK_BIT(RCC_BASE(RCC_CR), 26) ||
        CHECK_BIT(RCC_BASE(RCC_CR), 24))
        return;

    RCC_BASE(RCC_DCKCFGR) &= ~UPPER16BITS(BITC | BITB);
    RCC_BASE(RCC_DCKCFGR) |= (src & 0x3) << 27;
}

void update_i2s1_source(uint32_t src) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 28) || CHECK_BIT(RCC_BASE(RCC_CR), 26) ||
        CHECK_BIT(RCC_BASE(RCC_CR), 24))
        return;

    RCC_BASE(RCC_DCKCFGR) &= ~UPPER16BITS(BITA | BIT9);
    RCC_BASE(RCC_DCKCFGR) |= (src & 0x3) << 25;
}

void update_timer_prescaler_selection(uint32_t selection) {
    RCC_BASE(RCC_DCKCFGR) &= ~UPPER16BITS(BIT8);
    RCC_BASE(RCC_DCKCFGR) |= (selection & 0x1) << 24;
}

void update_sai2_source(uint32_t src) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 28) || CHECK_BIT(RCC_BASE(RCC_CR), 26) ||
        CHECK_BIT(RCC_BASE(RCC_CR), 24))
        return;

    RCC_BASE(RCC_DCKCFGR) &= ~UPPER16BITS(BIT7 | BIT6);
    RCC_BASE(RCC_DCKCFGR) |= (src & 0x3) << 22;
}

void update_sai1_source(uint32_t src) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 28) || CHECK_BIT(RCC_BASE(RCC_CR), 26) ||
        CHECK_BIT(RCC_BASE(RCC_CR), 24))
        return;

    RCC_BASE(RCC_DCKCFGR) &= ~UPPER16BITS(BIT5 | BIT4);
    RCC_BASE(RCC_DCKCFGR) |= (src & 0x3) << 20;
}

void update_sai_division_factor(uint32_t div) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 28))
        return;

    RCC_BASE(RCC_DCKCFGR) &= ~(BITC | BITB | BITA | BIT9 | BIT8);
    RCC_BASE(RCC_DCKCFGR) |= (div & 0x1F) << 8;
}

void update_i2s_division_factor(uint32_t div) {
    if (CHECK_BIT(RCC_BASE(RCC_CR), 26))
        return;

    RCC_BASE(RCC_DCKCFGR) &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
    RCC_BASE(RCC_DCKCFGR) |= div & 0x1F;
}

void update_spdif_rx_clock_section(uint32_t selection) {
    RCC_BASE(DCKCFGR2) &= ~UPPER16BITS(BITD);
    RCC_BASE(DCKCFGR2) |= (selection & 0x1) << 29;
}

void update_sdio_selection(uint32_t selection) {
    RCC_BASE(DCKCFGR2) &= ~UPPER16BITS(BITC);
    RCC_BASE(DCKCFGR2) |= (selection & 0x1) << 28;
}

void update_clk_48mhz_selection(uint32_t selection) {
    RCC_BASE(DCKCFGR2) &= ~UPPER16BITS(BITB);
    RCC_BASE(DCKCFGR2) |= (selection & 0x1) << 27;
}

void update_cec_selection(uint32_t selection) {
    RCC_BASE(DCKCFGR2) &= ~UPPER16BITS(BITA);
    RCC_BASE(DCKCFGR2) |= (selection & 0x1) << 26;
}
void update_fmpi2c1_selection(uint32_t selection) {
    RCC_BASE(DCKCFGR2) &= ~UPPER16BITS(BIT7 | BIT6);
    RCC_BASE(DCKCFGR2) |= (selection & 0x3) << 22;
}

/* clocks gated enable */
void enable_rcc_clock(clocks_t clock) {
    CLEAR_BIT(RCC_BASE(CKGATENR), (uint32_t)clock);
}

void disable_rcc_clock(clocks_t clock) {
    SET_BIT(RCC_BASE(CKGATENR), (uint32_t)clock);
}

bool is_rcc_clock_enabled(clocks_t clock) {
    return !CHECK_BIT(RCC_BASE(CKGATENR), (uint32_t)clock);
}
