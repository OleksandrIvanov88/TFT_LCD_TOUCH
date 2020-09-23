#pragma once
/**
 * li9325 and tft libs macro definitions and functions.
 *
 * Provide some sort of compiler portability and help make our code base more concise
 */

// attributes
#define attr_alwaysinline	    __attribute__((always_inline))
#define attr_pack(N)			__attribute__((packed, aligned(N)))
#define attr_alias(name)		__attribute__((alias(#name)))
#define attr_weak			    __attribute__((weak))
#define attr_weakalias(name)	__attribute__((weak, alias(#name)))

// macrofunctions
#define swap(a, b) do {int16_t t = a; a = b; b = t;} while(0)
#define sk_arr_len(array) (sizeof (array) / sizeof (*(array)))
#define sk_arr_foreach(var, arr) \
    for (long keep = 1, cnt = 0, size = sk_arr_len(arr); \
              keep && (cnt < size); keep = !keep, cnt++) \
        for (typeof(*(arr)) var = (arr)[cnt]; keep; keep = !keep)


