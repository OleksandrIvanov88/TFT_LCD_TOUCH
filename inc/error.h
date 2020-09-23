#pragma once

/**
 * ili9325 and tft libs errors - provides definitions for all error codes.
 *
 * User may check corresponding error codes against this definitions.
 * It is not requried to include this file in user code as it is already included
 * everywhere these error definitions are used (unless you want to use same codes for own routines)
 */

enum tft_err {
	/** No error, everything fine */
	TFT_EOK = 0,
	/** Unknown error. Something went wrong but don't boter detecting what */
	TFT_EUNKNOWN = -1,
	/** Some feature is not implemented yet */
	TFT_ENOTIMPLEMENTED = -2,
	/** Something simply could not be done */
	TFT_EUNAVAILABLE = -3,
	/** Wrong argument passed */
	TFT_EWRONGARG = -4,
	/** Not enough arguments */
	TFT_ENENARG = -5,
	/** Range exceeds available limits */
	TFT_ERANGE = -6,
	/** Data storage is full */
	TFT_EFULL = -7,
	/** Data storage is empty */
	TFT_EEMPTY = -8
};


// alias type for more concise definitions
typedef enum tft_err tft_err;


