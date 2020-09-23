
#include "pin.h"
/**
 * Densification helper (private)
 * Example:
 *     mask = 0b1010000011110010
 *   sparse = 0b1010101010101010
 *   result = 0b1 1     1010  1
 */
static uint16_t group_densify(uint16_t mask, uint16_t sparse)
{
	uint16_t ret = 0;
	int idx = 0;
	for (int i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			ret |= sparse & (1 << i) ? (1 << idx) : 0;
			idx++;
		}
	}
	return ret;
}

/**
 * Sparsification helper (private)
 * Example:
 *     mask = 0b1010000011110010
 *    dense = 0b1 1     1010  1
 *   result = 0b1010000010100010
 */
static uint16_t group_sparsify(uint16_t mask, uint16_t dense)
{
	uint16_t ret = 0;
	for (int i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			ret |= dense ? (1 << i) : 0;
		}
	}
	return ret;
}

/**
 * Read pin input and return its value
 * @pin: pin to read
 * @return: boolean value of pin input
 *
 * Note:
 * For the means of improved speed, directly reads port input register and performs no checks
 * that the pin is really set to input.
 * The user is responsible for checking pin configuration validity.
 *
 * Inversion is taken into account as specified in :c:type:`ili9325_pin`
 */
bool pin_read(ili9325_pin pin)
{
	#if MCU_LIB
		bool ret = HAL_GPIO_ReadPin((GPIO_TypeDef *)ili9325_pin_port_to_gpio(pin.port), pin.pin);
	#else
		bool ret = gpio_port_read(ili9325_pin_port_to_gpio(pin.port)) & (1 << pin.pin);//libopencm3
	#endif
	
	return ret ^ pin.isinverse;
}

/**
 * Set pin output level to specified value
 *
 * Note:
 * Does not check if pin is set to output, to improve speed.
 * The user is responsible for providing a correct pin as an argument.
 *
 * Inversion is taken into account as specified in :c:type:`ili9325_pin`
 */
void pin_set(ili9325_pin pin, bool value)
{
	if (value ^ pin.isinverse) {
		#if MCU_LIB 
			HAL_GPIO_WritePin((GPIO_TypeDef *)ili9325_pin_port_to_gpio(pin.port), (1 << pin.pin), PIN_SET);//for HAL
		#else
			gpio_set(ili9325_pin_port_to_gpio(pin.port), (1 << pin.pin)); // for libopen
		#endif
	}
	else {
		#if MCU_LIB
			HAL_GPIO_WritePin((GPIO_TypeDef *)ili9325_pin_port_to_gpio(pin.port), (1 << pin.pin), PIN_RESET);//for HAL
		#else 
			gpio_clear(ili9325_pin_port_to_gpio(pin.port), (1 << pin.pin)); //for libopen
		#endif
	}	
}

/**
 * Toggle pin output level to the opposite value
 *
 * Note:
 * Does not check if pin is set to output, for speed improvement.
 * The user is responsible for providing a correct pin.
 *
 * Inversion is taken into account as specified in :c:type:`ili9325_pin`
 */
void pin_toggle(ili9325_pin pin)
{
	#if MCU_LIB
		HAL_GPIO_TogglePin((GPIO_TypeDef *)ili9325_pin_port_to_gpio(pin.port), pin.pin);//for HAL
	#else
		gpio_toggle(ili9325_pin_port_to_gpio(pin.port), (1 << pin.pin)); //for libopen
	#endif
}

/**
 * Read group of pins and return collected value.
 * @group: pin group (:c:type:`ili9325_pin_group`)
 * @return: densified value read from specified :c:member:`ili9325_pin_group.pins`
 *
 * Only the pins specified in :c:type:`ili9325_pin_group` are collected in the resulting value applying
 * densification.
 * If :c:member:`ili9325_pin_group.pins` are specified, for example as 0b0010100100001100,
 * and GPIO has value 0bABCDEFGHIJKLMNOP, the resulting (collected) value will be
 * 0bCEHMN.
 */
uint16_t pins_group_read(ili9325_pin_group group)
{
	#if MCU_LIB
		uint16_t val = ((GPIO_TypeDef *)ili9325_pin_port_to_gpio(group.port))->IDR;//for HAL
	#else 
		uint16_t val = gpio_port_read(ili9325_pin_port_to_gpio(group.port));//for libopen
	#endif
	val ^= group.inversions;
	return group_densify(group.pins, val);
}

/**
 * Set group of pins to the provided (densified) value
 * @group: pin group (:c:type:`ili9325_pin_group`)
 * @values: densified value specifying which pins to set
 *
 * Only the pins specified in :c:type:`ili9325_pin_group` are affected by set operation.
 * If :c:member:`ili9325_pin_group.pins` are specified, for example as 0b0101000000000011,
 * and `values` is set to 0b000011110000ABCD, the resulting GPIO value will be
 * 0b0A0B0000000000CD.
 *
 * Note:
 * Access to the corresponding GPIO registers is not atomic.
 */
