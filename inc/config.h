#pragma once

/**
 * This header provides configuration defaults for ili9325 lib.
 *
 * The preferred way of configuring is to override current default values with user ones
 * in separate config file (or using compilation-time -DDEFINES) instead of directly modifying
 * this file.
 *
 */

/*Make library code smaller at the cost of performance penalty */
#define _USE_SIZE_OPTIMIZATIONS		0

#if !defined(ILI9325_USE_SIZE_OPTIMIZATIONS)
#define ILI9325_USE_SIZE_OPTIMIZATIONS	(_USE_SIZE_OPTIMIZATIONS)
#endif

//#define HAL

#define _LIBOPENCM3   0
#define _LIBHAL       1

#if !defined(HAL)
    #define MCU_LIB (_LIBOPENCM3)
#else   
    #define MCU_LIB (_LIBHAL) 
#endif



