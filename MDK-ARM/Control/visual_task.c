#include "visual_task.h"
#include "crc8_crc16.h"
#include "math.h"
#include "armor_predictor.h"
#include "armor_sight.h"
#include "includes.h"

uint32_t de_time=0;
uint32_t time_plus;
#define pi 3.14159265f
#define E  2.71828f

float aimx=0;
float aimy=0;
float aimz=0;
float yawtarget = 0;
float pittarget = 0;
float auto_shoot = 0;
PCSendPacket_t mcu_data;
uint8_t mcu_send_data[sizeof(PCSendPacket_t)];
ArmorSightOutput_t AS_output;

/*
函数名称：send_mcu_data
函数作用：装填视觉结构体并发送
函数介绍：见函数内部注释
函数使用：可任意调用，需要按照视觉组要求装填数据     （现放在send_pluse函数中等待发送）
*/
void send_mcu_data()
{
	memset(&mcu_data, 0, sizeof(PCSendPacket_t));
	mcu_data.header = 0x5A;
	
	if (game_robot_status.robot_id <100)     //根据装甲板id设置敌人颜色
		mcu_data.enemy_color =1;
	else if (game_robot_status .robot_id >100)
		mcu_data.enemy_color =0;
	
	if (robot.chassis_gimbal ==chassis_lock)
		mcu_data.chassis_status =1;
	else
		mcu_data.chassis_status =0;
	
	if (robot.aiming_state ==armor_aim || robot.aiming_state ==no_aim)
		mcu_data.mode =2;
	else if (robot.aiming_state ==bbuff_aim)
		mcu_data.mode =1;
	else if (robot.aiming_state ==sbuff_aim)
		mcu_data.mode =0;
	
	mcu_data.curr_yaw =imu.yaw /360*2*pi; //陀螺仪角度
	mcu_data.curr_pitch =(gimbal_pitch.angle/8192.0f*2*pi - PITCH_MID_ANGLE/360*2*pi);

	Append_CRC16_Check_Sum((uint8_t*)&mcu_data, sizeof(PCSendPacket_t));
	memcpy(mcu_send_data, &mcu_data, sizeof(mcu_data));

	HAL_UART_Transmit_DMA(&VISUAL_HUART,(uint8_t *)mcu_send_data, sizeof(mcu_data));  //DMA发送函数
}

/*
函数名称：send_pluse
函数作用：外触发视觉相机采集图片并发送数据
函数介绍：设置了一个周期为16ms，开通时间为2ms的脉冲用于触发视觉相机，同时在触发相机后的7ms调用send_mcu_data函数发送视觉结构体
    （相机触发7ms后再发送是考虑到曝光时间，这里也可以不需要考虑曝光时间，也可以按实验结果进行调试）
函数使用：可任意调用   （现在control_task函数中调用）
*/
void send_pluse()
{
	//这里用了很多非阻塞性延时，可以先学习一下dev_timer.c这个文件
	static DEV_TIMER_t timer9;

	DEV_TIMER_StartTimerOne(&timer9, 5); 
	if (DEV_TIMER_TimeOut(&timer9))
	{
//		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);
//		time_plus=uwTick;		
		send_mcu_data();
	}

}
/*
函数名称：predict_task
函数作用：处理视觉接收结构体
函数介绍：按视觉要求处理视觉接受结构体数据，执行预测任务，不多介绍了
函数使用：可任意调用  （现在gimbal_task函数中调用）
    GMY_target_angle_pc,GMP_target_angle_pc,GMP_target_angle_err是输出;
    GMY_target_angle_pc为yaw轴目标角度  GMP_target_angle_pc为pitch轴目标角度    GMP_target_angle_err为pitch轴运动限幅（消抖，现在不用了）
*/
void predict_task()
{
	ArmorPredictorInput_t AP_input;
	ArmorPredictorOutput_t AP_output;

	AP_input.ammo_speed = game_robot_status.shooter_id1_17mm_speed_limit;
	//读取新数据
	if(newdata == 1 )
	{
		ArmorPredictorSetInput(&armor_predictor, &AP_input, &pc_armor);
		newdata = 0;
	}
	else  //没有新数据就靠之前的位置速度信息进行预测
	{
		ArmorPredictorNowStateUpdate(&armor_predictor);
	}
	ArmorPredictorLoop(&armor_predictor);
	ArmorPredictorGetOutput(&armor_predictor, &AP_output);
		
	ArmorSightInput_t AS_input;

	AS_input.yaw = imu.yaw/360*2*pi;
	AS_input.pit = (gimbal_pitch.angle/8192.0f*2*pi - PITCH_MID_ANGLE/360*2*pi);
	AS_input.ammo_speed = game_robot_status.shooter_id1_17mm_speed_limit;
	AS_input.traj_pit_offset =0;
	AS_input.traj_yaw_offset =0;
	ArmorSightSetInput(&armor_sight, &AS_input, &AP_output, pc_armor.yaw);
	ArmorSightLoop(&armor_sight);
	ArmorSightGetOutput(&armor_sight, &AS_output);

	aimx = AS_output.aim_armor.x;
	aimy = AS_output.aim_armor.y;
	aimz = AS_output.aim_armor.z;
	yawtarget = AS_output.yaw_target;
	pittarget = AS_output.pit_target;
	auto_shoot = AS_output.aim_shoot;
}
