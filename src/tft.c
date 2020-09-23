/*Copyright (c) 2020 Oleksandr Ivanov.
  *
  * This software component is licensed under MIT license.
  * You may not use this file except in compliance retain the
  * above copyright notice.
  */

#include "tft.h"
#include "ili9325.h"
#include "macro.h"
#include <stdlib.h>
#include <math.h>
#include <libprintf/printf.h>
// #include <stdio.h>
// #include <stdarg.h>

/**
 *                                     GRAPHIC LIBRARY
 * This library is based on using a function from the library for the controller ili9325.
 *
 */

/**
 * Initialization TFT.
 * @width: width pixels amount
 * @hight: hight pixels amount
 * @color: color to fill screen after initialization
 * @context: should be called first
 */
void tft_init(uint16_t width, uint16_t hight, uint16_t color)
{
	ili9325_set_resol(width, hight);
	ili9325_init();
	tft_set_font(COURIER_NEW_8_NORM, WHITE, BLACK);
	tft_set_cursor(0, 0);
	tft_fill_screen(color);
}

/**
 * Font settings
 * @type:  Font type.
 * @color: Font color
 * @back:  Font background color
 */
void tft_set_font(uint8_t type,	uint16_t color, uint16_t back_color)
{
	font_color = color;
	font_back_color = back_color;
	switch(type) {
		case COURIER_NEW_8_BOLD:	font_type = font8;
				break;
		case COURIER_NEW_12_BOLD:	font_type = font12;
				break;
		case COURIER_NEW_8_NORM: 	font_type = font8_normal;
				break;
		case COURIER_NEW_20_NORM:   font_type = font20_normal;
				break;
		case UBUNTUMONO_14_NORM:  	font_type = font14_ubuntu;
				break;
		case SEVEN_SEGMENT:         font_type = seven_seg_num_font;
				break;
		default:font_type= font8_normal;
		        break;
	}

	font_width	= font_type[0];	 // Width in pixels
	font_height	= font_type[1];	 // Height in pixels
	font_byte	= font_type[2];	 // Amounts of bytes for one sumbol
	offset_char	= font_type[3];	 // Offset to the beginning of the symbol table
}

/**
 * Drawing the line(Bresenham's algorithm)
 * @x1:    line start coordinate x
 * @y1:    line start coordinate y
 * @x2:    line end coordinate x
 * @y2:    line end coordinate y
 * @width: line width
 * @color: line color
 */
void tft_draw_line(uint16_t x1, uint16_t y1, uint16_t x2,uint16_t y2, uint8_t width, uint16_t color)
{
	int deltaX = abs(x2 - x1);				// Finding the difference between x2 and x1 by module
	int deltaY = abs(y2 - y1);				// Finding the difference between y2 and y1 by module
	int signX = (x1 < x2) ? 1 : -1;			// Select the direction of drawing along the x axis
	int signY = (y1 < y2) ? 1 : -1;			// Select the direction of drawing along the y axis
	int error = deltaX - deltaY;			// Calculate the difference between the x and y offsets

	while(1)
	{
		tft_draw_point(x1, y1, width, color);
		if(x1 == x2 && y1 == y2) return;	// End of line drawing when coordinates match
		int error2 = error*2;

		if(error2 > -deltaY) {				// Estimate the error
			error -= deltaY;				// Error correction
			x1 += signX;					// Move to the next coordinate along the x axis
		}

		if(error2 < deltaX) {				// Estimate the error
			error += deltaX;				// Error correctionи
			y1 += signY;					// Move to the next coordinate along the y axis
		}
	}
}

/**
 * Drawing rectangle
 * @x: start coordinate x
 * @y: start coordinate y
 * @longX: x axis length
 * @longY: y axis length
 * @width: line width
 * @color: line color
 */
