/*
 * peripheral_clocks.h
 *
 *  Created on: Jun 8, 2023
 *      Author: Tom
 */

#ifndef STM_RCC_H_
#define STM_RCC_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm_utils.h"

/* Clock control macros */
#define SERIAL_AUDIO_INTERFACE_PLL 28
#define I2S_PLL 26
#define MAIN_PLL 24

#define PLL_ON 1
#define PLL_OFF 0

/* PLL configuration macros */
#define ENABLE_PLL BITF

/* Clock interrupts macros */
#define INTERNAL_LOW_SPEED_CLK BIT0
#define EXTERNAL_LOW_SPEED_CLK BIT1
#define INTERNAL_HIGH_SPEED_CLK BIT2
#define EXTERNAL_HIGH_SPEED_CLK BIT3
#define MAIN_PLL_CLK BIT4
#define I2S_PLL_CLK BIT5
#define SAI_PLL_CLK BIT6
#define CLOCK_SECURITY_SYSTEM BIT7

/* Peripheral Clock Enable macros */
#define USB_OTG_HSULPI_EN 0, UPPER16BITS(BITE)
#define USB_OTG_HS_EN 0, UPPER16BITS(BITD)
#define DMA2_EN 0, UPPER16BITS(BIT6)
#define DMA1_EN 0, UPPER16BITS(BIT5)
#define BACKUP_SRAM_EN 0, UPPER16BITS(BIT2)
#define CRC_EN 0, BITC
#define USB_OTG_FS_EN 1, BIT7
#define DCMI_EN 1, BIT0
#define QUADSPI_EN 2, BIT1
#define FLEXIBLE_MEM_CONTROLLER_EN 2, BIT0
#define DAC_EN 3, UPPER16BITS(BITD)
#define PWR_EN 3, UPPER16BITS(BITC)
#define CEC_EN 3, UPPER16BITS(BITB)
#define CAN2_EN 3, UPPER16BITS(BITA)
#define CAN1_EN 3, UPPER16BITS(BIT9)
#define FMP12C_EN 3, UPPER16BITS(BIT8)
#define I2C3_EN 3, UPPER16BITS(BIT7)
#define I2C2_EN 3, UPPER16BITS(BIT6)
#define I2C1_EN 3, UPPER16BITS(BIT5)
#define UART5_EN 3, UPPER16BITS(BIT4)
#define UART4_EN 3, UPPER16BITS(BIT3)
#define UART3_EN 3, UPPER16BITS(BIT2)
#define UART2_EN 3, UPPER16BITS(BIT1)
#define SPDIFRX_EN 3, UPPER16BITS(BIT0)
#define SPI3_EN 3, BITF
#define SPI2_EN 3, BITE
#define WWDG_EN 3, BITB
#define TIM14_EN 3, BIT8
#define TIM13_EN 3, BIT7
#define TIM12_EN 3, BIT6
#define TIM7_EN 3, BIT5
#define TIM6_EN 3, BIT4
#define TIM5_EN 3, BIT3
#define TIM4_EN 3, BIT2
#define TIM3_EN 3, BIT1
#define TIM2_EN 3, BIT0
#define SAI2_EN 4, UPPER16BITS(BIT7)
#define SAI1_EN 4, UPPER16BITS(BIT6)
#define TIM11_EN 4, UPPER16BITS(BIT2)
#define TIM10_EN 4, UPPER16BITS(BIT1)
#define TIM9_EN 4, UPPER16BITS(BIT0)
#define SYSCONFIG_EN 4, BITE
#define SPI4_EN 4, BITD
#define SPI1_EN 4, BITC
#define SDIO_EN 4, BITB
#define ADC3_EN 4, BITA
#define ADC2_EN 4, BIT9
#define ADC1_EN 4, BIT8
#define UART6_EN 4, BIT5
#define UART1_EN 4, BIT4
#define TIM8_EN 4, BIT1
#define TIM1_EN 4, BIT0

/* Backup domain control macros */
#define LOW_POWER false
#define HIGH_DRIVE true

/* Clock control and status macros */
#define LOW_POWER_RESET UPPER16BITS(BITF)
#define WINDOW_WATCHDOG_RESET UPPER16BITS(BITE)
#define INDEPENDENT_WATCHDOG_RESET UPPER16BITS(BITD)
#define SOFTWARE_RESET UPPER16BITS(BITC)
#define POR_PDR_RESET UPPER16BITS(BITB)
#define PIN_RESET UPPER16BITS(BITA)
#define BOR_RESET UPPER16BITS(BIT9)
#define INTERNAL_LOW_SPEED_OSCILLATOR BIT1

