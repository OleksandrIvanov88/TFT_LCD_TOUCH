#include "tick.h"
#include "intrinsics.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

/**
 * lib tick for tft conntroller and tft graphical libs- provides support for system ticks
 * Ticks may be used for uptime accounting, providing delays and other means.
 */

volatile uint32_t __cont_nticks = 0;


/**
 * Default SysTick ISR handler used by tick module.
 * This implementation simply calls :c:func:`__cont_tick_inc_callback` to increment tick
 * value.
 * User may override this handler to provide own implementation.
 */
void attr_weak sys_tick_handler(void)
{
	__cont_tick_inc_callback();
}


/**
 * Initialize tick internals for SysTick
 * @period: Tick period in clock cycles. Must be <= 0x7fffff8 in any case
 * @irq_priority: SysTick interrupt priority (as set by :c:func:`nvic_set_priority`)
 * @return: true on success, false on init failure
 *
 * Note:
 * Tries to use divider by 8 if applicable to extend possible period range.
 * Either any 24 bit, or 27 bit with lower 3 bits set to 0 (dividable by 8 with no remainder)
 * value must be used for period.
 */
bool cont_tick_init(uint32_t period, uint_fast8_t irq_priority)
{
	if (0 == period)
		return false;

	// We have 24 bit counter. So try to set systick to optimal value, using Fcpu/8
	// divider if applicable and returning error if desired period can not be reached at all
	bool div8 = false;

	if (!(period % 8)) {
		period /= 8;
		div8 = true;
	}

	// disable just in case we're trying to reconfigure
	// while the SysTick is running
	systick_counter_disable();

	if (period & 0xFF000000) {
		// still can not get the 24 bit fit because of remainder or something else
		return false;
	}

	systick_set_clocksource(div8 ? STK_CSR_CLKSOURCE_AHB_DIV8 : STK_CSR_CLKSOURCE_AHB);

	systick_interrupt_enable();

	systick_set_reload(period);

	// load value
	STK_CVR = period;

	nvic_set_priority(NVIC_SYSTICK_IRQ, irq_priority);
	nvic_enable_irq(NVIC_SYSTICK_IRQ);

	__cont_tick_set_current(0);

	systick_counter_enable();
	return true;
}


/**
 * Returns current tick period in Hz.
 * Note:
 * The resulting value may be influenced by an integer division truncation.
 */
uint32_t cont_get_tick_rate_hz(void)
{
	// rcc_ahb_frequency holds frequency in Hz of AHB domain (CPU is clocked from it)
	uint32_t rate = rcc_ahb_frequency / systick_get_reload();

	if (!(STK_CSR & STK_CSR_CLKSOURCE)) {
		// detect if divider by 8 is used and divide
		rate /= 8;
	}
	return rate;
}


/**
 * Use tick counter to provide millisecond delays.
 * Note:
 * Puts MCU to sleep using WFI instruction regularly until the desired tick value is reached.
 */
void cont_tick_delay_ms(uint32_t ms)
{
	uint32_t cur = cont_tick_get_current();
	uint32_t delta = (cont_get_tick_rate_hz() / 1000) * ms;
	uint32_t next = cur + delta;

	// take overflow into account
	if (next < cur) {	// 32 bit overflow happened
		while (cont_tick_get_current() > next)
			__WFI();
	} else {			// no overflow
		while (cont_tick_get_current() <= next)
			__WFI();
	}
}