void tft_draw_rectangle(uint16_t x, uint16_t y,	uint16_t longX, uint16_t longY, uint8_t width, uint16_t color)
{
	tft_draw_line(x, y, x+longX, y, width, color);
	tft_draw_line(x, y, x, y+longY, width, color);
	tft_draw_line(x, y+longY, x+longX, y+longY, width, color);
	tft_draw_line(x+longX, y, x+longX, y+longY, width, color);
}

/**
 * Fill rectangle with color
 * @x: start coordinate x
 * @y: start coordinate y
 * @longX: x axis length
 * @longY: y axis length
 * @color: fill color
 * @return: TFT_EOK if success or TFT_ERANGE if not
 */
tft_err tft_fill_rectangle(uint16_t x, uint16_t y, uint16_t longX, uint16_t longY, uint16_t color)
 {
 	uint32_t i = (uint32_t)longX*longY;	// Calculating the total number of pixels of the area
 	if(tft_set_frame(x, y, x + longX -1 , y + longY - 1)) {
 		return TFT_ERANGE;
 	}
 	while(i--) tft_frame_draw_pixel(color);
 	return TFT_EOK;
 }


/**
 * Drawing circle
 * @x0: start coordinate x
 * @y0: start coordinate y
 * @radius: x axis length
 * @width: line width
 * @color: line color
 */
void tft_draw_circle (uint16_t x0, uint16_t y0, uint16_t radius, uint8_t width, uint16_t color)
{
	int x = -radius, y = 0, err = 2 - 2 * radius, e2;
	do {

		/*Arc of 1st quarter*/
		tft_draw_point(x0 + x, y0 + y, width, color);
		/*Arc of 2nd quarter*/
		tft_draw_point(x0 + x, y0 - y, width, color);
		/*Arc of 3rd quarter*/
		tft_draw_point(x0 - x, y0 - y, width, color);
		/*Arc of 4th quarter*/
		tft_draw_point(x0 - x, y0 + y, width, color);

		e2 = err;
		if (e2 <= y) {
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x) e2 = 0;
		}
		if (e2 > x) err += ++x * 2 + 1;
	}
	while (x <= 0);
}

/**
 * Filling the circle with color
 * @x0: start coordinate x
 * @y0: start coordinate y
 * @radius: x axis length
 * @color: fill color
 */
void tft_fill_circle (uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
{
	int x = -radius, y = 0, err = 2 - 2 * radius, e2;
	do {
		tft_draw_line(x0 - x, y0 - y, x0 - x, y0 + y, 1, color);
		tft_draw_line(x0 + x, y0 - y, x0 + x, y0 + y, 1, color);
		e2 = err;
		if (e2 <= y) {
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x) e2 = 0;
		}
		if (e2 > x) err += ++x * 2 + 1;
	}
	while (x <= 0);
}

/**
 * Drawing triangle
 * @x0: coordinate x of the first point
 * @y0: coordinate y of the first point
 * @x1: coordinate x of the second point
 * @y1: coordinate y of the second point
 * @x2: coordinate x of the third point
 * @y2: coordinate y of the third point
 * @width: line width
 * @color: line color
 */

void tft_draw_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		               uint16_t x2, uint16_t y2, uint8_t width,  uint16_t color)
{
	tft_draw_line(x0, y0, x1, y1, width, color);
	tft_draw_line(x1, y1, x2, y2, width, color);
	tft_draw_line(x2, y2, x0, y0, width, color);
}


/**
 * Fill triangle.
 * @x0: coordinate x of the first point
 * @y0: coordinate y of the first point
 * @x1: coordinate x of the second point
 * @y1: coordinate y of the second point
 * @x2: coordinate x of the third point
 * @y2: coordinate y of the third point
 * @color: fill color
 * @return: TFT_EOK if success or TFT_ERANGE if not
 */
