#pragma once
#include "fonts.h"
#include "error.h"
#include <stdint.h>

/*
 * Colors set
 * Format 16 bit 565 RGB colors
 * RRRRR GGGGGG BBBBB
 */
#define WHITE         	0xFFFF
#define BLACK         	0x0000
#define BLUE         	0x001F
#define RED           	0xF800
#define MAGENTA       	0xF81F
#define GREEN         	0x07E0
#define CYAN          	0x7FFF
#define YELLOW        	0xFFE0
#define BROWN			0xBC40
#define BRRED			0xFC07	 // brownish red
#define GRAY 			0x8430
#define DARKBLUE 		0x01CF
#define LIGHTBLUE 		0x7D7C
#define GRAYBLUE 		0x5458


/*Cursor position for tft_printf() function*/
extern uint16_t cursor_x;
extern uint16_t cursor_y;

/*Pointers to LCD TFT controller's functions*/
extern tft_err (*tft_set_frame)(uint16_t w1, uint16_t h1, uint16_t w2, uint16_t h2);
extern void (*tft_fill_screen)(uint16_t color);
extern void (*tft_frame_draw_pixel)(uint16_t color);
extern tft_err (*tft_rotate_screen)(uint16_t rot_degrees);
extern void (*tft_reset)(void);

/*Function prototypes, for more info refer to tft.c*/
void tft_init(uint16_t width, uint16_t hight, uint16_t color);
void tft_draw_line (uint16_t x1, uint16_t y1, uint16_t x2,uint16_t y2, uint8_t width, uint16_t color);
void tft_draw_rectangle(uint16_t x, uint16_t y,	uint16_t longX, uint16_t longY, uint8_t width, uint16_t color);
tft_err tft_fill_rectangle(uint16_t x, uint16_t y, uint16_t longX, uint16_t longY, uint16_t color);
void tft_draw_circle (uint16_t x0, uint16_t y0, uint16_t radius, uint8_t  width, uint16_t color);
void tft_fill_circle (uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);
tft_err tft_draw_fast_h_lin(int16_t x, int16_t y, int16_t length, uint16_t color);
tft_err tft_draw_fast_v_lin(int16_t x, int16_t y, int16_t length, uint16_t color);
void tft_draw_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		               uint16_t x2, uint16_t y2, uint8_t width,  uint16_t color);
tft_err tft_fill_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		               uint16_t x2, uint16_t y2, uint16_t color);
tft_err tft_print_num(	uint16_t x, uint16_t y, uint32_t num, uint8_t len);
tft_err tft_print_num0(uint16_t x, uint16_t y, uint32_t num, uint8_t len);
tft_err tft_pic_from_flash(uint16_t x, uint16_t y, const uint16_t* img);
tft_err tft_print_char(uint16_t x, uint16_t y, uint16_t ascii);
void tft_set_font(uint8_t type, uint16_t color,uint16_t back_color);
tft_err tft_print_str(uint16_t x, uint16_t y, const char *str);
void tft_colors_test(void);
tft_err tft_draw_point(uint16_t w, uint16_t h, uint8_t size, uint16_t color);
void tft_printf(const char *string, ...);
void tft_set_cursor(uint16_t x, uint16_t y);


