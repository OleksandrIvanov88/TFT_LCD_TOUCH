/*Copyright (c) 2020 Oleksandr Ivanov.
  *
  * This software component is licensed under MIT license.
  * You may not use this file except in compliance retain the
  * above copyright notice.
  */
#include "xpt2046.h"
#include "ili9325.h"
#include "mcu_init.h"
#include "tft.h"
#include <stddef.h>

/**
		          TOUCH SCREEN LIBRARY FOR CONTROLLER XPT2046

The library required the inclusion of header files of the graphics library and display controller.
In the header file, configure the pins of the SPI program interface
Before using the library, call the init_xpt2046 () initialization function
If necessary, define the order of coordinates of the touch screen SWAP_XY, MIRROR_X, MIRROR_Y
*/

/*The number of cycles to read the coordinate*/
#define JITTER  20
/*The number of maximum and minimum discarded values (JITTER_CUT must be less than JITTER / 2)*/
const uint8_t JITTER_CUT = 3;
/*Threshold value of the pressing force (from 0 to 4095)*/
const uint16_t	JITTER_PRESS = 250;
/*Maximum value of 12-bit ADC*/
const uint16_t MAX_ADC_TOUCH = 4095;

/*Defining commands for activating different measurement modes of ADC
 * by the controller XPT2046*/
#ifndef SWAP_XY
	const uint8_t GET_TOUCH_X = 0xD2;   //0b11010010, ADC value for X coordinate
	const uint8_t GET_TOUCH_Y =	0x92;   //0b10010010, ADC value for Y coordinate
#else
	const uint8_t GET_TOUCH_Y =	0xD2;   //0b11010010, ADC value for Y coordinate
	const uint8_t GET_TOUCH_X =	0x92;   //0b10010010, ADC value for X coordinate
#endif

const uint8_t GET_TOUCH_Z1 = 0xB2;      //0b10110010, pressing force Z1
const uint8_t GET_TOUCH_Z2 = 0xC2;      //0b11000010, pressing force Z2

/*Initial values of the calibration value(selected empirically)*/
float kX = 11.23;         //kX - proportional coefficient X axis
float kY = 15.46;         //kY - proportional coefficient Y axis
uint8_t offsetX = 14;     //X origin offset
uint8_t offsetY = 12;     //Y origin offset

/**
 * ADC staring measuring
 * @cmd: commands for activating measuring ADC mode
 * @return: ADC value
 */
static uint16_t conversion(uint8_t cmd)
{
	uint16_t adc = 0;
	uint8_t buffer_rx[3] ={0};
	uint8_t buffer_tx[3] ={0xFF,0xFF,0xFF};
	buffer_tx[0] = cmd;
	pin_set(touch_cs_pin, PIN_RESET);
	touch_spi_tx_rx(SPI_FUNC, buffer_rx, buffer_tx);
	adc = buffer_rx[1];
	adc &= ~(1<<7);									  // Most significant bit is not using
	adc <<= 8;										  // Byte shift to high byte of a variable
	adc |= buffer_rx[2];							  // Reception of the least significant 8 bits
	pin_set(touch_cs_pin, PIN_SET);
	adc >>= 3;										  // The least significant 3 bits are not used.
	return adc;
}
/**
 * XPT2046 initialization
 */
void touch_init(void)
{
 	conversion(GET_TOUCH_X);	// Skip the first conversion command.
 	touch_delay_ms(1);
}

/**
 * Reading the ADC values of the coordinate
 * @adcX: pointer to a variable to write the ADC value for the coordinate X
 * @adcY: pointer to a variable to write the ADC value for the coordinate Y
 * @return: true if touch detected and false if not detected
 */
static bool get_adc_xy(uint16_t *adcX, uint16_t *adcY)
{
	uint16_t press;

	press = MAX_ADC_TOUCH - conversion(GET_TOUCH_Z2); // Reading the pressing force Z2
	press += conversion(GET_TOUCH_Z1);                // Reading the pressing force Z1 and addition to Z2
	press /= 2;

	if (press > JITTER_PRESS) {				          // Assessment of the pressing force to the touchscreen

#ifdef MIRROR_X
		*adcX = conversion(GET_TOUCH_X);
#else
		*adcX = MAX_ADC_TOUCH - conversion(GET_TOUCH_X);
		#endif

#ifdef MIRROR_Y
		*adcY = conversion(GET_TOUCH_Y);
		#else
		*adcY = MAX_ADC_TOUCH - conversion(GET_TOUCH_Y);
#endif

		return true;
	}
	return false;
}

