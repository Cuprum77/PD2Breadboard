#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>

// Preprocessor definitions
#define NUMBER_OF_ANGLES 3600	// decides how many angles to precompute, additional zeroes for more precision
#define ANGLE_SCALE NUMBER_OF_ANGLES / 360	// used to make the angle input more intuitive
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#define FIXED_POINT_SCALE 4096	// 2^12
#define FIXED_POINT_SCALE_BITS 12	// log2(FIXED_POINT_SCALE)
#define FIXED_POINT_SCALE_HIGH_RES 2097152  // 2^21
#define FIXED_POINT_SCALE_HIGH_RES_BITS 21	// log2(FIXED_POINT_SCALE_HIGH_RES)

extern int cosTable[NUMBER_OF_ANGLES];
extern int sinTable[NUMBER_OF_ANGLES];
extern int tanTable[NUMBER_OF_ANGLES];
extern int atanTable[NUMBER_OF_ANGLES];

extern int imin(int x, int y);
extern int imax(int x, int y);
extern int iabs(int x);
extern unsigned int isqrt(unsigned int x);
extern unsigned int ipow(unsigned int x, unsigned int y);
extern unsigned int ifactorial(unsigned int x);

extern void fillLookupTables();
extern void pointOnCircle(unsigned int radius, unsigned int angle, int offsetX, int offsetY, unsigned int* x, unsigned int* y);

extern int lerp(int v0, int v1, int t);

#ifdef __cplusplus
}
#endif