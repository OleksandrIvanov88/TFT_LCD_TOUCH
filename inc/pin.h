#pragma once

#include "config.h"
#include "macro.h"
#if MCU_LIB
	#include "gpio.h" // for HAL
#else 
	#include <libopencm3/stm32/gpio.h>
#endif
#include <stdint.h>
#include <stdbool.h>

/**
 * ili9325 lib pin abstraction layer.
 */

/**
 * GPIO ports definition (port A .. K).
 * Ports J and K are unavailable when :c:macro:`ILI9325_USE_SIZE_OPTIMIZATIONS` is set
 * to fit into :c:type:`ili9325_pin` 8-bit field
 */
enum ili9325_port {
	ili9325_PORTA = 0,
	ili9325_PORTB = 1,
	ili9325_PORTC = 2,
	ili9325_PORTD = 3,
	ili9325_PORTE = 4,
	ili9325_PORTF = 5,
	ili9325_PORTG = 6,
	ili9325_PORTH = 7,
#if !(ILI9325_USE_SIZE_OPTIMIZATIONS)
	/* Warning: Port I is used on some STM32F40xx packages */
	ili9325_PORTI = 8,
	ili9325_PORTJ = 9,
	ili9325_PORTK = 10
#endif
};
/*Pin state*/
enum pin_state {
  PIN_RESET = 0,
  PIN_SET
};

/*Control pins of parallel bus*/
enum ili9325_ctr_pins {
	RS_PIN = 6,
	WR_PIN,
	CS_PIN,
	RESET_PIN,
	RD_PIN
};

/*Pins fot touch's SPI*/
enum touch_pins {
	T_CS = 12,
	T_SCK,
	T_MISO,
	T_MOSI
};

/*Bus data and control ports, touch port*/
enum ili9325_data_ctrl_ports {
	PORT_CTRL = ili9325_PORTC,
	PORT_DATA = ili9325_PORTE,
	PORT_TOUCH_SPI = ili9325_PORTB,
	PORT_TOUCH_CS = ili9325_PORTB
};


/**
 * Represents separate GPIO pin
 */
struct attr_pack(1) ili9325_pin {
#if ILI9325_USE_SIZE_OPTIMIZATIONS
	/* Port value as :c:type:`ili9325_port` */
	uint8_t port      : 3;	// use shorter variant for packing into 1 byte
	/* Pin number (0 .. 15) */
	uint8_t pin       : 4;  // we have at maximum 16 pins in port -> 4 bits
	/* Set to true if port input or output should be inverted */
	uint8_t isinverse : 1;	// support value inversionm, it is useful dealing with connections
#else
	/* Port value as :c:type:`ili9325_port` */
	uint16_t port       : 4;
	/* Pin number (0 .. 15) */
	uint16_t pin        : 4;
	/* Set to true if port input or output should be inverted */
	uint16_t isinverse  : 1;
	/* Packing residue reserved for furter use */
	uint16_t __reserved : 7;  // 7 bits left. Reserve them for further use
#endif
};


/**
 * Represents group of pins residing in the same GPIO port.
 * Used as an optimization to allow reading/writing them all at once.
 */
struct attr_pack(1)ili9325_pin_group {
#if ILI9325_USE_SIZE_OPTIMIZATIONS
	/* Port value as :c:type:`ili9325_port` */
	uint16_t port       : 3;
	/* Packing residue reserved for furter use */
	uint16_t __reserved : 13;
#else
	/** Port value as :c:type:`ili9325_port` */
	uint16_t port       : 4;
	/** Packing residue reserved for furter use */
	uint16_t __reserved : 12;
#endif
	/*16-bit value where each bit represents corresponding pin in a port */
	uint16_t pins;
	/*16-bit mask where each bit==1 represents inversion in :c:member:`ili9325_pin_group.pins` */
	uint16_t inversions;
};


typedef struct ili9325_pin ili9325_pin;
typedef struct ili9325_pin_group ili9325_pin_group;

/** Map ili9325_port definitions to libopencm3 GPIOx. Intended mainly for private use */
inline attr_alwaysinline uint32_t ili9325_pin_port_to_gpio(enum ili9325_port port)
{
	// faster and more concise than building 1:1 map
	// benefit from memory locations of GPIO registers
	#if MCU_LIB
		return GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE) * port; //for HAL
	#else
		return GPIO_PORT_A_BASE + (GPIO_PORT_B_BASE - GPIO_PORT_A_BASE) * port; //for libopencm3
	#endif
}

/*Function prototypes, for more info refer to pin.c*/
// we pass by value (not by pointer) because this way compiler can optimize out
// variables from memory
bool pin_read(ili9325_pin pin);
void pin_set(ili9325_pin pin, bool value);
void pin_toggle(ili9325_pin pin);
uint16_t pins_group_read(ili9325_pin_group group);
void pins_group_set(ili9325_pin_group group, uint16_t values);
void pins_group_toggle(ili9325_pin_group group, uint16_t values);
void pin_set_f_group(ili9325_pin_group group, uint16_t pin,  uint16_t values);
void write_port_data(ili9325_pin_group pins, uint16_t data);

/*Config struct for bus's control port and pins*/
extern const ili9325_pin_group ili9325_ctrl_pins;
/*Config struct for bus's data port and pins*/
extern const ili9325_pin_group ili9325_data_pins;
/*Config struct for CS touch pin*/
extern const ili9325_pin touch_cs_pin;
/*Config struct for SPI touch pin*/
extern const ili9325_pin_group touch_spi_pins;





