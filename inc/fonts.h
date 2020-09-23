#ifndef TFT_DISPLAY_FONTS_FONTS_H_
#define TFT_DISPLAY_FONTS_FONTS_H_

#include "stdint.h"

#define COURIER_NEW_8_BOLD       0
#define COURIER_NEW_12_BOLD      1
#define COURIER_NEW_8_NORM       2
#define COURIER_NEW_20_NORM      3
#define UBUNTUMONO_14_NORM       4
#define SEVEN_SEGMENT            5

/*
 * Arrays to store images of font characters
 */
extern const uint8_t font8[];
extern const uint8_t font12[];
extern const uint8_t font8_normal[];
extern const uint8_t font20_normal[];
extern const uint8_t font14_ubuntu[];
extern const uint8_t seven_seg_num_font[];

/*
 * Global variables for font settings
 * */
extern const uint8_t	*font_type;		 // Font type
extern uint16_t		font_color;		 // Font symbol color
extern  uint16_t		font_back_color; // Font background color
extern  uint8_t			font_width;		 // Width in pixels
extern  uint8_t			font_height;	 // Height in pixels
extern  uint8_t			font_byte;		 // Amounts of bytes for one sumbol
extern  uint8_t			offset_char;	 // Offset to the beginning of the symbol table


#endif /* TFT_DISPLAY_FONTS_FONTS_H_ */
