/*Copyright (c) 2020 Oleksandr Ivanov.
  *
  * This software component is licensed under MIT license.
  * You may not use this file except in compliance retain the
  * above copyright notice.
  */

#include "ili9325.h"
#include "macro.h"
#include "tick.h"

/**
 *                                 LCD TFT CONTROLLER LIBRARY ILI9325
 *            You need to assign GPIO ports and pins in the pin.h header.
 *            Data transfers from the MCU to ILI9325 via a 16-bit bus such as i8080.
 *            You need to use 16 bit otput register for bus data.
 */

/**
 * Sends the command byte to controller
 * @command: command code
 */
static void ili9325_write_cmd(uint16_t command)
{
	pin_set_f_group(ili9325_ctrl_pins, RS_PIN, PIN_RESET);
	pin_set_f_group(ili9325_ctrl_pins, CS_PIN, PIN_RESET);
	write_port_data(ili9325_data_pins,command);
	pin_set_f_group(ili9325_ctrl_pins, WR_PIN, PIN_RESET);
	pin_set_f_group(ili9325_ctrl_pins, WR_PIN, PIN_SET);
	pin_set_f_group(ili9325_ctrl_pins, CS_PIN, PIN_SET);
}

/**
 * Sends data word to LCD TFT controller
 * @data: data to send
 */
static void ili9325_write_data16(uint32_t data)
{
	pin_set_f_group(ili9325_ctrl_pins, RS_PIN, PIN_SET);
	pin_set_f_group(ili9325_ctrl_pins, CS_PIN, PIN_RESET);
	write_port_data(ili9325_data_pins,data);
	pin_set_f_group(ili9325_ctrl_pins, WR_PIN, PIN_RESET);
	pin_set_f_group(ili9325_ctrl_pins, WR_PIN, PIN_SET);
	pin_set_f_group(ili9325_ctrl_pins, CS_PIN, PIN_SET);
}

/**
 * Write dtata to the register of the LCD TFT controller
 * @registerTFT: register address
 * @dataTFT: data to write
 */
static void ili9325_write_reg(uint16_t registerTFT, uint16_t dataTFT)
{
	ili9325_write_cmd(registerTFT);
	ili9325_write_data16(dataTFT);
}

/**
 * Seting sizes for display.
 * @width: width pixels amount
 * @hight: hight pixels amount
 * @context: should be called before ili9325_tft_init()
 */
void ili9325_set_resol(uint16_t width, uint16_t hight)
{
	tft_width = width;
	tft_hight = hight;
}

/**
 * LCD TFT initialization.
 * @context: should be called before working with the library
 */
void ili9325_init(void)
{
	if(tft_width == 0 || tft_hight == 0) return ; //return if size note seted by ili9325_tft_set_size()
	disp_orient.width = tft_width;
	disp_orient.hight = tft_hight;
	disp_orient.flag = ALBUM;
	disp_orient.current_rot = 0;

	pin_set_f_group(ili9325_ctrl_pins, RESET_PIN, PIN_RESET);
	ili9325_ptr_delay_ms(100);;
	pin_set_f_group(ili9325_ctrl_pins, RESET_PIN, PIN_SET);
	ili9325_ptr_delay_ms(100);
	ili9325_write_reg(0x01, 0x0000);	// Bit 8 - reflection in height (screen orientation)
	ili9325_write_reg(0x02, 0x0700);
	ili9325_write_reg(0x03, 0x1038);	// Bits 5,4,3 - screen sweep direction
	ili9325_write_reg(0x04, 0x0000);
	ili9325_write_reg(0x07, 0x0000);
	ili9325_write_reg(0x08, 0x0302);
	ili9325_write_reg(0x09, 0x0000);
	ili9325_write_reg(0x0A, 0x0000);
	ili9325_write_reg(0x0C, 0x0000);
	ili9325_write_reg(0x0D, 0x0000);
	ili9325_write_reg(0x0F, 0x0000);
	ili9325_write_reg(0x10, 0x1290);
	ili9325_write_reg(0x11, 0x0007);
	ili9325_ptr_delay_ms(100);
	ili9325_write_reg(0x12, 0x0092);
	ili9325_ptr_delay_ms(100);
	ili9325_write_reg(0x13, 0x0E00);
	ili9325_ptr_delay_ms(100);
	ili9325_write_reg(0x29, 0x0005);
	ili9325_write_reg(0x2B, 0x000E);
	ili9325_write_reg(0x30, 0x0004);
	ili9325_write_reg(0x31, 0x0307);
	ili9325_write_reg(0x32, 0x0002);
	ili9325_write_reg(0x35, 0x0206);
	ili9325_write_reg(0x36, 0x0A00);
	ili9325_write_reg(0x37, 0x0507);
	ili9325_write_reg(0x38, 0x0204);
	ili9325_write_reg(0x39, 0x0707);
	ili9325_write_reg(0x3C, 0x0405);
	ili9325_write_reg(0x3D, 0x0000);
	ili9325_write_reg(0x50, 0x0000);
	ili9325_write_reg(0x51, 0x00EF);
	ili9325_write_reg(0x52, 0x0000);
	ili9325_write_reg(0x53, 0x013F);
	ili9325_write_reg(0x60, 0xA700);	// bit 15 - flip width (screen orientation)
	ili9325_write_reg(0x61, 0x0001);
	ili9325_write_reg(0x6A, 0x0000);
	ili9325_write_reg(0x80, 0x0000);
	ili9325_write_reg(0x81, 0x0000);
	ili9325_write_reg(0x82, 0x0000);
	ili9325_write_reg(0x83, 0x0000);
	ili9325_write_reg(0x84, 0x0000);
	ili9325_write_reg(0x85, 0x0000);
	ili9325_write_reg(0x90, 0x0029);
	ili9325_write_reg(0x92, 0x0600);
	ili9325_write_reg(0x93, 0x0003);
	ili9325_write_reg(0x95, 0x0110);
	ili9325_write_reg(0x97, 0x0000);
	ili9325_write_reg(0x98, 0x0000);
	ili9325_write_reg(0x07, 0x0133);

	ili9325_fill_screen(0x0000); // fill screen black

}