tft_err tft_fill_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint16_t x2, uint16_t y2, uint16_t color) {

	int16_t a, b, y, last;
	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}
	if (y1 > y2) {
		swap(y2, y1);
		swap(x2, x1);
	}
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}
	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)
			a = x1;
		else if (x1 > b)
			b = x1;
		if (x2 < a)
			a = x2;
		else if (x2 > b)
			b = x2;
		tft_draw_fast_h_lin(a, y0, b - a + 1, color);
		return TFT_ERANGE;
	}

	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
			dx12 = x2 - x1, dy12 = y2 - y1;
	int32_t sa = 0, sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2)
		last = y1;   // Include y1 scanline
	else
		last = y1 - 1; // Skip it

	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		if (a > b)
			swap(a, b);
		tft_draw_fast_h_lin(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if (a > b)
			swap(a, b);
		if(tft_draw_fast_h_lin(a, y, b - a + 1, color)) {;
			return TFT_ERANGE;
		}
	}
	return TFT_EOK;
}

/**
 * Drawing the horizontal 1 pixel width line.
 * @x:      line start coordinate x
 * @y:      line start coordinate y
 * @length: line length
 * @color:  line color
 * @return: TFT_EOK if success or TFT_ERANGE if not
 */
tft_err tft_draw_fast_h_lin(int16_t x, int16_t y, int16_t length, uint16_t color)
{
	int16_t x2;
	// Initial off-screen clipping
	if ((length <= 0) || (y < 0) || (y >= disp_orient.hight) || (x >= disp_orient.width) ||
		((x2 = (x + length - 1)) < 0)) return TFT_ERANGE;

	if (x < 0) { // Clip left
		length += x;
		x = 0;
	}
	if (x2 >= disp_orient.width) { // Clip right
		x2 = disp_orient.width - 1;
		length = x2 - x + 1;
	}

	if(tft_set_frame(x, y, x2, y)) return TFT_ERANGE;
	while(length--) tft_frame_draw_pixel(color);

	return TFT_EOK;
}

/**
 * Drawing the vertical 1 pixel width line.
 * @x:      line start coordinate x
 * @y:      line start coordinate y
 * @length: line length
 * @color:  line color
 * @return: TFT_EOK if success or TFT_ERANGE if not
 */
tft_err tft_draw_fast_v_lin(int16_t x, int16_t y, int16_t length, uint16_t color) {

	int16_t y2;
	// Initial off-screen clipping
	if ((length <= 0) || (x < 0) || (x >= disp_orient.width) || (y >= disp_orient.hight) ||
		((y2 = (y + length - 1)) <  0)) return TFT_ERANGE;

	if (y < 0) { // Clip top
		length += y;
		y = 0;
	}
	if (y2 >= disp_orient.hight) { // Clip bottom
		y2 = disp_orient.hight - 1;
		length = y2 - y + 1;
	}
	if(tft_set_frame(x, y, x, y2)) return TFT_ERANGE;
	while(length--) tft_frame_draw_pixel(color);

	return TFT_EOK;
}

/**
 * Drawing picture from the flash memory.
 * @x: start coordinate x
 * @y: start coordinate y
 * @img: image data array of 16 bit words
 * @return: TFT_EOK if success or TFT_ERANGE if not
 */
tft_err tft_pic_from_flash(uint16_t x, uint16_t y, const uint16_t* img)
{
	uint16_t width = img[0];
	uint16_t height = img[1];
	uint16_t color;

	if(tft_set_frame(x, y, x+width - 1, y + height - 1)) {
		return TFT_ERANGE;
	}

	for (uint16_t n = 2; n < width * height + 2; n++) {
		color = img[n];								     // Read 2 bytes of pixel color from array
		tft_frame_draw_pixel(color);
	}
	return TFT_EOK;
}

/**
 * Printing char
 * @x: start coordinate x
 * @y: start coordinate y
 * @ascii: ASCII symbol
 * @return: true if success and false if not
 * @context: TFT_EOK if success or TFT_ERANGE if not
 * */