void pins_group_set(ili9325_pin_group group, uint16_t values)
{
	// We want to change only pins we use in group, and don't touch the others
	// We also want to manipulate the output register, not what's on port input at the moment,
    // so gpio_port_read() does not suit. Use GPIO ODR register directly
	values = group_sparsify(group.pins, values);
	values ^= group.inversions;
	#if MCU_LIB
		volatile uint32_t *odr = &(((GPIO_TypeDef *)ili9325_pin_port_to_gpio(group.port))->ODR);//for HAL
	#else
		volatile uint32_t *odr = &GPIO_ODR(ili9325_pin_port_to_gpio(group.port)); //for libopen
	#endif
	
	uint32_t pval = *odr;
	pval &= ~((uint32_t)(group.pins));	// reset all pins in account
	pval |= values;			// set all pins in account to our values
	*odr = pval;
}

/**
 * Toggle group of pins to the opposite values
 * @group: pin group (:c:type:`ili9325_pin_group`)
 * @values: densified value specifying which pins to toggle
 *
 * Only the pins specified in :c:type:`ili9325_pin_group` are affected by set operation.
 * If :c:member:`ili9325_pin_group.pins` are specified, for example as 0b0101000000000011,
 * and `values` is set to 0b000011110000ABCD, the resulting GPIO value will be
 * 0b0A0B0000000000CD.
 *
 * Note:
 * Access to the corresponding GPIO registers is not atomic.
 */
void pins_group_toggle(ili9325_pin_group group, uint16_t values)
{
	values = group_sparsify(group.pins, values);
	#if MCU_LIB
		volatile uint32_t *odr = &(((GPIO_TypeDef *)ili9325_pin_port_to_gpio(group.port))->ODR);//for HAL
	#else
		volatile uint32_t *odr = &GPIO_ODR(ili9325_pin_port_to_gpio(group.port)); //for libopen
	#endif
	*odr ^= values;
}

/**
 * Set pin fom the group output level to specified value
 *
 * Note:
 * Does not check if pin is set to output, to improve speed.
 * The user is responsible for providing a correct pin as an argument.
 *
 * Inversion is taken into account as specified in :c:type:`ili9325_pin`
 */
void pin_set_f_group(ili9325_pin_group group, uint16_t pin,  uint16_t value)
{
	if (value ^ group.inversions) {
		#if MCU_LIB
			HAL_GPIO_WritePin((GPIO_TypeDef*) ili9325_pin_port_to_gpio(group.port), (1 << pin), PIN_SET);//for HAL
		#else 
			gpio_set(ili9325_pin_port_to_gpio(group.port), (1 << pin)); // for libopen
		#endif
	}
	else {
		#if MCU_LIB
			HAL_GPIO_WritePin((GPIO_TypeDef*) ili9325_pin_port_to_gpio(group.port), (1 << pin), PIN_RESET);//for HAL
		#else 
			gpio_clear(ili9325_pin_port_to_gpio(group.port), (1 << pin)); //for libopen
		#endif
	}
}


/**
 * Write a value to the given GPIO port.
 * @pins: grop of pins
 * @data: data to write
 */
void write_port_data(ili9325_pin_group pins, uint16_t data)
{
	#if MCU_LIB
		((GPIO_TypeDef *)ili9325_pin_port_to_gpio(pins.port))->ODR = data;//for HAL
	#else
		gpio_port_write(ili9325_pin_port_to_gpio(pins.port), data);//for libopen
	#endif
}	

/*Screen control pins*/
const ili9325_pin_group ili9325_ctrl_pins = {
		.port = PORT_CTRL,
		.pins = (1 << RS_PIN) | (1 << WR_PIN) | (1 << CS_PIN) | (1 << RESET_PIN) | (1 << RD_PIN),
		.inversions = 0
};

/*Screen data bus pins*/
const ili9325_pin_group ili9325_data_pins = {
		.port = PORT_DATA,
		.pins = 0xFFFF, // all pins of this port
		.inversions = 0
};

/*Touch SPI pins*/
const ili9325_pin_group touch_spi_pins = {
		.port = PORT_TOUCH_SPI,
		.pins = (1 << T_SCK) | (1 << T_MISO) | (1 << T_MOSI),
		.inversions = 0
};

/*Touch CS pin*/
const ili9325_pin touch_cs_pin = {
		.port = PORT_TOUCH_CS,
		.pin = T_CS,
		.isinverse = 0
};
