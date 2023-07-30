/**
  ******************************************************************************
  * @file    pid.h
  * @author  cao yue 
  * @version V1.0.0
  * @date    2020/11/6
  * @brief    
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef __pid_H
#define __pid_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#define ABS(x)		((x>0)? (x): (-x)) 

typedef struct
{
	float kp;
	float ki;
	float kd;
	float errIntegralLimit;
	float ioutLimit;	
	float outputLimit;

	float err;
	float errLast;
	float errIntegral;

	float pout;			
	float iout;							
	float dout;							
	float pos_out;		
}pid_t;

void PID_struct_init(pid_t* pid, float outputLimit, float ioutLimit,\
						 float kp, float ki, float kd);
float pid_calc(pid_t* pid, float get, float set);
void pid_init(void);


extern pid_t chassis_LF_pid;   
extern pid_t chassis_RF_pid; 
extern pid_t chassis_LB_pid; 
extern pid_t chassis_RB_pid; 		
extern pid_t gimbal_yaw_ppid; 
extern pid_t gimbal_yaw_spid;
extern pid_t gimbal_pitchdown_ppid; 
extern pid_t gimbal_pitch_spid;
extern pid_t ammo_booster_ppid;
extern pid_t ammo_booster_spid;
extern pid_t fric_left_pid;
extern pid_t fric_right_pid; 	
extern pid_t imu_pwm_pid;		
extern pid_t chassis_follow_pid;		
extern pid_t ammo_freq_pid;
extern pid_t gate_moto_ppid;
extern pid_t gate_moto_spid;
extern pid_t yaw_aim_pid;
extern pid_t pitch_aim_pid;
		
#endif

