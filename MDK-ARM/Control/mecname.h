
#ifndef _MECNAME_
#define _MECNAME_
#include "stm32f4xx_hal.h"



#define RADIAN_COEF 57.3f
/* the radius of wheel(mm) */
#define RADIUS 76
/* the perimeter of wheel(mm) */
#define PERIMETER 478


/* gimbal is relative to chassis center x axis offset(mm) */
#define ROTATE_X_OFFSET 0
/* gimbal is relative to chassis center y axis offset(mm) */
#define ROTATE_Y_OFFSET 0

/* chassis motor use 3508 */
/* the deceleration ratio of chassis motor */
#define MOTOR_DECELE_RATIO (187.0f / 3591.0f)
/* single 3508 motor maximum speed, unit is rpm */
#define MAX_WHEEL_RPM 9000 //8347rpm = 3500mm/s


#define MOTOR_ENCODER_ACCURACY 8192.0f


	

void mecname_calc(void);


extern float wheel_rpm[4];
#endif