/**
 * Seting the area for drawing
 * @w1: Vertical start coordinate
 * @h1: Horizontal start coordinate
 * @w2: End vertical coordinate
 * @h2: End horizontal coordinate
 * 
 * @return:TFT_EOK if success or TFT_ERANGE if not
 */
tft_err ili9325_set_frame(uint16_t w1, uint16_t h1, uint16_t w2, uint16_t h2)
{
	if ((w1 >= disp_orient.width)||(h1 >= disp_orient.hight)
	    ||(w2 >= disp_orient.width)||(h2 >= disp_orient.hight)){
		return TFT_ERANGE;
	}
	if(disp_orient.flag == PORTRATE) {
		swap(w1, h1);
		swap(w2, h2);
	}
	ili9325_write_reg(0x50,h1);
	ili9325_write_reg(0x51,h2);
	ili9325_write_reg(0x52,w1);
	ili9325_write_reg(0x53,w2);
	/*Positioning the cursor at the beginning of the area*/
	ili9325_write_reg(0x20,h1);
	ili9325_write_reg(0x21,w1);

	/*Command of begining writing to GRAM(graphic memory)*/
	ili9325_write_cmd(GRAM_R);//

	return TFT_EOK;
}

/**
 * Pixel drawing
 * @w: Width coordinate
 * @h: Height coordinate
 * @color: Pixel color
 */
void ili9325_draw_pixel(uint16_t w, uint16_t h, uint16_t color)
{
	if (w >= disp_orient.width
		|| h >= disp_orient.hight) return;
	if(disp_orient.flag == PORTRATE) swap(w, h);
	ili9325_write_reg(0x20,h);
	ili9325_write_reg(0x21,w);
	ili9325_write_reg(0x22,color);
}


/**
 * Filling the screen by color
 * @color: color to fill
 */
void ili9325_fill_screen(uint16_t color)
{
	uint32_t i = (uint32_t)(disp_orient.width)*(disp_orient.hight);
	ili9325_set_frame(0, 0, disp_orient.width-1, disp_orient.hight-1);
	while(i--) ili9325_write_data16(color);
}

/**
 * Change screen orientation relative to default
 * @rot_degrees: Rotation in degrees related to default orientation.Possible values: 0 or 360(album), 90(portrait), 180(reversed album), 270(reversed portrait)
 * 
 * @return:TFT_EOK if success or TFT_EWRONGARG if not
 */
tft_err ili9325_rotate_screen(uint16_t rot_degrees)
{
	switch (rot_degrees) {
	case 0:
	case 360:
		disp_orient.width = tft_width;
		disp_orient.hight = tft_hight;
		disp_orient.flag = ALBUM;
		disp_orient.current_rot = 0;
		ili9325_write_reg(0x60, 0xA700); //register bit 15 set 1
	    ili9325_write_reg(0x01, 0x0000); //register bit 8 set 0
	    ili9325_write_reg(0x03, 0x1038); //register bits 5,4,3 set 111
	    break;
	case 90:
		disp_orient.width = tft_hight;
		disp_orient.hight = tft_width;
		disp_orient.flag = PORTRATE;
		disp_orient.current_rot = 90;
		ili9325_write_reg(0x60, 0x2700); //register bit15 is 0
		ili9325_write_reg(0x01, 0x0000); //register bit 8 is 0
		ili9325_write_reg(0x03, 0x1030); //register bits 5,4,3 set 110
		break;
	case 180:
		disp_orient.width = tft_width;
		disp_orient.hight = tft_hight;
		disp_orient.flag = ALBUM;
		disp_orient.current_rot = 180;
		ili9325_write_reg(0x60, 0x2700); //register bit 15 is 0
		ili9325_write_reg(0x01, 0x0100); //register bit 8 is 1
		ili9325_write_reg(0x03, 0x1038); //register bits 5,4,3 set 111
		break;
	case 270:
		disp_orient.width = tft_hight;
		disp_orient.hight = tft_width;
		disp_orient.flag = PORTRATE;
		disp_orient.current_rot = 270;
		ili9325_write_reg(0x60, 0xA700); //register bit 15 is 1
		ili9325_write_reg(0x01, 0x0100); //register bit 8 is 1
		ili9325_write_reg(0x03, 0x1030); //register bits 5,4,3 set 110
		break;
	default:
		return TFT_EWRONGARG;
	}
	return TFT_EOK;
}

