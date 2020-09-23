#include "mcu_init.h"
#include "tick.h"
#include "tft.h"
#include "ili9325.h"
#include <stddef.h>
#if MCU_LIB
#else
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#endif

/**
 * Enable RCC for the GPIO from the pin group
 * @pins: pointer to the pin group
 * 
 * Note: If HAL defined in config.h will be using HAL's macros, 
 * default libopencm3 functions for RCC enabeling
 */
static void rcc_enable_group(const ili9325_pin_group *pins)
{
#if MCU_LIB
    /*HAL*/
    /*RCC clock activation of GPIO pins*/
    if (pins->port == ili9325_PORTA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    if (pins->port == ili9325_PORTB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    if (pins->port == ili9325_PORTC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    if (pins->port == ili9325_PORTD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    if (pins->port == ili9325_PORTE)
        __HAL_RCC_GPIOE_CLK_ENABLE();
    if (pins->port == ili9325_PORTF)
        __HAL_RCC_GPIOF_CLK_ENABLE();
    if (pins->port == ili9325_PORTG)
        __HAL_RCC_GPIOG_CLK_ENABLE();
    if (pins->port == ili9325_PORTH)
        __HAL_RCC_GPIOH_CLK_ENABLE();
    if (pins->port == ili9325_PORTI)
        __HAL_RCC_GPIOI_CLK_ENABLE();
	/*Should be uncommented if your MCU has ports J or K
	if(pins->port == ili9325_PORTJ) 
		__HAL_RCC_GPIOJ_CLK_ENABLE();
	if(pins->port == ili9325_PORTK) 
		__HAL_RCC_GPIOK_CLK_ENABLE();
	*/
#else
    uint32_t RCC_GPIO = 0;
    if (pins->port == ili9325_PORTA)
        RCC_GPIO = RCC_GPIOA;
    if (pins->port == ili9325_PORTB)
        RCC_GPIO = RCC_GPIOB;
    if (pins->port == ili9325_PORTC)
        RCC_GPIO = RCC_GPIOC;
    if (pins->port == ili9325_PORTD)
        RCC_GPIO = RCC_GPIOD;
    if (pins->port == ili9325_PORTE)
        RCC_GPIO = RCC_GPIOE;
    if (pins->port == ili9325_PORTF)
        RCC_GPIO = RCC_GPIOF;
    if (pins->port == ili9325_PORTG)
        RCC_GPIO = RCC_GPIOG;
    if (pins->port == ili9325_PORTH)
        RCC_GPIO = RCC_GPIOH;
    if (pins->port == ili9325_PORTI)
        RCC_GPIO = RCC_GPIOI;
    /*Should be uncommented if your MCU has ports J or K
    if(pins->port == ili9325_PORTJ) 
		RCC_GPIO = RCC_GPIOJ;
    if(pins->port == ili9325_PORTK) 
		RCC_GPIO = RCC_GPIOK;
    */
    rcc_periph_clock_enable(RCC_GPIO);
#endif
}

/**
 * Enable RCC for the GPIO from the pin init struct
 * @pin: pointer to the pin
 * 
 * Note: If HAL defined in config.h will be using HAL's macros, 
 * default libopencm3 functions for RCC enabeling
 */
static void rcc_enable_pin(const ili9325_pin *pin)
{
#if MCU_LIB
    /*HAL*/
    /*RCC clock activation of GPIO pins*/
    if (pin->port == ili9325_PORTA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    if (pin->port == ili9325_PORTB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    if (pin->port == ili9325_PORTC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    if (pin->port == ili9325_PORTD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    if (pin->port == ili9325_PORTE)
        __HAL_RCC_GPIOE_CLK_ENABLE();
    if (pin->port == ili9325_PORTF)
        __HAL_RCC_GPIOF_CLK_ENABLE();
    if (pin->port == ili9325_PORTG)
        __HAL_RCC_GPIOG_CLK_ENABLE();
    if (pin->port == ili9325_PORTH)
        __HAL_RCC_GPIOH_CLK_ENABLE();
    if (pin->port == ili9325_PORTI)
        __HAL_RCC_GPIOI_CLK_ENABLE();
	/*Should be uncommented if your MCU has ports J or K
	if(pin->port == ili9325_PORTJ) 
		__HAL_RCC_GPIOJ_CLK_ENABLE();
	if(pin->port == ili9325_PORTK) 
		__HAL_RCC_GPIOK_CLK_ENABLE();
	*/
#else
    uint32_t RCC_GPIO = 0;
    if (pin->port == ili9325_PORTA)
        RCC_GPIO = RCC_GPIOA;
    if (pin->port == ili9325_PORTB)
        RCC_GPIO = RCC_GPIOB;
    if (pin->port == ili9325_PORTC)
        RCC_GPIO = RCC_GPIOC;
    if (pin->port == ili9325_PORTD)
        RCC_GPIO = RCC_GPIOD;
    if (pin->port == ili9325_PORTE)
        RCC_GPIO = RCC_GPIOE;
    if (pin->port == ili9325_PORTF)
        RCC_GPIO = RCC_GPIOF;
    if (pin->port == ili9325_PORTG)
        RCC_GPIO = RCC_GPIOG;
    if (pin->port == ili9325_PORTH)
        RCC_GPIO = RCC_GPIOH;
    if (pin->port == ili9325_PORTI)
        RCC_GPIO = RCC_GPIOI;
    /*Should be uncommented if your MCU has ports J or K
    if(pin->port == ili9325_PORTJ) 
		RCC_GPIO = RCC_GPIOJ;
    if(pin->port == ili9325_PORTK) 
		RCC_GPIO = RCC_GPIOK;
    */
    rcc_periph_clock_enable(RCC_GPIO);
#endif
}


/**
 * MCU clock initialization to 168 MHZ using 
 * external crystal oscillator
 * Note: If HAL defined in config.h you need to use Cube 
 * IDE to set clock and this function is not required.
 * Use this function for libopencm3 clock enabeling
 */
void clock_init(void)
{
#if MCU_LIB

	/*(!!!)Use Cube IDE to set clock */

#else
	// Set HCE to external clock source (HSE bypass), not crystal. See page 218 RM
	rcc_osc_bypass_enable(RCC_HSE);		// bypass load capacitors used for crystals
	rcc_osc_on(RCC_HSE);				// enable High-Speed External clock (HSE)
	
	// trap until external clock is detected as stable
	while (!rcc_is_osc_ready(RCC_HSE));
	
	// as page 79 DS tells to operate at high frequency, we need to supply more power
	// by setting VOS=1 bit in power register
	rcc_periph_clock_enable(RCC_PWR);
	pwr_set_vos_scale(PWR_SCALE1);
	rcc_periph_clock_disable(RCC_PWR);	// no need to configure or use features requiring clock

	// Configure PLL
	rcc_osc_off(RCC_PLL);		        // Disable PLL before configuring it

	// Set PLL multiplication factor as specified at page 226 RM
	rcc_set_main_pll_hse(4, 168, 2, 7, 0);	// with input from HSE to PLL
	rcc_css_disable();		                // Disable clock security system
	rcc_osc_on(RCC_PLL);				    // Enable PLL
	while (!rcc_is_osc_ready(RCC_PLL));     // Wait for PLL out clock to stabilize

	// Set all prescalers.
	// (!) Important. Different domains have different maximum allowed clock frequencies
	// So we need to set corresponding prescalers before switching AHB to PLL
	rcc_set_hpre(RCC_CFGR_HPRE_DIV_NONE);
    rcc_set_ppre1(RCC_CFGR_PPRE_DIV_4);
    rcc_set_ppre2(RCC_CFGR_PPRE_DIV_2);

	// Enable caches. Flash is slow (around 30 MHz) and CPU is fast (168 MHz)
	flash_dcache_enable();
	flash_icache_enable();

	// IMPORTANT! We must increase flash wait states (latency)
	// otherwise fetches from flash will ultimately fail
	flash_set_ws(FLASH_ACR_LATENCY_7WS);

	// Select PLL as AHB bus (and CPU clock) source
    rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
	// Wait for clock domain to be changed to PLL input
	rcc_wait_for_sysclk_status(RCC_PLL);

	// set by hand since we've not used rcc_clock_setup_pll
	rcc_ahb_frequency = 168000000ul;
	rcc_apb1_frequency = rcc_ahb_frequency / 4;
	rcc_apb2_frequency = rcc_ahb_frequency / 2;

	// Disable internal 16 MHz RC oscillator (HSI)
	rcc_osc_off(RCC_HSI);
#endif	
}


/**
 * Configuration and initialization for GPIO pins
 * @pins: group of pins to initialize
 */
void pins_tft_init(const ili9325_pin_group* pins)
{
	if(pins == NULL) return;
	rcc_enable_group(pins);
	#if MCU_LIB
		/*HAL*/
		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		GPIO_InitStruct.Pin = pins->pins;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init((GPIO_TypeDef*) ili9325_pin_port_to_gpio(pins->port), &GPIO_InitStruct);

	#else 
		/*libopencm3*/
		gpio_set_output_options(ili9325_pin_port_to_gpio(pins->port),
								GPIO_OTYPE_PP,
								GPIO_OSPEED_50MHZ,
								pins->pins);
		gpio_mode_setup(ili9325_pin_port_to_gpio(pins->port),
						GPIO_MODE_OUTPUT,
						GPIO_PUPD_PULLUP,
						pins->pins);
			
	#endif
    pins_group_set(*pins, PIN_SET);                
}

/**
 * Initializing SPI MOSI, MISO, SCK, CS for xpt2046 touch chip
 * @cs: pointer to the CS pin init struct
 * @spi: pointer to the MOSI, MISO, SCK pin group init struct
 * 
 * Note: If HAL defined in config.h you need to use Cube IDE f
 * or SPI MOSI, MISO, SCK initialization.Default libopencm3, you need 
 * define SPI_INIT and RCC_SPI_INIT in mcu_init.h
 */
void pins_touch_init(const ili9325_pin_group* spi, const ili9325_pin* cs)
{
	if(spi == NULL && cs == NULL) return;
	rcc_enable_group(spi);
	rcc_enable_pin(cs);
#if MCU_LIB
	/*HAL*/
 /*(!!!)Use Cube IDE for SPI MOSI, MISO, SCKL initialization*/

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = (1 << cs->pin);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init((GPIO_TypeDef*) ili9325_pin_port_to_gpio(cs->port), &GPIO_InitStruct);
	pin_set(*cs, PIN_SET);

#else
	/*libopencm3*/
	/*Setup GPIO*/
	// Pins directly assigned to SPI peripheral SRC, MOSI and MISO
	gpio_set_output_options(ili9325_pin_port_to_gpio(spi->port), 
	                        GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, 
							spi->pins);
	gpio_set_af(ili9325_pin_port_to_gpio(spi->port), 
	            GPIO_AF5, spi->pins);
	gpio_mode_setup(ili9325_pin_port_to_gpio(spi->port), 
	                GPIO_MODE_AF, GPIO_PUPD_NONE, 
					spi->pins);
	// CS Pin drived manually
	gpio_set_output_options(ili9325_pin_port_to_gpio(cs->port), 
							GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, 
							1 << cs->pin);
	gpio_mode_setup(ili9325_pin_port_to_gpio(cs->port), 
					GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, 
					1 << cs->pin);
	gpio_set(ili9325_pin_port_to_gpio(cs->port), 1 << cs->pin);

	rcc_periph_clock_enable(RCC_SPI_INIT);
	// SPI2 belongs to APB1 (42 MHz frequency)
	// We have /2, /4, /8, /16, /32, /64, /128 and /256 prescalers
	// Our SPI XPT2046 controller can work up to 2 MHz clock
	// set spi baudrate to /32, which gives us 42/32 = 1.3125 MHz SCLK frequency
	spi_set_baudrate_prescaler(SPI_INIT, SPI_CR1_BR_FPCLK_DIV_32);
	// Our MCU is master and xpt2046 chip is slave
	spi_set_master_mode(SPI_INIT);
	// We work in full duplex (simultaneous transmit and receive)
	spi_set_full_duplex_mode(SPI_INIT);
	// Data frame format is 8 bit, not 16
	spi_set_dff_8bit(SPI_INIT);
	// No CRC calculation is required
	spi_disable_crc(SPI_INIT);
	// Our chip requires Most Significant Bit first
	spi_send_msb_first(SPI_INIT);
	// Select SPI mode 0
	spi_set_clock_polarity_0(SPI_INIT);
	spi_set_clock_phase_0(SPI_INIT);
	// Set hardware control of NSS pin. Because disabling it can cause master to become slave
	// depending on NSS input state.
	// Normally NSS will be hard-wired to slave. But in our case we have other pin connected to
	// slave CS and should drive it manually
	spi_enable_ss_output(SPI_INIT);
	// So simply enable spi peripheral
	spi_enable(SPI_INIT);
#endif

}

void touch_spi_tx_rx(enum spi_touch spi, uint8_t* buffer_rx, uint8_t* buffer_tx)
{
	if(buffer_rx == NULL || buffer_tx == NULL) return;
#if MCU_LIB
    /*HAL*/
	SPI_HandleTypeDef hspi
	if(spi == SPI1_TOUCH) hspi = hspi1;
	if(spi == SPI2_TOUCH) hspi = hspi2;
	if(spi == SPI3_TOUCH) hspi = hspi3;
	HAL_SPI_TransmitReceive(&hspi, buffer_tx, buffer_rx, 3, 1000);
#else
	/*libopencn3*/
	uint32_t SPI_LIB;
	if(spi == SPI1_TOUCH) SPI_LIB = SPI1;
	if(spi == SPI2_TOUCH) SPI_LIB = SPI2;
	if(spi == SPI3_TOUCH) SPI_LIB = SPI3;

	for (int32_t i = 0; i < 3; i++) {
			spi_send(SPI_LIB, buffer_tx[i]);
			buffer_rx[i] = spi_read(SPI_LIB);
	}
#endif
}

/*Pointers definition to the ms delay function*/
#if MCU_LIB
	void (*ili9325_ptr_delay_ms)(uint32_t delay) = HAL_Delay;//HAL
	void (*touch_delay_ms)(uint32_t delay) = HAL_Delay;//HAL

#else
	void (*ili9325_ptr_delay_ms)(uint32_t delay) = cont_tick_delay_ms;//for libopen
	void (*touch_delay_ms)(uint32_t delay) = cont_tick_delay_ms;//for libopen
#endif

/*Pointers definition for touch screen's displaying functions*/
void (*touch_printf)(const char *fmt, ...) = tft_printf;
void (*touch_setcursor)(uint16_t x, uint16_t y) = tft_set_cursor;
void (*touch_fill_screen)(uint16_t color) = ili9325_fill_screen;
void (*touch_fill_circle)(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) = tft_fill_circle;