/* Spread spectrum macros */
#define CENTER_SPREAD false
#define DOWN_SPREAD true

/* Custom types */
typedef struct {
	/* IMPORTANT: BIT 15 MUST BE SET WHEN MODIFYING PLL BITFIELDS.
	 * FAILURE TO DO SO WILL RESULT IN THE FIELD IN THE REGISTER NOT BEING MODIFIED */

	/*  Main PLL division factor for I2Ss, SAIs, SYSTEM and SPDIF-Rx clocks */
	uint16_t PLLR;

	/* Main PLL (PLL) division factor for USB OTG FS, SDIO clocks
	 * The USB OTG FS requires a 48 MHz clock to work correctly. The SDIO needs a
	   frequency lower than or equal to 48 MHz to work correctly */
	uint16_t PLLQ;

	/* Main PLL(PLL) and audio PLL (PLLI2S) entry clock source */
	uint16_t PLLSRC;

	/* Main PLL (PLL) division factor for main system clock */
	uint16_t PLLP;

	/* Main PLL (PLL) multiplication factor for VCO */
	uint16_t PLLN;

	/* Division factor for the main PLL (PLL) input clock */
	uint16_t PLLM;
} pll_config_t;

typedef enum {
	no_clock = 0x0,
	lse_clock = 0x1,
	lsi_clock = 0x2,
	hse_clock = 0x3
} rtc_clocks_t;

typedef enum {
	CLK_RCC = 6,
	CLK_FLASH_INTERFACE = 5,
	CLK_SRAM = 4,
	CLK_SPARE = 3,
	CLK_CORTEX_M4 = 2,
	CLK_AHB_APB2_BRIDGE = 1,
	CLK_AHB_APB1_BRIDGE = 0
} clocks_t;


/* Clock control */
void enable_PLL(uint32_t pll);
void disable_PLL(uint32_t pll);
bool is_PLL_enabled(uint32_t pll);
bool check_PLL_locked(uint32_t pll);
void update_clock_security_system(bool enable);
bool clock_security_system_enabled(void);
void bypass_hse_oscillator(bool bypass);
bool hse_ready(void);
void update_hse_status(bool on);
uint32_t get_high_speed_clock_calibration(void);
uint32_t get_high_speed_clock_trim(void);
void set_high_speed_clock_trim(uint32_t trim);
void update_internal_high_speed_clock(bool enable);
bool check_internal_high_speed_clock_ready(void);
uint32_t read_clock_control_reg(uint32_t bitfield);

/* PLL configuration */
void configure_main_pll(pll_config_t configs);
void read_main_pll_configs(pll_config_t *configs);

/* Clock Configuration */
void update_MCO2(uint8_t bitfield);
void update_MCO2PRE(uint8_t bitfield);
void update_MCO1(uint8_t bitfield);
void update_MCO1PRE(uint8_t bitfield);
void update_RTCPRE(uint8_t bitfield);
void update_PPRE2(uint8_t bitfield);
void update_PPRE1(uint8_t bitfield);
void update_HPRE(uint8_t bitfield);
void update_SW(uint8_t bitfield);
uint32_t read_clk_configs(uint32_t bitfield);

/* Clock interrupts */
void clear_clock_interrupt(uint32_t mask);
void enable_clock_interrupt(uint32_t mask);
uint32_t check_clock_interrupt(uint32_t mask);

/* Peripheral reset */
void reset_gpio_clock(gpio_bank_t bank, bool hold_reset);
void reset_usb_otg_hs(bool hold_reset);
void reset_dma1(bool hold_reset);
void reset_dma2(bool hold_reset);
void reset_crc(bool hold_reset);

void reset_usb_otg_fs(bool hold_reset);
void reset_camera_interface(bool hold_reset);

void reset_quad_spi(bool hold_reset);
void reset_flexible_memory_controller(bool hold_reset);

