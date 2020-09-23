#pragma once

#include "pin.h"
#include "error.h"

/*GRAM(graphic memory) register for writing data.*/
#define GRAM_R  0x22

/*Display resolution*/
extern uint16_t tft_width;
extern uint16_t tft_hight;

/*Display orientation flags*/
enum display_orient {
	ALBUM = 0,
	PORTRATE
};

/*
 * display_setings
 * @width: display width in pixels
 * @hight: display width in pixels
 * @flag: indicate orientation ALBUM == 0, PORTRATE == 1
 * @current_rot: current rotation stay:0, 90, 180, 270
 */

struct display_setings {
	uint16_t width;
	uint16_t hight;
	uint8_t flag;
	uint16_t current_rot;
};
/*Display orientation*/
extern struct display_setings disp_orient;

/*Pointers to delay function*/
extern void (*ili9325_ptr_delay_ms)(uint32_t delay);

/*Function prototypes, for more info refer to ili9325.c*/
void ili9325_set_resol(uint16_t width, uint16_t hight);
void ili9325_init(void);
void ili9325_draw_pixel(uint16_t w, uint16_t h, uint16_t color);
void ili9325_fill_screen(uint16_t color);
tft_err ili9325_set_frame(uint16_t w1, uint16_t h1, uint16_t w2, uint16_t h2);
tft_err ili9325_rotate_screen(uint16_t rot_degrees);
void ili9325_frame_draw_pixel(uint16_t color);
void ili9325_screen_reset(void);