/**
 * Pixel drawing for dedicated frame
 * @color: Pixel color
 */
void ili9325_frame_draw_pixel(uint16_t color)
{
	ili9325_write_data16(color);
}

/**
 * Reset the screen
 */
void ili9325_screen_reset(void)
{
	pin_set_f_group(ili9325_ctrl_pins, RESET_PIN, PIN_RESET);
	ili9325_ptr_delay_ms(100);;
	pin_set_f_group(ili9325_ctrl_pins, RESET_PIN, PIN_SET);
	ili9325_ptr_delay_ms(100);
	if (disp_orient.current_rot == 0 || disp_orient.current_rot == 90) {
		ili9325_write_reg(0x01, 0x0000);
	} else if (disp_orient.current_rot == 180
			|| disp_orient.current_rot == 270) {
		ili9325_write_reg(0x01, 0x0100);
	}
	ili9325_write_reg(0x02, 0x0700);
	if (disp_orient.current_rot == 0 || disp_orient.current_rot == 180) {
		ili9325_write_reg(0x03, 0x1038);
	} else if (disp_orient.current_rot == 90
			|| disp_orient.current_rot == 270) {
		ili9325_write_reg(0x03, 0x1030);
	}
	ili9325_write_reg(0x04, 0x0000);
	ili9325_write_reg(0x07, 0x0000);
	ili9325_write_reg(0x08, 0x0302);
	ili9325_write_reg(0x09, 0x0000);
	ili9325_write_reg(0x0A, 0x0000);
	ili9325_write_reg(0x0C, 0x0000);
	ili9325_write_reg(0x0D, 0x0000);
	ili9325_write_reg(0x0F, 0x0000);
	ili9325_write_reg(0x10, 0x1290);
	ili9325_write_reg(0x11, 0x0007);
	ili9325_ptr_delay_ms(100);
	ili9325_write_reg(0x12, 0x0092);
	ili9325_ptr_delay_ms(100);
	ili9325_write_reg(0x13, 0x0E00);
	ili9325_ptr_delay_ms(100);
	ili9325_write_reg(0x29, 0x0005);
	ili9325_write_reg(0x2B, 0x000E);
	ili9325_write_reg(0x30, 0x0004);
	ili9325_write_reg(0x31, 0x0307);
	ili9325_write_reg(0x32, 0x0002);
	ili9325_write_reg(0x35, 0x0206);
	ili9325_write_reg(0x36, 0x0A00);
	ili9325_write_reg(0x37, 0x0507);
	ili9325_write_reg(0x38, 0x0204);
	ili9325_write_reg(0x39, 0x0707);
	ili9325_write_reg(0x3C, 0x0405);
	ili9325_write_reg(0x3D, 0x0000);
	ili9325_write_reg(0x50, 0x0000);
	ili9325_write_reg(0x51, 0x00EF);
	ili9325_write_reg(0x52, 0x0000);
	ili9325_write_reg(0x53, 0x013F);
	if (disp_orient.current_rot == 0 || disp_orient.current_rot == 270) {
		ili9325_write_reg(0x60, 0xA700);
	} else if (disp_orient.current_rot == 90
			|| disp_orient.current_rot == 180) {
		ili9325_write_reg(0x60, 0x2700);
	}
	ili9325_write_reg(0x61, 0x0001);
	ili9325_write_reg(0x6A, 0x0000);
	ili9325_write_reg(0x80, 0x0000);
	ili9325_write_reg(0x81, 0x0000);
	ili9325_write_reg(0x82, 0x0000);
	ili9325_write_reg(0x83, 0x0000);
	ili9325_write_reg(0x84, 0x0000);
	ili9325_write_reg(0x85, 0x0000);
	ili9325_write_reg(0x90, 0x0029);
	ili9325_write_reg(0x92, 0x0600);
	ili9325_write_reg(0x93, 0x0003);
	ili9325_write_reg(0x95, 0x0110);
	ili9325_write_reg(0x97, 0x0000);
	ili9325_write_reg(0x98, 0x0000);
	ili9325_write_reg(0x07, 0x0133);

	ili9325_fill_screen(0x0000); // fill screen black
}

struct display_setings disp_orient = {
		.current_rot = 0,
		.flag = ALBUM,
		.hight = 0,
		.width = 0
};

uint16_t tft_width = 0;
uint16_t tft_hight = 0;