tft_err tft_print_char(uint16_t x, uint16_t y, uint16_t ascii)
{
	uint16_t	line;
	uint8_t		temp, pos = 0;

	if(x > disp_orient.width - font_width || y > disp_orient.hight - font_height) return TFT_ERANGE;
	if(tft_set_frame(x, y, x + font_width-1, y + font_height - 1)) return TFT_ERANGE;

	if (ascii >= 192) ascii -= 64;
	ascii -= offset_char;					          // Symbol code of the beginning of the symbol table
	ascii = ascii * font_byte + 4;			          // Determining the address of the beginning of a symbol in an array
	for(uint8_t j=0; j < font_height; j++) {
		line = font_width;
		while(line != 0) {
			temp = font_type[ascii + (pos++)];
			for(uint8_t i = 0; (i < 8) & (line != 0); i++, line--) {
				if (temp & 0x80) {									// Highlighting the most significant bit
					tft_frame_draw_pixel(font_color);           // Passing the pixel color of a char
				}
				else {
					tft_frame_draw_pixel(font_back_color);      // Passing the pixel color of a background
				}
				temp <<= 1;										// Go to the next digit
			}
		}
	}
	return TFT_EOK;
}

/**
 * Printing the natural number
 * @x: coordinate x of first digit
 * @y: coordinate y of first digit
 * @num: number to print
 * @len: Places amount for digit, if len > num length the space will be replaced by ' '
 * @return: TFT_EOK if success or TFT_ERANGE if not
 * @context: Set the font by tft_set_font() before using
 */
tft_err tft_print_num(	uint16_t x, uint16_t y, uint32_t num, uint8_t len)
{
	uint8_t t, temp;
	uint8_t enshow = 0;

	for(t = 0; t < len; t++) {
		temp = (num / (uint32_t)pow(10, len - t - 1)) % 10;
		if( enshow == 0 && t < (len - 1)) {
			if(temp == 0) {
				/*Printing a space if there is no number*/
				if(tft_print_char(x, y, ' ')) {
					return TFT_ERANGE;
				}
				x += font_width;				// Step to next
				continue;
			}
			else {
				enshow = 1;
			}
		}
		if(tft_print_char(x, y, temp + '0')) {
			return TFT_ERANGE;
		}
		x += font_width;						// Step to next
	}
	return TFT_EOK;
}

/**
 * Printing the natural number with 0 instead ' '
 * @x: coordinate x of first digit
 * @y: coordinate y of first digit
 * @num: number to print
 * @len: Places amount for digit, if len > num length the space will be replaced by 0
 * @return: TFT_EOK if success or TFT_ERANGE if not
 * @context: Set the font by tft_set_font() before using
 */
tft_err tft_print_num0(uint16_t x, uint16_t y, uint32_t num, uint8_t len)
{
	uint8_t t, temp;
	for(t = 0; t < len; t++) {
		temp = (num / (uint32_t)pow(10, len - t - 1)) % 10;
		if(tft_print_char(x, y, temp + '0')) {
			return TFT_ERANGE;
		}
		x += font_width;
	}
	return TFT_EOK;
}

/**
 * String printing
 * @x: coordinate x of first symbol
 * @y: coordinate y of first symbol
 * @str: string to print
 * 
 * @return:TFT_EOK if success or TFT_ERANGE if not
 */
tft_err tft_print_str(uint16_t x, uint16_t y, const char *str)
{
	while(*str != 0) {
		if(x > disp_orient.width - font_width){
			y += font_height; x = 0;
		}
		if(y > disp_orient.hight - font_height) {
			y = 0; x = 0;
		}
		if(tft_print_char(x, y, *str)) {
			return TFT_ERANGE;
		}
		x += font_width;
		str++;
	}
	return TFT_EOK;
}

/**
 * Drawing the square dot with size and color
 * @w: Width coordinate
 * @h: Height coordinate
 * @size: Point side size (limited by size * size <65535)
 * @color: Dot color
 * 
 * @return:TFT_EOK if success or TFT_ERANGE if not
 */
