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
�������ƣ�send_mcu_data
�������ã�װ���Ӿ��ṹ�岢����
�������ܣ��������ڲ�ע��
����ʹ�ã���������ã���Ҫ�����Ӿ���Ҫ��װ������     ���ַ���send_pluse�����еȴ����ͣ�
*/
void send_mcu_data()
{
	memset(&mcu_data, 0, sizeof(PCSendPacket_t));
	mcu_data.header = 0x5A;
	
	if (game_robot_status.robot_id <100)     //����װ�װ�id���õ�����ɫ
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
	
	mcu_data.curr_yaw =imu.yaw /360*2*pi; //�����ǽǶ�
	mcu_data.curr_pitch =(gimbal_pitch.angle/8192.0f*2*pi - PITCH_MID_ANGLE/360*2*pi);

	Append_CRC16_Check_Sum((uint8_t*)&mcu_data, sizeof(PCSendPacket_t));
	memcpy(mcu_send_data, &mcu_data, sizeof(mcu_data));

	HAL_UART_Transmit_DMA(&VISUAL_HUART,(uint8_t *)mcu_send_data, sizeof(mcu_data));  //DMA���ͺ���
}

/*
�������ƣ�send_pluse
�������ã��ⴥ���Ӿ�����ɼ�ͼƬ����������
�������ܣ�������һ������Ϊ16ms����ͨʱ��Ϊ2ms���������ڴ����Ӿ������ͬʱ�ڴ���������7ms����send_mcu_data���������Ӿ��ṹ��
    ���������7ms���ٷ����ǿ��ǵ��ع�ʱ�䣬����Ҳ���Բ���Ҫ�����ع�ʱ�䣬Ҳ���԰�ʵ�������е��ԣ�
����ʹ�ã����������   ������control_task�����е��ã�
*/
void send_pluse()
{
	//�������˺ܶ����������ʱ��������ѧϰһ��dev_timer.c����ļ�
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
�������ƣ�predict_task
�������ã������Ӿ����սṹ��
�������ܣ����Ӿ�Ҫ�����Ӿ����ܽṹ�����ݣ�ִ��Ԥ�����񣬲��������
����ʹ�ã����������  ������gimbal_task�����е��ã�
    GMY_target_angle_pc,GMP_target_angle_pc,GMP_target_angle_err�����;
    GMY_target_angle_pcΪyaw��Ŀ��Ƕ�  GMP_target_angle_pcΪpitch��Ŀ��Ƕ�    GMP_target_angle_errΪpitch���˶��޷������������ڲ����ˣ�
*/
void predict_task()
{
	ArmorPredictorInput_t AP_input;
	ArmorPredictorOutput_t AP_output;

	AP_input.ammo_speed = game_robot_status.shooter_id1_17mm_speed_limit;
	//��ȡ������
	if(newdata == 1 )
	{
		ArmorPredictorSetInput(&armor_predictor, &AP_input, &pc_armor);
		newdata = 0;
	}
	else  //û�������ݾͿ�֮ǰ��λ���ٶ���Ϣ����Ԥ��
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
