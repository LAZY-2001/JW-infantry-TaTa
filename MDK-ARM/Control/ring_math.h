#ifndef __RING_MATH_H
#define __RING_MATH_H

#include "stm32f4xx_hal.h"

int32_t RingConvertToRI(int32_t ring, int32_t x);
float RingConvertToRF(float ring, float x);
uint8_t RingAssertRI(int32_t ring, int32_t x);
uint8_t RingAssertRF(float ring, float x);
int32_t RingAddRI(int32_t ring, int32_t x, int32_t y);
float RingAddRF(float ring, float x, float y);
int32_t RingCloseSubRI(int32_t ring, int32_t x, int32_t y);
float RingCloseSubRF(float ring, float x, float y);
int32_t RingValueStepRI(int32_t ring, int32_t value, int32_t dst, int32_t step);
float RingValueStepRF(float ring, float value, float dst, float step);
int32_t RingCloseValueStepRI(int32_t ring, int32_t value, int32_t dst, int32_t abs_step);
float RingCloseValueStepRF(float ring, float value, float dst, float abs_step);

#endif /* __RING_MATH_H */
