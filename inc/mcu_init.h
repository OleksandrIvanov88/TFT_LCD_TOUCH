#pragma once

#include "pin.h"

enum spi_touch {
    SPI1_TOUCH = 0,
    SPI2_TOUCH,
    SPI3_TOUCH
};

/*Define of spi which will be using in touch_spi_tx_rx()*/
#define SPI_FUNC SPI2_TOUCH
/*Define of spi which will be using in pins_touch_init()*/
#define SPI_INIT SPI2 
/*Define of spi rcc which will be using in pins_touch_init()*/
#define RCC_SPI_INIT RCC_SPI2

/*Function prototypes for more info refer to mcu_init.c*/
void clock_init(void);
void pins_tft_init(const ili9325_pin_group* pins);
void pins_touch_init(const ili9325_pin_group* spi, const ili9325_pin* cs);
void touch_spi_tx_rx(enum spi_touch spi, uint8_t* buffer_rx, uint8_t* buffer_tx);

