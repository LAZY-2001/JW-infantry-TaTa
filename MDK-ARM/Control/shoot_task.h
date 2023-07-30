
#ifndef _SHOOT
#define _SHOOT

#include "stm32f4xx_hal.h"
extern float real_angle;
extern float target_angle;
void shoot_task(void);
void ammo_angle_process(void);

extern int16_t FRIC_SPEED;
#endif 

