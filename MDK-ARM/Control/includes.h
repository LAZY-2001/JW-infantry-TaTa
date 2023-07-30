#ifndef _INCLUDES_
#define _INCLUDES_
//#define INFANTRY_MIKEY
//#define INFANTRY_RAPH
//#define INFANTRY_LEO
#define INFANTRY_TaTa
#define INFANTRY

#define S_ACC  //S型曲线加速

#define RC_SHOOT_ONE
#ifndef RC_SHOOT_ONE
	#define RC_SHOOT_CONTINUOUS
#endif

#ifdef INFANTRY
 #define  REDUCTION_RATIO   36.0f    //减速比
 #define    BLOCK_ANGLE    30        //堵塞处理转角
 #define  ONE_BULLET_ANGLE  45
 /* wheel track distance(mm) */
 #define WHEELTRACK 400        //轮距
 /* wheelbase distance(mm) */
 #define WHEELBASE 380       //轴距
  #define FOLLOW_DERICTION 1

	#ifdef INFANTRY_LEO
		#define PITCH_UP_LIMIT    ((740+4191)/8192.0*360.0f) //度
		#define PITCH_DOWN_LIMIT    ((7400-4000)/8192.0*360.0f) //度
		#define PITCH_MID_ANGLE    ((140+4191)/8192.0*360.0f) //度
		#define CHASSIS_FOLLOW_CENTER   7490
	#endif
	
	#ifdef INFANTRY_TaTa
		#define PITCH_UP_LIMIT    ((640+3091)/8192.0*360.0f) //度
		#define PITCH_DOWN_LIMIT    ((6300-3600)/8192.0*360.0f) //度
		#define PITCH_MID_ANGLE    ((130+3191)/8192.0*360.0f) //度
		#define CHASSIS_FOLLOW_CENTER   5445
	#endif
	
	#ifdef INFANTRY_RAPH
		#define PITCH_UP_LIMIT    (3200/8192.0*360.0f) //度
		#define PITCH_DOWN_LIMIT    (1918/8192.0*360.0f) //度
		#define PITCH_MID_ANGLE    (2700/8192.0*360.0f) //度
		#define CHASSIS_FOLLOW_CENTER   4090
	#endif
	
	#ifdef INFANTRY_MIKEY
		#define PITCH_UP_LIMIT    ((400 -4000+8191)/8192.0*360.0f) //度
		#define PITCH_DOWN_LIMIT    ((7530 -4000)/8192.0*360.0f) //度
		#define PITCH_MID_ANGLE    ((8090 -4000)/8192.0*360.0f) //度
		#define CHASSIS_FOLLOW_CENTER   1664
	#endif
	
#endif 

#define pwm_init() {                           \
	HAL_TIM_Base_Start_IT(&htim1);               \
	HAL_TIM_Base_Start_IT(&htim6);  						 \
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);     \
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);     \
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);     \
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);     \
	HAL_TIM_Base_Start(&htim4);                  \
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);    \
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);    \
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);    \
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);	   \
	HAL_TIM_Base_Start(&htim5);                  \
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);    \
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);    \
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);    \
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);	   \
}

#define all_init() {      \
	pwm_init();             \
	pid_init();             \
	pc_uart_init();         \
	dbus_uart_init();       \
	can_filter_config();    \
	mpu_device_init();      \
	init_quaternion();	    \
	robot_state_init();     \
	referee_control_startupInit();  \
}

#include "bsp_uart.h"
#include "bsp_can.h"
#include "pid.h"
#include "robot_state.h"
#include "mecname.h"
#include "function_task.h"
#include "chassis_task.h"
#include "bsp_imu.h"
#include "gimbal_task.h" 
#include "control_task.h"
#include "shoot_task.h"
#include "dev_timer.h"
#include "visual_task.h"
#include "string.h"
#include "stdint.h"
#include "ring_math.h"
#include "arm_math.h"
#include "pc_comm_protocol.h"
#include "crc8_crc16.h"
#include "referee_protocol_v14.h"
#include "referee_control.h"

extern uint8_t newdata;
extern float aimx;
extern float aimy;
extern float aimz;
extern float yawtarget;
extern float pittarget;
extern float auto_shoot;
extern uint8_t autofire;
extern uint8_t test;
extern int32_t aim;
#endif

