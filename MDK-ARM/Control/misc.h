#ifndef __MISC_H
#define __MISC_H

#include "stm32f4xx_hal.h"

#define MISC_Abs(x) ((x)>0 ? (x) : -(x))
#define MISC_Limit(x, min, max) ((x)<(min)?(min):((x)>(max)?(max):(x)))

#define R2D(rad) ((rad)*57.2957795131f)
#define D2R(deg) ((deg)*0.0174532925f)

float Convert8192To360(int32_t value_8192);
float RPM2DPS(float rpm);
float MISC_ValueStep(float value, float target, float step);

#endif /* __MISC_H */