/**
 *  Finds average value of the array
 *  @array: array of values
 * 
 *  @return: average integer value
 */
static uint16_t average_array(uint16_t* array)
{
    uint32_t temp = 0;
    /*Sorting array elements using the bubble method*/
    for (uint8_t i = 0; i < JITTER - 1; i++) {
        for (uint8_t j = 0; j < JITTER - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
    temp = 0;
    /*Excluding extreme elements of array*/
    for (uint8_t i = JITTER_CUT; i < JITTER - JITTER_CUT; ++i) {
        temp += array[i];
    }
    temp /= JITTER - JITTER_CUT * 2;
    return temp;
}

/**
 * Processing x and y coordinate from the ADC value in the touch point
 * @touchX: pointer to a variable to write the value of the coordinate X
 * @touchY: pointer to a variable to write the value of the coordinate Y
 * @return: true if coordinate read successfully and false if not.
 */
bool touch_get_xy(uint16_t *touchX, uint16_t *touchY)
{
	if(touchX == NULL && touchY == NULL) return false;
	uint16_t adcX;
	uint16_t adcY;
	uint16_t arrayX[JITTER] = {0};
	uint16_t arrayY[JITTER] = {0};

	/*Cycle for multiple reading of the touch point coordinate*/
	for(uint8_t i=0; i<JITTER; i++)
	{
		if (get_adc_xy(&arrayX[i], &arrayY[i]) == false){
			return false;
		}
	}
	adcX = average_array(arrayX);
	adcY = average_array(arrayY);

	*touchX = adcX / kX - offsetX;
	*touchY = adcY / kY - offsetY;

	/*If the values go beyond the screen, then exit the function*/
	if (*touchX >= X_PIX_MAX) return false;
	if (*touchY >= Y_PIX_MAX) return false;

	return true;
}

/**
 * Shows the massage of the calibration begining
 */
static void calibr_start(void)
{
	touch_fill_screen(BLACK);
	touch_setcursor(0, Y_PIX_MAX / 4);
	touch_printf("\t\t\tSCREEN CALIBRATION\n");
	touch_printf("\t\tTouch points on the screen\n");
    // Display on the screen 4 calibration points
	touch_fill_circle(POINT1_X, POINT1_Y, 5, GREEN);
	touch_fill_circle(POINT2_X, POINT2_Y, 5, GREEN);
	touch_fill_circle(POINT3_X, POINT3_Y, 5, GREEN);
	touch_fill_circle(POINT4_X, POINT4_Y, 5, GREEN);
}

/**
 * Shows that curent point calibration compleated
 *  @point_x:  X coordinate of calibrated point
 *  @point_y:  Y coordinate of calibrated point
 */
static void set_point(uint16_t point_x, uint16_t point_y)
{
	touch_fill_circle(point_x, point_y, 5, RED);    // Change color of calibration point
}

/**
 *  Shows the massage about calibration finish
 */
static void calibr_stop(void)
{
	touch_fill_screen(BLACK);
	touch_setcursor(1, 1);
	touch_printf("SCREEN CALIBRATION FINISHED\n");
	touch_printf("kX = %4f\n", kX);
	touch_printf("kY = %4f\n", kY);
	touch_printf("offsetX = %4d\n", offsetX);
	touch_printf("offsetY = %4d\n", offsetY);
	touch_delay_ms(5000);
	touch_fill_screen(BLACK);
}

/**
 *  Calibrates touch screen
 *  @_kX - proportional coefficient X axis
 *  @_kY - proportional coefficient Y axis
 *  @_offsetX - X origin offset
 *  @_offsetY - Y origin offset
 */
void touch_calibr(float *_kX, float *_kY, uint8_t *_offsetX, uint8_t *_offsetY)
{
	if(_kX == NULL && _kX == NULL
		&& _offsetX == NULL && _offsetY == NULL) return;
	enum POINTS {
		LEFT_TOP_1 = 0,
		RIGHT_TOP_2,
		RIGHT_DOWN_3,
		LEFT_DOWN_4
	};
    uint8_t MeasureFlags = 0;
    uint16_t adcX;
    uint16_t adcY;
    uint16_t arrayX[JITTER];
    uint16_t arrayY[JITTER];
    uint16_t pointX[4] = { 0 };
    uint16_t pointY[4] = { 0 };


    calibr_start();

    while (MeasureFlags != 0x0F) {
    	while (get_adc_xy(&adcX, &adcY) == false) {};
    	touch_delay_ms(100);
    	/*Cycle for multiple reading of the touch point coordinate*/
		for(uint8_t i=0; i<JITTER; i++) {
			if(get_adc_xy(&arrayX[i], &arrayY[i]) == false)	{
				i = 0;											// Restart the read cycle
			}
			touch_delay_ms(5);
		}
		adcX = average_array(arrayX);
		adcY = average_array(arrayY);

        if ((adcX < MAX_ADC_TOUCH / 2) && (adcY < MAX_ADC_TOUCH / 2)) {    // Left top dot (DOT 1)
            pointX[LEFT_TOP_1] = adcX;
            pointY[LEFT_TOP_1] = adcY;
            MeasureFlags |= (1 << LEFT_TOP_1);
            set_point(POINT1_X, POINT1_Y);
        } else if ((adcX > MAX_ADC_TOUCH / 2) && (adcY < MAX_ADC_TOUCH / 2)) {    // Right top dot (DOT 2)
            pointX[RIGHT_TOP_2] = adcX;
            pointY[RIGHT_TOP_2] = adcY;
            MeasureFlags |= (1 << RIGHT_TOP_2);
            set_point(POINT2_X, POINT2_Y);
        } else if ((adcX > MAX_ADC_TOUCH / 2) && (adcY > MAX_ADC_TOUCH / 2)) {    // Right down dot (DOT 3)
            pointX[RIGHT_DOWN_3] = adcX;
            pointY[RIGHT_DOWN_3] = adcY;
            MeasureFlags |= (1 << RIGHT_DOWN_3);
            set_point(POINT3_X, POINT3_Y);
        } else if ((adcX < MAX_ADC_TOUCH / 2) && (adcY > MAX_ADC_TOUCH / 2)) {    //Left down dot (DOT 4)
            pointX[LEFT_DOWN_4] = adcX;
            pointY[LEFT_DOWN_4] = adcY;
            MeasureFlags |= (1 << LEFT_DOWN_4);
            set_point(POINT4_X, POINT4_Y);
        }
        while (get_adc_xy(&adcX, &adcY) == true) {};
        touch_delay_ms(1000);
    }
    //Averaging the value of the X coordinate over points 2 and 3
    pointX[RIGHT_DOWN_3] = (pointX[RIGHT_DOWN_3] + pointX[RIGHT_TOP_2]) / 2;
    // Averaging the X coordinate over points 1 and 4
    pointX[LEFT_TOP_1] = (pointX[LEFT_TOP_1] + pointX[LEFT_DOWN_4]) / 2;
    // Averaging Y coordinate values at points 3 and 4
    pointY[RIGHT_DOWN_3] = (pointY[RIGHT_DOWN_3] + pointY[LEFT_DOWN_4]) / 2;
    // Averaging the value of the Y coordinate over points 1 and 2
    pointY[LEFT_TOP_1] = (pointY[LEFT_TOP_1] + pointY[RIGHT_TOP_2]) / 2;
    // X-axis proportional factor calculation
    *_kX = (float)(pointX[RIGHT_DOWN_3] - pointX[LEFT_TOP_1]) / (POINT3_X - POINT1_X);
    // Y-axis proportional factor calculation
    *_kY = (float)(pointY[RIGHT_DOWN_3] - pointY[LEFT_TOP_1]) / (POINT3_Y - POINT1_Y);
    // Calculation of the displacement coefficient along the X axis
    *_offsetX = pointX[LEFT_TOP_1] / *_kX - POINT1_X;
    // Calculation of the displacement coefficient along the Y axis
    *_offsetY = pointY[LEFT_TOP_1] / *_kY - POINT1_Y;

    calibr_stop();
}

/**
 *  Forcing сalibration coefficientі
 *  @_kX - proportional coefficient X axis
 *  @_kY - proportional coefficient Y axis
 *  @_offsetX - X origin offset
 *  @_offsetY - Y origin offset
 */
void set_touch_calibr(uint8_t _kX, uint8_t _kY, uint8_t _offsetX, uint8_t _offsetY)
{
	kX = _kX;
	kY = _kY;
	offsetX = _offsetX;
	offsetY = _offsetY;
}
