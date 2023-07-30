#include "includes.h"
#include "tim.h"
#include <string.h>
#include "math.h"

rc_state_e rc_state=rc_off;
rc_state_e last_rc_state=rc_off;
DEV_TIMER_t rc_state_check_timer;    //遥控器关闭计时器
DEV_TIMER_t powerctrl_timer;         //功率控制板发送频率推荐10Hz，所以需要计时器  
float gate_real_angle=0;
float gate_target_angle=0;
uint8_t calibrate_flag = 1;
extern uint32_t tick_record;
extern int16_t source;

void gate_angle_process()
{
	if(gate_moto.angle <=gate_moto.last_angle )
		{
			if((gate_moto.last_angle -gate_moto.angle) >4000)//编码器上溢
				gate_real_angle =gate_real_angle +(gate_moto.angle +8192 -gate_moto.last_angle) *360 /8192.0 / 36;
			else//正常
				gate_real_angle =gate_real_angle -(gate_moto.last_angle- gate_moto.angle) *360 /8192.0 / 36;
		}
		else
		{
			if((gate_moto.last_angle -gate_moto.angle) <-4000)//编码器下溢
				gate_real_angle = gate_real_angle -(gate_moto.last_angle +8192 -gate_moto.angle) *360 / 8192.0 / 36;
			else//正常
				gate_real_angle = gate_real_angle +(gate_moto.angle -gate_moto.last_angle) *360 /8192.0 / 36;
		}
}

void gate_angle_calibrate()
{ 
  if(gate_moto.given_current >6000 && abs(gate_target_angle -gate_real_angle) <20)//打开弹仓门防止堵转
  {
   gate_target_angle = 145;
   gate_real_angle   = gate_target_angle;
  }
  if (gate_moto.given_current <-6000 && abs(gate_target_angle -gate_real_angle) <20)//关闭弹仓门防止堵转
  {
   gate_target_angle = 0;
   gate_real_angle   = gate_target_angle;
  }
	calibrate_flag = 0;
}
 
void infantry_gate_switch() //步兵弹仓门
{
 static DEV_TIMER_t timer_gate;
 DEV_TIMER_StartTimerOne(&timer_gate, 1000); //每隔10秒校准一次
	
 if (DEV_TIMER_TimeOut(&timer_gate))
  calibrate_flag = 1;

 if(calibrate_flag == 1)
		gate_angle_calibrate(); 
 else
 {
  if (robot.gate_state ==gate_on)
  {
    if (gate_target_angle < 140)
     gate_target_angle++;
    else if(gate_target_angle > 140)
     gate_target_angle--;  
  }
  else
  {
    if (gate_target_angle > 3)
     gate_target_angle--;
    else if (gate_target_angle < 3)
     gate_target_angle++;
  }
 }
 pid_calc(&gate_moto_ppid, gate_real_angle, gate_target_angle);
 pid_calc(&gate_moto_spid, gate_moto.speed_rpm *6/36, gate_moto_ppid.pos_out);
}

void rc_state_check()  //遥控器状态检查
{
	last_rc_state =rc_state;
	if(rc_state ==rc_on)
	{
		DEV_TIMER_StartTimerOne(&rc_state_check_timer, 100);//遥控器开启时进行计时，串口接收中断时立刻重新开始计时
		if (DEV_TIMER_TimeOut(&rc_state_check_timer))
			rc_state =rc_off;  //超时未收到遥控器数据，判断为遥控器关闭
	}
}

static void rc_off_handle()  //遥控器关闭处理
{
	gimbal_init();
	memset(&rc,0,22);    //清空存储的遥控器数据
	robot_state_init();
}

void control()
{
	static uint8_t  data1[8],data2[8],data3[8],data4[8];
	
	if (rc_state==rc_off && last_rc_state==rc_on)
		rc_off_handle();

	if(source == 1  && HAL_GetTick() -tick_record >10)
	{
		source = 0;
	}
	referee_control_loop();	
	predict_task();
	gimbal_task();
	infantry_gate_switch();
	chassis_task();
	shoot_task();
	rc_state_check();
	client_send_task();
	
	if (robot.gate_state ==gate_on)
	{
		  __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, 700);
		  __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 700);
		  __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, 700);
		  __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, 700);
		  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, 700);
		  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, 700);
		  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3, 700);
		  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 700);
	}
	else
	{
			__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, 2550);
		  __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 2550);
		  __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, 2550);
		  __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, 2550);
		  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, 2550);
		  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, 2550);
		  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3, 2550);
		  __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 2550);
	}
	
	set_current(data1, chassis_LF_pid.pos_out ,chassis_RF_pid.pos_out ,chassis_RB_pid.pos_out ,chassis_LB_pid.pos_out );	
	set_current(data2, 0 ,gimbal_yaw_spid.pos_out, gate_moto_spid.pos_out ,ammo_booster_spid.pos_out );
	set_current(data3, fric_left_pid.pos_out,fric_right_pid.pos_out, 0, 0);
	set_current(data4, gimbal_pitch_spid.pos_out,0,0,0);
	
	if(rc_state ==rc_off)  //遥控器关闭，底盘不动
	{
		set_current(data1,0,0,0,0);
		set_current(data2,0,0,0,0);
		set_current(data3,0,0,0,0);
		set_current(data4,0,0,0,0);
	}
	
	can_transmit_msg_one(&hcan1,data1);
	can_transmit_msg_two(&hcan1,data2);
	can_transmit_msg_one(&hcan2,data3);
	can_transmit_msg_two(&hcan2,data4);
}

void control_task()
{
	static uint8_t gimbal_init_flag=1;

	if (imu.yaw !=NULL && HAL_GetTick() >3000 )
	{
		if (gimbal_init_flag ==1)
		{
			gimbal_init();
			gimbal_init_flag =0;
		}
		control();
		send_pluse();
	}
	else
	{
		robot_state_init();
		gimbal_init();
	}
	if (HAL_GetTick() -alive_timer >5000)//防止拨弹盘复活后突然转动
		target_angle =real_angle;  //这里的两个变量均与拨弹盘相关
}