tft_err tft_draw_point(uint16_t w, uint16_t h, uint8_t size, uint16_t color)
{
	if (w + size >= disp_orient.width
		|| h + size >= disp_orient.hight) return TFT_ERANGE;
	uint16_t i = (uint16_t)size*size;
	if(i >= 65535) return TFT_ERANGE;
	if(tft_set_frame(w, h, w+size-1, h+size-1)) return TFT_ERANGE;
	while(i--)	tft_frame_draw_pixel(color);
	return TFT_EOK;
}

/**
 * Sets the cursor coordinates for tft_printf()
 * @x: x-coordinate.
 * @y: y-coordinate.
 */
void tft_set_cursor(uint16_t x, uint16_t y)
{
	cursor_x = x;
	cursor_y = y;
}

/**
 * Print the specified text
 * @fmt:format text.
 * @note: use float with printf from newlib-nano(-u_printf_float)
 */
void tft_printf(const char *fmt, ...)
{
	static char buf[256];
	char *p;
	va_list lst;

	va_start(lst, fmt);
	vsnprintf(buf, sizeof(buf), fmt, lst);
	va_end(lst);

	volatile uint16_t height, width;
	height = font_height;
	width = font_width;
	p = buf;

	while (*p) {
		if (*p == '\n') {
			cursor_y += height;
			cursor_x = 0;
		} else if (*p == '\r') {
			cursor_x = 0;
		} else if (*p == '\t') {
			cursor_x += width * 4;
		} else {
			if (cursor_y >= (tft_hight - height)) {
				cursor_y = 0;
			}
			tft_print_char(cursor_x, cursor_y, *p);
			cursor_x += width;
			if (!disp_orient.flag && (cursor_x > (tft_width - width))) {
				cursor_y += height;
				cursor_x = 0;
			} else if(disp_orient.flag && (cursor_x > (tft_hight - width))) {
				cursor_y += height;
				cursor_x = 0;
			}
		}
		p++;
	}

}


/**
 * Colors testing. Shows all posibal colors line by line.
 * */
void tft_colors_test(void) {

	unsigned int i, j;
	tft_fill_screen(BLACK);


	for (i = 0; i < disp_orient.hight; i++) {
		for (j = 0; j < disp_orient.width; j++) {
			if (i < 15) {

				tft_frame_draw_pixel(WHITE);
			}
			else if (i < 30) {
				tft_frame_draw_pixel(BLACK);
			}
			else if (i < 45) {
				tft_frame_draw_pixel(BLUE);
			}
			else if (i < 60) {
				tft_frame_draw_pixel(RED);
			}
			else if (i < 75) {
				tft_frame_draw_pixel(MAGENTA);
			}
			else if (i < 90) {
				tft_frame_draw_pixel(GREEN);
			}
			else if (i < 105) {
				tft_frame_draw_pixel(CYAN);
			}
			else if (i < 120) {
				tft_frame_draw_pixel(YELLOW);
			}
			else if (i < 135) {
				tft_frame_draw_pixel(BROWN);
			}
			else if (i < 150) {
				tft_frame_draw_pixel(BRRED);
			}
			else if (i < 165) {
				tft_frame_draw_pixel(GRAY);
			}
			else if (i < 180) {
				tft_frame_draw_pixel(DARKBLUE);
			}
			else if (i < 195) {
				tft_frame_draw_pixel(LIGHTBLUE);
			}
			else if (i < 210) {
				tft_frame_draw_pixel(GRAYBLUE);
			}
		}
	}
}

uint16_t cursor_x = 0;
uint16_t cursor_y = 0;

tft_err (*tft_set_frame)(uint16_t w1, uint16_t h1, uint16_t w2, uint16_t h2) = ili9325_set_frame;
void (*tft_frame_draw_pixel)(uint16_t color) = ili9325_frame_draw_pixel;
void (*tft_fill_screen)(uint16_t color) = ili9325_fill_screen;
tft_err (*tft_rotate_screen)(uint16_t rot_degrees)= ili9325_rotate_screen;
void (*tft_reset)(void) = ili9325_screen_reset;



