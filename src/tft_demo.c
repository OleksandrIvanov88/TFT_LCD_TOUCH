#include "tft.h"
#include "img_buffer.h"
#include "tick.h"
#include "mcu_init.h"
#include "xpt2046.h"

#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/pwr.h>

int main(void)
{
	/*MCU clock initialisation to max 168 MHZ*/
	clock_init();
	/*Initialize tick internals for SysTick*/
	cont_tick_init(16000000ul / 10000ul, 2);
	/*Enable interrupts globally*/
	cm_enable_interrupts();
	/*Configuration and initialization data pins*/
	pins_tft_init(&ili9325_data_pins);
	/*Configuration and initialization control pins*/
	pins_tft_init(&ili9325_ctrl_pins);
	pins_touch_init(&touch_spi_pins, &touch_cs_pin);
	/*LCD TFT screen initialization*/
	tft_init(320, 240, BLACK);
	touch_init();
	/*Filling the screen with BLACK color*/
	tft_fill_screen(BLACK);
	/*Setting the cursor to x == 0 and y == 0 coordinate*/
	tft_set_cursor(0, 0);
	/*Setting screen rotation*/
	tft_rotate_screen(0);
	/*Setting the font*/
	tft_set_font(COURIER_NEW_12_BOLD, WHITE, BLACK);

	/*Printf demo*/
	char *check = ", testing the printf() function";
	tft_printf("Hello GlobalLogic%s\r\n\r\n", check);
	uint32_t int_num = 123456;
	float f_num = 2.89;
	uint32_t hex_num = 0xFAC8;
	tft_printf("Testing printing int,float and hex format:\r\n"
			"%d %0.2f 0x%X\r\n\r\n", int_num, f_num, hex_num);
	tft_printf("Проверка печати кириллицей.");
	cont_tick_delay_ms(5000);

	/*Fonts demo*/
	tft_fill_screen(BLACK);
	tft_set_cursor(0, 0);
	tft_set_font(COURIER_NEW_20_NORM, GREEN, BLACK);
	tft_printf("Embedded system BaseCamp!\r\n");
	tft_set_font(UBUNTUMONO_14_NORM, WHITE, BLACK);
	tft_printf("Embedded system BaseCamp!\r\n");
	tft_set_font(COURIER_NEW_12_BOLD, BLUE, BLACK);
	tft_printf("Embedded system BaseCamp!\r\n");
	tft_set_font(COURIER_NEW_8_BOLD, YELLOW, BLACK);
	tft_printf("Embedded system BaseCamp!\r\n");
	tft_set_font(COURIER_NEW_8_NORM, RED, GREEN);
	tft_printf("Embedded system BaseCamp!\r\n");
	tft_set_font(SEVEN_SEGMENT, WHITE, BLACK);
	tft_printf("0123456789\r\n");
	cont_tick_delay_ms(5000);

	/*Graphical primitives demo*/
	tft_fill_screen(BLACK);
	tft_draw_line(2, 2, 80, 30, 3, GREEN);
	cont_tick_delay_ms(1000);
	tft_draw_fast_h_lin(85, 2, 200, RED);
	cont_tick_delay_ms(1000);
	tft_draw_fast_v_lin(290, 2, 100, BLUE);
	cont_tick_delay_ms(1000);
	tft_draw_rectangle(20, 40, 150, 80, 2, RED);
	cont_tick_delay_ms(1000);
	tft_fill_rectangle(20, 150, 150, 80, WHITE);
	cont_tick_delay_ms(1000);
	tft_draw_circle(220, 40, 30, 2, YELLOW);
	cont_tick_delay_ms(1000);
	tft_fill_circle(220, 120, 30, RED);
	cont_tick_delay_ms(1000);
	tft_draw_triangle(250, 150, 280, 200, 200, 200, 1, MAGENTA);
	cont_tick_delay_ms(1000);
	tft_fill_triangle(250, 150, 280, 200, 200, 200, BLUE);
	cont_tick_delay_ms(3000);

	/*Drawing picture from flash demo*/
	tft_fill_screen(BLACK);
	tft_pic_from_flash(5, 5, linux_pict);
	cont_tick_delay_ms(1000);
	tft_rotate_screen(90);
	tft_fill_screen(BLACK);
	tft_pic_from_flash(5, 5, linux_pict);
	cont_tick_delay_ms(1000);
	tft_rotate_screen(180);
	tft_fill_screen(BLACK);
	tft_pic_from_flash(5, 5, linux_pict);
	cont_tick_delay_ms(1000);
	tft_rotate_screen(270);
	tft_fill_screen(BLACK);
	tft_pic_from_flash(5, 5, linux_pict);
	cont_tick_delay_ms(1000);
	tft_rotate_screen(360);
	tft_fill_screen(BLACK);
	tft_pic_from_flash(0, 30, gl_base_camp);
	cont_tick_delay_ms(1000);
	tft_colors_test();
	cont_tick_delay_ms(1000);
	tft_fill_screen(BLACK);

	/*Setting font for the calibration function text*/
	tft_set_font(COURIER_NEW_8_NORM, WHITE, BLACK);
	/*Touch screen calibaration*/
	touch_calibr(&kX, &kY, &offsetX, &offsetY);
	/*Creating the menu buttons: CLEAN, BALL, DRAW, RUB*/
	tft_set_font(COURIER_NEW_8_NORM, WHITE, BLACK);
	tft_draw_rectangle(266, 9, 43, 43, 4, BLUE);
	tft_set_cursor(272, 25);
	tft_printf("CLEAN");
	tft_draw_rectangle(266, 72, 43, 43, 4, BLUE);
	tft_set_cursor(275, 87);
	tft_printf("BALL");
	tft_draw_rectangle(266, 135, 43, 43, 4, BLUE);
	tft_set_cursor(275, 149);
	tft_printf("DRAW");
	tft_draw_rectangle(266, 190, 43, 43, 4, BLUE);
	tft_set_cursor(275, 205);
	tft_printf("RUB");
	enum MODES {
		BALL = 0, DRAW, RUB
	};
	uint8_t mode = 0;
	uint16_t x_last_cord = 1000;
	uint16_t y_last_cord = 1000;
	uint16_t x_last_cord_r = 1000;
	uint16_t y_last_cord_r = 1000;
	const uint8_t ball_radius = 10;
	const uint8_t line_with = 4;
	const uint8_t ball_with = 2;


	while (1) {
		uint16_t x, y;
		if (touch_get_xy(&x, &y) == true) {
			tft_set_cursor(0, 0);
			tft_printf("X = %3d\r\nY = %3d\r\n", x, y);

			/*Cleaning the screen if CLEAN touched*/
			if (x > 266 && x < 309 && y > 9 && y < 52) {
				tft_fill_rectangle(0, 0, 265, 240, BLACK);
			}

			/*Set drawing mode to BALL, DRAW or RUB depend on touched button*/
			if (x > 266 && x < 309 && y > 72 && y < 115) {
				mode = BALL;
			} else if (x > 266 && x < 309 && y > 135 && y < 178) {
				mode = DRAW;
			} else if (x > 266 && x < 309 && y > 192 && y < 231) {
				mode = RUB;
			}

			/*Actions depend on selected mode*/
			if (mode == BALL) {
				if (x < 266 - ball_radius) {
					if (x_last_cord != x || y_last_cord != y) {
						tft_draw_circle(x_last_cord, y_last_cord, ball_radius,
								ball_with, BLACK);
						tft_draw_circle(x, y, ball_radius, ball_with, RED);
						x_last_cord = x;
						y_last_cord = y;
					}
				}
			} else if (mode == DRAW) {
				if (x < 266 - line_with) {
					tft_draw_point(x, y, line_with, RED);
				}
			} else if (mode == RUB) {
				if (x < 266 - 13) {
					if (x_last_cord_r != x || y_last_cord_r != y) {
						tft_fill_rectangle(x_last_cord_r, y_last_cord_r, 14, 14,
								BLACK);
						tft_draw_rectangle(x, y, 13, 13, 1, WHITE);
						x_last_cord_r = x;
						y_last_cord_r = y;
					}
				}
			}
		}
	}
}
