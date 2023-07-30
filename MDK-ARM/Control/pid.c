/**
  ******************************************************************************
  * @file    pid.c
  * @author  cao yue 
  * @version V1.0.0
  * @date    2020/11/6
  * @brief    
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include <math.h>

pid_t chassis_LF_pid;   
pid_t chassis_RF_pid; 
pid_t chassis_LB_pid; 
pid_t chassis_RB_pid; 		
pid_t gimbal_yaw_ppid; 
pid_t gimbal_yaw_spid; 
pid_t gimbal_pitchdown_ppid; 
pid_t gimbal_pitch_spid;
pid_t ammo_booster_ppid;
pid_t ammo_booster_spid;
pid_t fric_left_pid;
pid_t fric_right_pid; 
pid_t imu_pwm_pid;
pid_t chassis_follow_pid;
pid_t ammo_freq_pid;
pid_t gate_moto_ppid;
pid_t gate_moto_spid;
pid_t yaw_aim_pid;
pid_t pitch_aim_pid;

static void AbsLimit(float *a, float ABS_MAX)
{
    if(*a > ABS_MAX)
		{
      *a = ABS_MAX;
		}
    else if(*a < -ABS_MAX)
		{
      *a = -ABS_MAX;
		}
}

static void PIDRefreshErrIntegralLimit(pid_t* pid)
{
	if(pid->ki != 0)
	{
		pid->errIntegralLimit = pid->ioutLimit / pid->ki;
	}
	else
	{
		pid->errIntegralLimit = 0;
	}
	
}

void PIDReset(pid_t* pid)
{
	pid->err = 0;
	pid->errLast = 0;
	pid->errIntegral = 0;
}

void PID_struct_init(pid_t* pid, float outputLimit, float ioutLimit,\
						 float kp, float ki, float kd)
{
	pid->outputLimit = outputLimit;
	pid->ioutLimit = ioutLimit;
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	
	PIDRefreshErrIntegralLimit(pid);
}

float pid_calc(pid_t* pid, float get, float set)
{
	pid->errLast = pid->err;
	pid->err = set - get;
	pid->errIntegral += pid->err;
	AbsLimit(&(pid->errIntegral), pid->errIntegralLimit);

	pid->pout = pid->kp * pid->err;
	pid->iout = pid->ki * pid->errIntegral;
	pid->dout = pid->kd * (pid->err - pid->errLast);
	AbsLimit(&(pid->iout), pid->ioutLimit);
	
	pid->pos_out = pid->pout + pid->iout + pid->dout;
	AbsLimit(&(pid->pos_out), pid->outputLimit);
	
	return pid->pos_out;	
}

void pid_init()
{
	#ifdef INFANTRY_MIKEY
	const float chassis_Kp=10, chassis_Ki=1, chassis_Kd=0;
	PID_struct_init(&chassis_LF_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_RF_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_LB_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_RB_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	
	//PID_struct_init(&gimbal_yaw_ppid, 10, 100, 0.8, 0, 0.8);//26  0    0.3
	//PID_struct_init(&gimbal_yaw_spid, 30000, 5000, 5600, 5, 15);
  PID_struct_init(&gimbal_yaw_ppid, 10, 100, 0.5, 0, 0.8);
	PID_struct_init(&gimbal_yaw_spid, 30000, 3000, 8000, 5, 2);
	
	PID_struct_init(&gimbal_pitchdown_ppid, 10, 0, 0.8, 0, 0.8);
	PID_struct_init(&gimbal_pitch_spid, 30000, 3000, 6500,	5, 20	);//8500  15

	PID_struct_init(&fric_left_pid, 16384, 5000,12,0,0.2	);
	PID_struct_init(&fric_right_pid, 16384, 5000,12,0,0.2	);
	
	PID_struct_init(&ammo_booster_ppid, 300, 1000, 12,	0,	1);
	PID_struct_init(&ammo_booster_spid, 10000, 3000, 120,	0.8,	0);

	PID_struct_init(&imu_pwm_pid, 2000,500,1100,10,0);
	
	PID_struct_init(&chassis_follow_pid, 200,100, 4, 0, 0.5);
	
	PID_struct_init(&ammo_freq_pid, 300,200,3,0.5,0);   //要调参
	
	PID_struct_init(&gate_moto_ppid, 1080, 1080, 13.0f,	0.0f,	0.0f	);
	PID_struct_init(&gate_moto_spid, 10000, 5000, 65.0f,	0.8f,	0.0f	);
	
	PID_struct_init(&pitch_aim_pid, 2, 0.2, 0.1,	0, 0.1);
	PID_struct_init(&yaw_aim_pid, 10, 2, 1.6,	0.1, 1);
	#endif
	
	#ifdef INFANTRY_TaTa
	const float chassis_Kp=10, chassis_Ki=1, chassis_Kd=0;
	PID_struct_init(&chassis_LF_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_RF_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_LB_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_RB_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	
//	PID_struct_init(&gimbal_yaw_ppid, 10, 100, 0.6, 0.002, 1.3);//26  0    0.3
//	PID_struct_init(&gimbal_yaw_spid, 30000, 5000, 5600, 10, 30);
  PID_struct_init(&gimbal_yaw_ppid, 10, 2, 1, 0.0001, 10);//0.6, 0.01, 8
	PID_struct_init(&gimbal_yaw_spid, 20000, 5000, 6800, 30, 3000);//7000, 100, 2.5
	
	PID_struct_init(&gimbal_pitchdown_ppid, 10, 1, 0.45, 0.008, 0);//0.6, 0.007, 1.0
	PID_struct_init(&gimbal_pitch_spid, 20000, 3000, 8500, 100, 3000);//8500, 15, 4.5 

	PID_struct_init(&fric_left_pid, 16384, 5000,20,0.1,3	);
	PID_struct_init(&fric_right_pid, 16384, 5000,20,0.1,3	);
	
	PID_struct_init(&ammo_booster_ppid, 45*30, 0, 12,	0,	1);
	PID_struct_init(&ammo_booster_spid, 10000, 5000, 100,	0.8, 1);

	
	PID_struct_init(&imu_pwm_pid, 2000,500,1100,10,0);
	
	PID_struct_init(&chassis_follow_pid, 200,100, 4, 0, 0.5);
	
	PID_struct_init(&ammo_freq_pid, 300,200,3,0.5,0);   //要调参
	
	PID_struct_init(&gate_moto_ppid, 1080, 1080,13.0f,	0.0f,	0.0f	);
	PID_struct_init(&gate_moto_spid, 10000, 5000,75.0f,	0.8f,	0.0f	);
	
	PID_struct_init(&pitch_aim_pid, 2, 0.2, 1.0,	0, 0.1);
	PID_struct_init(&yaw_aim_pid, 10, 2, 1.6,	0.1, 1);
	#endif
	
	#ifdef INFANTRY_RAPH
	const float chassis_Kp=10,chassis_Ki=1,chassis_Kd=0;
	PID_struct_init(&chassis_LF_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_RF_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_LB_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_RB_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	
	PID_struct_init(&gimbal_yaw_ppid, 10, 100, 0.4, 0, 0	);
	PID_struct_init(&gimbal_yaw_spid, 30000, 3000, 8000, 0, 0.3	);
	
	PID_struct_init(&gimbal_pitch_ppid, 30, 0,0.7,	0,	1.2);
	PID_struct_init(&gimbal_pitch_spid, 30000, 5000,8000,	3,	0.5	);
		
	PID_struct_init(&fric_left_pid, 16384, 5000,10,0,0.2	);
	PID_struct_init(&fric_right_pid, 16384, 5000,10,0,0.2	);
	
	//PID_struct_init(&ammo_booster_ppid, 1080, 1080,13,	0,	0	
	//PID_struct_init(&ammo_booster_spid, 10000, 5000,80,	0.5f,	0	);
	PID_struct_init(&ammo_booster_ppid, 300, 1080,12,	0,	1	);	
	PID_struct_init(&ammo_booster_spid, 10000, 5000,100,	0.8f,	0	);
	
	PID_struct_init(&imu_pwm_pid, 2000,500,1100,10,0);
	
	PID_struct_init(&chassis_follow_pid, 200,100,6,0,0.5);
	
	PID_struct_init(&ammo_freq_pid, 300,200,3,0.5,0);   //要调参
	
	PID_struct_init(&gate_moto_ppid, 1080, 1080,13.0f,	0.0f,	0.0f	);
	PID_struct_init(&gate_moto_spid, 10000, 5000,75.0f,	0.8f,	0.0f	);
	#endif
	
	#ifdef INFANTRY_LEO
	const float chassis_Kp=2,chassis_Ki=1,chassis_Kd=0;
	PID_struct_init(&chassis_LF_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_RF_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_LB_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	PID_struct_init(&chassis_RB_pid, 16384, 5000,chassis_Kp,	chassis_Ki,	chassis_Kd	);
	
	PID_struct_init(&gimbal_yaw_ppid, 10, 100,0.4,	0,	0	);
	PID_struct_init(&gimbal_yaw_spid, 30000, 5000,8000,5,0.5	);
	
	PID_struct_init(&gimbal_pitch_ppid, 10, 0,0.6,	0,	0.3);
	PID_struct_init(&gimbal_pitch_spid, 30000, 5000,8000,	3,	1	);
	
	PID_struct_init(&fric_left_pid, 16384, 5000,10,0,0.2	);
	PID_struct_init(&fric_right_pid, 16384, 5000,10,0,0.2	);
	
	PID_struct_init(&ammo_booster_ppid, 1080, 1080,13,	0,	0	);
	PID_struct_init(&ammo_booster_spid, 10000, 5000,80,	0.5f,	0	);

	
	PID_struct_init(&imu_pwm_pid, 2000,500,1100,10,0);
	
	PID_struct_init(&chassis_follow_pid, 200,0,6,0,0.5);
	
	PID_struct_init(&ammo_freq_pid, 300,200,3,0.5,0);   //要调参
	
	PID_struct_init(&gate_moto_ppid, 1080, 1080,13.0f,	0.0f,	0.0f	);
	PID_struct_init(&gate_moto_spid, 10000, 5000,75.0f,	0.8f,	0.0f	);
	
	PID_struct_init(&yaw_aim_pid, 5, 0.5,1.8,	0.08,	0.5);
	PID_struct_init(&pitch_aim_pid, 1, 0.2,0.2,	0.01,	0.1	);
	#endif
}

