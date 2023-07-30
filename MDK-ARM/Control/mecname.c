#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "includes.h"

#define MEC_VAL_LIMIT(val, min, max) \
  do                                 \
  {                                  \
    if ((val) <= (min))              \
    {                                \
      (val) = (min);                 \
    }                                \
    else if ((val) >= (max))         \
    {                                \
      (val) = (max);                 \
    }                                \
  } while (0)
	
float wheel_rpm[4];

void mecname_calc()
{
  float max = 0;	
	
	static float rotate_ratio_fr;
  static float rotate_ratio_fl;
  static float rotate_ratio_bl;
  static float rotate_ratio_br;
  static float wheel_rpm_ratio;

  rotate_ratio_fr = ((WHEELTRACK + WHEELBASE) / 2.0f - ROTATE_X_OFFSET + ROTATE_Y_OFFSET) / RADIAN_COEF;
  rotate_ratio_fl = ((WHEELTRACK + WHEELBASE) / 2.0f - ROTATE_X_OFFSET - ROTATE_Y_OFFSET) / RADIAN_COEF;
  rotate_ratio_bl = ((WHEELTRACK + WHEELBASE) / 2.0f + ROTATE_X_OFFSET - ROTATE_Y_OFFSET) / RADIAN_COEF;
  rotate_ratio_br = ((WHEELTRACK + WHEELBASE) / 2.0f + ROTATE_X_OFFSET + ROTATE_Y_OFFSET) / RADIAN_COEF;

  wheel_rpm_ratio = 60.0f / (PERIMETER * MOTOR_DECELE_RATIO);

  MEC_VAL_LIMIT(mecname.Velocity_X, -4000, 4000); //mm/s
  MEC_VAL_LIMIT(mecname.Velocity_Y, -4000, 4000); //mm/s
  MEC_VAL_LIMIT(mecname.Palstance , -360, 360); //deg/s
	
//后轮驱动
//	wheel_rpm[2] = -2 *(-mecname.Velocity_X + mecname.Velocity_Y - mecname.Palstance * rotate_ratio_br) * wheel_rpm_ratio;	
//  wheel_rpm[3] = -2 *(-mecname.Velocity_X - mecname.Velocity_Y - mecname.Palstance * rotate_ratio_bl) * wheel_rpm_ratio;
//四轮驱动
  wheel_rpm[0] = -(-mecname.Velocity_X - mecname.Velocity_Y - mecname.Palstance * rotate_ratio_fl) * wheel_rpm_ratio;
  wheel_rpm[1] = -(-mecname.Velocity_X + mecname.Velocity_Y - mecname.Palstance * rotate_ratio_fr) * wheel_rpm_ratio;
  wheel_rpm[2] = -(mecname.Velocity_X + mecname.Velocity_Y - mecname.Palstance * rotate_ratio_br) * wheel_rpm_ratio;	
  wheel_rpm[3] = -(mecname.Velocity_X - mecname.Velocity_Y - mecname.Palstance * rotate_ratio_bl) * wheel_rpm_ratio;

  //find max item
  for (uint8_t i = 0; i < 4; i++)
  {
    if (fabs(wheel_rpm[i]) > max)
      max = fabs(wheel_rpm[i]);
  }
  //equal proportion
  if (max > MAX_WHEEL_RPM)
  {
    float rate = MAX_WHEEL_RPM / max;
    for (uint8_t i = 0; i < 4; i++)
      wheel_rpm[i] *= rate;
  }
}


