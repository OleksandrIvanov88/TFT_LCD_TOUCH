#pragma once

#include<stdint.h>
#include<stdbool.h>

/*Seting the order of touchscreen coordinate*/
#define			SWAP_XY             // Swap the X and Y coordinates on the touch screen
#define			MIRROR_X    	    // Flip touchscreen coordinates on X axis
//#define			MIRROR_Y		// Flip touch screen Y coordinates

//LCD screen pixel size
#define X_PIX_MAX tft_width
#define Y_PIX_MAX tft_hight

/*Offset in pixels of the coordinate for the calibration points*/
#define OFFSET_POINT 20
#define POINT1_X OFFSET_POINT                  // Left top point (Point 1)
#define POINT1_Y OFFSET_POINT
#define POINT2_X (X_PIX_MAX - OFFSET_POINT)    // Right top point (Point 2)
#define POINT2_Y OFFSET_POINT
#define POINT3_X (X_PIX_MAX - OFFSET_POINT)    // Right down point (Point 3)
#define POINT3_Y (Y_PIX_MAX - OFFSET_POINT)
#define POINT4_X OFFSET_POINT                  // Left down point (Point 4)
#define POINT4_Y (Y_PIX_MAX - OFFSET_POINT)

/*Initial values of the calibration variables (selected empirically)*/
extern float		kX;
extern float		kY;
extern uint8_t		offsetX;
extern uint8_t		offsetY;

/*Function prototypes, for more info refer to xpt2046.c*/
void touch_init(void);
void touch_calibr(float *_kX, float *_kY, uint8_t *_offsetX, uint8_t *_offsetY);
bool touch_get_xy(uint16_t *x, uint16_t *y);
void set_touch_calibr(uint8_t _kX, uint8_t _kY, uint8_t _offsetX, uint8_t _offsetY);

/*Pointers to function releted to LCD screen operation*/
extern void (*touch_printf)(const char *fmt, ...);
extern void (*touch_setcursor)(uint16_t x, uint16_t y);
extern void (*touch_fill_screen)(uint16_t color);
extern void (*touch_fill_circle)(int16_t x0, int16_t y0, int16_t r, uint16_t color);
/*Pointer to delay function*/
extern void (*touch_delay_ms)(uint32_t delay);

