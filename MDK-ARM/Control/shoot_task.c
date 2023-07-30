#include "includes.h"
#include "math.h"

int16_t FRIC_SPEED=0;
float target_angle=0;
float real_angle=0;
float this_speed=0;
float ammo_delta_angle=0;
float angle_record=0;
uint8_t block_flag=0;
uint8_t last_block_flag=0;
extern int32_t sp;
extern int16_t source;

void ammo_angle_process()
{
	if(ammo_booster.angle <=ammo_booster.last_angle)
	{
		if((ammo_booster.last_angle -ammo_booster.angle) >4000)//编码器上溢
			real_angle =real_angle +(ammo_booster.angle +8192 -ammo_booster.last_angle)* 360.0f /8192.0f /REDUCTION_RATIO;
		else//正常
			real_angle =real_angle  -(ammo_booster.last_angle -ammo_booster.angle) *360.0f /8192.0f /REDUCTION_RATIO;
	}
	else
	{
		if((ammo_booster.last_angle -ammo_booster.angle) <-4000)//编码器下溢
			real_angle =real_angle -(ammo_booster.last_angle +8192 -ammo_booster.angle) *360.0f /8192.0f /REDUCTION_RATIO;
		else//正常
			real_angle =real_angle +(ammo_booster.angle -ammo_booster.last_angle) *360.0f /8192.0f /REDUCTION_RATIO;
	}
}

void fric_ctrl()
{
	FRIC_SPEED=5100;
	#ifdef INFANTRY_MIKEY
		if(game_robot_status.shooter_id1_17mm_speed_limit==15)
			FRIC_SPEED=4870;
		else if (game_robot_status.shooter_id1_17mm_speed_limit==18)
			FRIC_SPEED=5350;
		else if (game_robot_status.shooter_id1_17mm_speed_limit==30)
			FRIC_SPEED=7410;
	#endif
	#ifdef INFANTRY_RAPH
		if(game_robot_status.shooter_id1_17mm_speed_limit==15)
			FRIC_SPEED=4800;
		else if (game_robot_status.shooter_id1_17mm_speed_limit==18)
			FRIC_SPEED=5250;
		else if (game_robot_status.shooter_id1_17mm_speed_limit==30)
			FRIC_SPEED=7200;
	#endif
	#ifdef INFANTRY_LEO
		if(game_robot_status.shooter_id1_17mm_speed_limit==15)
			FRIC_SPEED=4700;
		else if (game_robot_status.shooter_id1_17mm_speed_limit==18)
			FRIC_SPEED=5100;
		else if (game_robot_status.shooter_id1_17mm_speed_limit==30)
			FRIC_SPEED=7100;
	#endif
	#ifdef INFANTRY_TaTa
	if(game_robot_status.shooter_id1_17mm_speed_limit==15)
			FRIC_SPEED=4500+sp*50;
		else if (game_robot_status.shooter_id1_17mm_speed_limit==18)
			FRIC_SPEED=4970+sp*50;
		else if (game_robot_status.shooter_id1_17mm_speed_limit==30)
			FRIC_SPEED=7700+sp*50;
	#endif
	
	if (robot.fric_state ==fric_on)
	{
		pid_calc(&fric_left_pid, fric_left.speed_rpm, FRIC_SPEED );
		pid_calc(&fric_right_pid, fric_right.speed_rpm, -FRIC_SPEED );
	}
	else
	{
		pid_calc(&fric_left_pid, fric_left.speed_rpm, 0 );
		pid_calc(&fric_right_pid, fric_right.speed_rpm, 0);
	}
}

float booster_current_cnt =0;
void block_check()  //卡弹检测
{
	static uint16_t time_cnt=0;
	static uint16_t block_cnt=0;
	static float block_time=0;
	//根据经验得出的卡弹检测逻辑
	booster_current_cnt =ammo_booster.given_current /(this_speed-10);
	time_cnt++;
	if (booster_current_cnt >900)
		block_cnt++;
	
	block_time =(float)block_cnt /(float)time_cnt;
	if (time_cnt ==45)
	{
		time_cnt =5;
		block_cnt =0;
	}
	if (block_time >0.76f)
		block_flag =1;	
}

void block_handle()  //卡弹处理
{
	static DEV_TIMER_t block_timer;
	this_speed =ammo_booster.speed_rpm *6.0f /REDUCTION_RATIO;
  
	if (block_flag ==1 && last_block_flag ==0)
	{
		real_angle =0;
		target_angle =0;
		ammo_delta_angle =0;
		angle_record =0;
		target_angle =real_angle +BLOCK_ANGLE;
		last_block_flag =block_flag;
		DEV_TIMER_StartTimerOne(&block_timer, 300);  //卡弹处理时间400ms
	}
	pid_calc(&ammo_booster_ppid, real_angle, target_angle);
	pid_calc(&ammo_booster_spid, this_speed, ammo_booster_ppid.pos_out );
	
	if (DEV_TIMER_TimeOut(&block_timer))  //卡弹处理时间结束
	{
		block_flag= 0;	
		real_angle= 0;
		target_angle= 0;
		ammo_delta_angle= 0;
		angle_record= 0;
		last_block_flag= 0;
	}
}

void shoot()  //射击任务
{
	this_speed =ammo_booster.speed_rpm *6 /REDUCTION_RATIO;
	
	if (last_rc.kb.bit.R ==0 && rc.kb.bit.R ==1) //R键
	{
		real_angle= 0;
		target_angle= 0;
		ammo_delta_angle= 0;
		angle_record= 0;
	}
	
	if (robot.shoot_state ==shoot_one_bullet)  //单发
	{
		if (autofire ==1 && auto_shoot ==1 && test ==1)
		{
			shoot_flag = 1;
			test = 0;
		}
		if (shoot_flag==1 && fabs(real_angle -target_angle) <90 && source == 1)
		{
			target_angle -=ONE_BULLET_ANGLE;
			shoot_flag =0;
		}
	}
	else if (robot.shoot_state ==shoot_continuous)  //连发
	{
		if (shoot_flag ==1)
		{
			if (last_rc.mouse.l ==0)
			{
				ammo_delta_angle =0;
				angle_record =target_angle;
			}
			ammo_delta_angle +=0.5f;
		}
    //单击一次2
		if(last_rc.mouse.l ==1 && rc.mouse.l ==0 && ammo_delta_angle <135)
		{	
		 ammo_delta_angle =135;
		 if (game_robot_status.shooter_id1_17mm_cooling_limit -power_heat_data.shooter_id1_17mm_cooling_heat <=30)
			 ammo_delta_angle =0;
		}
		target_angle =angle_record -ammo_delta_angle;

	}
	if (game_robot_status.shooter_id1_17mm_cooling_limit -power_heat_data.shooter_id1_17mm_cooling_heat <=15)//||block_flag==1) &&GAME_STATE.game_status .game_progress ==4)
	{
		real_angle=0;
		target_angle=0;
		ammo_delta_angle=0;
		angle_record=0;
	}
		pid_calc(&ammo_booster_ppid,real_angle,target_angle);
		pid_calc(&ammo_booster_spid,this_speed,ammo_booster_ppid.pos_out);
}

void shoot_task()
{
	fric_ctrl();
	block_check();
	
	if (block_flag ==0)
		shoot();
	else 
		block_handle();
}
		
