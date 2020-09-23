#pragma once



#include "macro.h"
#include <stdint.h>
#include <stdbool.h>


/**
 * System tick counter. Variable, which holds current tick value.
 * Initialized to 0 at startup and at initialization with :c:func:`cont_tick_init`.
 * Should not be used directly. Use setters/getters provided here instead.
 */
extern volatile uint32_t __cont_nticks;


/** Returns current system tick counter (:c:data:`__cont_nticks`) value */
inline attr_alwaysinline uint32_t cont_tick_get_current(void)
{
	return __cont_nticks;
}

/**
 * Set current system tick counter (:c:data:`__cont_nticks`) value.
 * Note:
 * Normally this should never be called by user.
 * But is provided for some rare cases.
 */
inline attr_alwaysinline void __cont_tick_set_current(uint32_t val)
{
	__cont_nticks = val;
}

/**
 * System tick increment callback.
 * This is called in ISR (usually Cortex SysTick handler isr) at regular time periods to
 * increment the current tick value
 */
inline attr_alwaysinline void __cont_tick_inc_callback(void)
{
	__cont_nticks++;
}

/*Function prototypes, for more info refer to tick.h*/
void attr_weak sys_tick_handler(void);
bool cont_tick_init(uint32_t period, uint_fast8_t irq_priority);
uint32_t cont_get_tick_rate_hz(void);
void cont_tick_delay_ms(uint32_t ms);