void reset_dac(bool hold_reset);
void reset_pwr(bool hold_reset);
void reset_cec(bool hold_reset);
void reset_can2(bool hold_reset);
void reset_can1(bool hold_reset);
void reset_fmpi2c(bool hold_reset);
void reset_i2c3(bool hold_reset);
void reset_i2c2(bool hold_reset);
void reset_i2c1(bool hold_reset);
void reset_uart5(bool hold_reset);
void reset_uart4(bool hold_reset);
void reset_usart3(bool hold_reset);
void reset_usart2(bool hold_reset);
void reset_spdif_rx(bool hold_reset);
void reset_spi3(bool hold_reset);
void reset_spi2(bool hold_reset);
void reset_window_watchdog(bool hold_reset);
void reset_timer14(bool hold_reset);
void reset_timer13(bool hold_reset);
void reset_timer12(bool hold_reset);
void reset_timer7(bool hold_reset);
void reset_timer6(bool hold_reset);
void reset_timer5(bool hold_reset);
void reset_timer4(bool hold_reset);
void reset_timer3(bool hold_reset);
void reset_timer2(bool hold_reset);

void reset_sai2(bool hold_reset);
void reset_sai1(bool hold_reset);
void reset_timer11(bool hold_reset);
void reset_timer10(bool hold_reset);
void reset_timer9(bool hold_reset);
void reset_sys_config(bool hold_reset);
void reset_spi4(bool hold_reset);
void reset_spi1(bool hold_reset);
void reset_sdio(bool hold_reset);
void reset_adc(bool hold_reset);
void reset_usart6(bool hold_reset);
void reset_usart1(bool hold_reset);
void reset_timer8(bool hold_reset);
void reset_timer1(bool hold_reset);

/* Peripheral clock enable */
void enable_gpio_clock(gpio_bank_t bank);
void disable_gpio_clock(gpio_bank_t bank);
bool check_gpio_clock(gpio_bank_t bank);

void enable_peripheral_clock(uint32_t reg, uint32_t peripheral);
void disable_peripheral_clock(uint32_t reg, uint32_t peripheral);
bool check_peripheral_clock(uint32_t reg, uint32_t peripheral);

void enable_peripheral_low_power_clock(uint32_t reg, uint32_t peripheral);
void disable_peripheral_low_power_clock(uint32_t reg, uint32_t peripheral);
bool check_peripheral_low_power_clock(uint32_t reg, uint32_t peripheral);

/* Backup domain */
void reset_backup_domain(bool hold_reset);
void enable_rtc_clock(void);
void disable_rtc_clock(void);
void select_rtc_clock(rtc_clocks_t selection);
void set_lse_mode(bool mode);
void set_lse_bypass(bool bypass);
bool lse_ready(void);
void enable_lse(void);
void disable_lse(void);
uint32_t read_backup_domain(uint32_t mask);

/* Clock control and status */
bool check_clock_flag(uint32_t clk_flag);
void clear_clock_flags(void);
void enable_low_speed_oscillator(void);
void disable_low_speed_oscillator(void);
uint32_t read_clock_control_and_status_register(uint32_t mask);

/* spread spectrum clock generation */
void enable_spread_spectrum(void);
void disable_spread_spectrum(void);
bool is_spread_spectrum_enabled(void);
void update_step_increment(uint16_t step_size);
void update_modulation_period(uint16_t period);
uint32_t check_spread_spectrum_reg(uint32_t mask);

/* PLLI2S Configuration */
void configure_12s_pll(pll_config_t configs);
void read_i2s_pll_configs(pll_config_t *configs);

/* PLLSAI Configuration */
void configure_sai_pll(pll_config_t configs);
void read_sai_pll_configs(pll_config_t *configs);

/* Dedicated clock configuration */
void update_i2s2_source(uint32_t src);
void update_i2s1_source(uint32_t src);
void update_timer_prescaler_selection(uint32_t selection);
void update_sai2_source(uint32_t src);
void update_sai1_source(uint32_t src);
void update_sai_division_factor(uint32_t div);
void update_i2s_division_factor(uint32_t div);
void update_spdif_rx_clock_section(uint32_t selection);
void update_sdio_selection(uint32_t selection);
void update_clk_48mhz_selection(uint32_t selection);
void update_cec_selection(uint32_t selection);
void update_fmpi2c1_selection(uint32_t selection);

/* clocks gated enable */
void enable_rcc_clock(clocks_t clock);
void disable_rcc_clock(clocks_t clock);
bool is_rcc_clock_enabled(clocks_t clock);

#endif /* STM_RCC_H_ */
