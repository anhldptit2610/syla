#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define SYSTEM_CLOCK_RATE       2000000

/* macros */
#define LSB(n)                  (((uint16_t)n >> 0) & 0x00ff)
#define MSB(n)                  (((uint16_t)n >> 8) & 0x00ff)
#define TO_U16(lsb, msb)        (((uint16_t)msb << 8) | ((uint16_t)lsb))
#define BIT(f, i)               (((f) & (1U << i)) >> i)


#ifdef __cplusplus
}
#endif