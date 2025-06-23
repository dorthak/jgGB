#pragma once

#include <cstdint>
#include <stdbool.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <array>


#define DEBUG_FILE_PRINT
//#define DEBUG_SCREEN_PRINT
//#define DEBUG_TIMING

#define LIMIT_CYCLE_M
//#define LIMIT_CYCLE_T


#define BIT(a, n) ((a & (1 << n)) ? 1 : 0)
#define BIT_SET(a, n, on)  (on ? (a) |= (1 << n) : (a) &= ~(1 << n))
#define BETWEEN(a, b, c) ((a >= b) && (a <= c))



#define NO_IMPL { fprintf(stderr, "NOT YET IMPLEMENTED\n"); exit(-5); };

