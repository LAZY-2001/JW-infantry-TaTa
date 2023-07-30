#include "includes.h"
#include "math.h"
#include "string.h"
#include "math.h"
#include "armor_predictor.h"
#include "armor_sight.h"
#define pi 3.14159265f
#define MOUSE_VEL_LIMIT(vel, min, max) \
  do                                 \
  {                                  \
    if ((vel) <= (min))              \
      (vel) = (min);                 \
    else if ((vel) >= (max))         \
      (vel) = (max);                 \
  } while (0)
//extern float a;

void gimbal_init()
{
	//GMP_target_angle =0;
	GMP_target_angle =PITCH_MID_ANGLE;
	GMY_target_angle =imu.yaw;
  ArmorPredictorInit(&armor_predictor, 0.05f);//0.08
  ArmorSightInit(&armor_sight, 0.01f, 0.02f);
}

float GMY_target_angle=0 ;
float GMP_target_angle=0;
float pitch_mid_angle = PITCH_MID_ANGLE;
float yaw_angle =0;
float yaw_lock_middle_angle =0;
float yawcontrol = 0;
float pitchv = 0;

void gimbal_task()
{
	//打开底盘锁定瞬间，当前编码器角度计为yaw_lock_middle_angle
	if (robot.chassis_gimbal ==chassis_lock && last_robot.chassis_gimbal !=chassis_lock)
	{
		yaw_lock_middle_angle =gimbal_yaw.angle;
		GMY_target_angle =0;
	}
	else if (last_robot.chassis_gimbal ==chassis_lock && robot.chassis_gimbal != chassis_lock)
	{
		GMY_target_angle =imu.yaw; 
	}
	//底盘锁定模式下，yaw_angle定义为编码器读数与yaw_lock_middle_angle的差
	//yaw位置控制变为相对于yaw_lock_middle_angle的变化角度）
	if (robot.chassis_gimbal ==chassis_lock)
	{
		yaw_angle =(gimbal_yaw.angle -yaw_lock_middle_angle)/8192.0f *360;  //以右转为正方向(left)
	}
	else
	{
		yaw_angle=imu.yaw; //非底盘锁定模式，使用imu数据
	}
	
	//鼠标控制消抖
	if (fabs(rc.mouse.x) > 5 *fabs(rc.mouse.y))
		rc.mouse.y =0;
	if (fabs(rc.mouse.y) > 5 *fabs(rc.mouse.x))
		rc.mouse.x =0;
		
	if (robot.control_state ==RC)
	{
		GMY_target_angle = RingConvertToRF(360,GMY_target_angle - 0.6*rc.ch1/660.0f);
		GMP_target_angle -= 0.6 *rc.ch2 /660.0f;
	}
	else if(robot.control_state ==KB)
	{
		MOUSE_VEL_LIMIT(rc.mouse.x, -100, 100);  //鼠标控制限幅
		MOUSE_VEL_LIMIT(rc.mouse.y, -100, 100);
		GMY_target_angle =RingConvertToRF(360, GMY_target_angle -rc.mouse.x/200.0f);
		GMP_target_angle +=rc.mouse.y /200.0f;
	}
	
	if (robot.aiming_state ==armor_aim && pc_armor.tracking ==1 && visual_timer > (uwTick -1000) && fabs(rc.mouse.x) <5 && fabs(rc.mouse.y) <5)
	{
		GMY_target_angle =yawtarget /2/pi*360;
		GMP_target_angle =PITCH_MID_ANGLE +pittarget/2/pi*360;
	}
	else if ((robot.aiming_state ==sbuff_aim || robot.aiming_state ==bbuff_aim) && visual_timer > (uwTick -1000) && fabs(rc.mouse.x) <5 && fabs(rc.mouse.y) <5)
	{
		GMY_target_angle =pc_energy.energy_yaw;
		GMP_target_angle =PITCH_MID_ANGLE -pc_energy.energy_pitch;
	}

	if (GMP_target_angle >PITCH_UP_LIMIT)
		GMP_target_angle =PITCH_UP_LIMIT;
	else if (GMP_target_angle <PITCH_DOWN_LIMIT)
		GMP_target_angle =PITCH_DOWN_LIMIT;
	
	yawcontrol = RingCloseSubRF(360, yaw_angle, GMY_target_angle);
	pid_calc(&gimbal_yaw_ppid,  yawcontrol, 0 );	
	pid_calc(&gimbal_yaw_spid, imu.wz, gimbal_yaw_ppid.pos_out);  

	pid_calc(&gimbal_pitchdown_ppid, gimbal_pitch.angle/8192.0f*360, GMP_target_angle);
	pid_calc(&gimbal_pitch_spid, -imu.wy, gimbal_pitchdown_ppid.pos_out);
}
