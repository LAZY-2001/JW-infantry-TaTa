

#ifndef _CHASSIS
#define _CHASSIS

#include "stm32f4xx_hal.h"




typedef struct {
	float  Velocity_X;
	float  Velocity_Y;
	float  Palstance;
}mecname_t;

void chassis_task(void);
void static_bias_45(void);
extern mecname_t mecname;
extern uint16_t MAX_CHASSIS_VX_SPEED ; //8000rpm
extern uint16_t MAX_CHASSIS_VY_SPEED ;
extern float axes_angle;
#endif


