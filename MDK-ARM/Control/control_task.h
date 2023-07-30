
#ifndef _CONTROL
#define _CONTROL

#include "stm32f4xx_hal.h"
#include "dev_timer.h"

void control_task(void);



typedef enum 
{
	rc_on,        //是否能接收到遥控器的数据（遥控器是否开启）
	rc_off,
}rc_state_e;


extern rc_state_e rc_state;
extern rc_state_e last_rc_state;
extern DEV_TIMER_t rc_state_check_timer;
extern DEV_TIMER_t powerctrl_timer;
extern float gate_real_angle;

void gate_angle_process(void);
#endif

