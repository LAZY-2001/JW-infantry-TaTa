/******************************************************************
 *  chassis_task.c��Ҫ������һЩ�����˶��ļ���  ��С����   ���̸���...
 ******************************************************************/
#include "includes.h"
#include "math.h"
uint16_t  acc_time ;   //   ��λms
uint16_t MAX_CHASSIS_VX_SPEED =1000; //8000rpm
uint16_t MAX_CHASSIS_VY_SPEED =2000;
static float Velocity_Y_cnt=0;
static float Velocity_X_cnt=0;
float axes_angle=0;
const float pi=3.14159;
float gimbal_axes_X=0;
float gimbal_axes_Y=0;
float rotate_speed=180;
mecname_t mecname;	

void power_control()
{
	static uint16_t cap_limit_power=0;
	static uint8_t  data[8];
	
	//���ʿ��ư�100ms����һ��
	DEV_TIMER_StartTimerOne(&powerctrl_timer, 100);
	
	if(DEV_TIMER_TimeOut(&powerctrl_timer))
	{
		//���ݵ��̹�������ȷ�����ʿ��ư����������
		//ROBOT_STATUS.game_robot_status.chassis_power_limit =40;
		if(power_heat_data.chassis_power_buffer >40)
			cap_limit_power =(game_robot_status.chassis_power_limit) *100 +2000;
		else
			cap_limit_power =(game_robot_status.chassis_power_limit) *100 -300;

		if(cap_limit_power >13000)
			cap_limit_power =13000;

		set_power(data, cap_limit_power);
		can_transmit_msg_four(&hcan1, data);
	}
	//���ݵ��̹������ƣ�ȷ���ٶ�����
	MAX_CHASSIS_VY_SPEED =34.76f *game_robot_status.chassis_power_limit +50;
	MAX_CHASSIS_VX_SPEED =MAX_CHASSIS_VY_SPEED -200;
	#ifdef INFANTRY_RAPH  //�ĺų����Ӵ�����Щ�����⴦��
		MAX_CHASSIS_VY_SPEED += 120;
		MAX_CHASSIS_VX_SPEED += 120;
	#endif
	rotate_speed =4.22f *game_robot_status.chassis_power_limit -30;
	if (rotate_speed >350)
		rotate_speed =350;

	chassis_follow_pid.outputLimit =180 +game_robot_status.chassis_power_limit;
	acc_time =MAX_CHASSIS_VY_SPEED +300;	//acc_timeԽ�󣬼��ٶ�ԽС

	if(robot.chassis_gimbal ==chassis_rotate)
	{	//С����ģʽ�½����ƶ��ٶ�
		MAX_CHASSIS_VY_SPEED =MAX_CHASSIS_VY_SPEED *0.7f;
		MAX_CHASSIS_VX_SPEED =MAX_CHASSIS_VY_SPEED;
	}
	//����ٶ���������
	if(MAX_CHASSIS_VX_SPEED >3300)
		MAX_CHASSIS_VX_SPEED =3300;
	if(MAX_CHASSIS_VY_SPEED >3300)
		MAX_CHASSIS_VY_SPEED =3300;
	if(rotate_speed >360)
		rotate_speed =360;
	//���ݵ�ѹ�ϵ�ʱ�������ƶ��ٶ�
	static float power_limit_k_target =1, power_limit_k =1;
	power_limit_k_target = 1- (1800 - powerctrl_board.cap_vot) /100.0f/(18 -14);
	
	if (power_limit_k_target >1)
		power_limit_k_target =1;
	else if (power_limit_k_target <0)
		power_limit_k_target =0;
	 
	if (power_limit_k_target -power_limit_k >0.001f)
		power_limit_k +=0.001f;
	else if(power_limit_k_target -power_limit_k <0.001f)
		power_limit_k -=0.001f;
	
	MAX_CHASSIS_VX_SPEED *= power_limit_k;
	MAX_CHASSIS_VY_SPEED *= power_limit_k;
	rotate_speed *= power_limit_k;
	//��shift�����ٶ�����
	if (robot.control_state == KB)
	{	
		if (rc.kb.bit.SHIFT ==1 && rc.kb.bit.CTRL !=1 && powerctrl_board.cap_vot >2000)
		{
			MAX_CHASSIS_VX_SPEED +=700;
			MAX_CHASSIS_VY_SPEED +=700;
			rotate_speed +=60;
		}
	}
}

void forward_back_speed_set()	//ǰ���ƶ����ٲ���
{
	const  uint8_t delta_cnt1=10;  //��Ȼͣ��ʱ�ĵ�λʱ���ٶ�˥����
	const  uint8_t delta_cnt2=8;  //ǰ��������ʱ�ĵ�λʱ���ٶ�����

	#ifdef S_ACC	//S�����߼��ٲ���
	{
		if(rc.kb.bit.W ==1)
			Velocity_Y_cnt +=delta_cnt2;
		else if(rc.kb.bit.S ==1)
			Velocity_Y_cnt -=delta_cnt2;
		else
		{
			if(Velocity_Y_cnt >delta_cnt1)
				Velocity_Y_cnt -=delta_cnt1;
			else if(Velocity_Y_cnt <-delta_cnt1)
				Velocity_Y_cnt +=delta_cnt1;
			else
				Velocity_Y_cnt =0;
		}
		
		if(Velocity_Y_cnt >acc_time)
			Velocity_Y_cnt =acc_time;
		else if(Velocity_Y_cnt <-acc_time)
			Velocity_Y_cnt =-acc_time;

		if (rc.kb.bit.W ==1)
			gimbal_axes_Y =(sin(Velocity_Y_cnt/acc_time*pi-pi/2)+1) /2 *MAX_CHASSIS_VY_SPEED;
		else if (rc.kb.bit.S ==1)
			gimbal_axes_Y =(sin(Velocity_Y_cnt/acc_time*pi+pi/2)-1) /2 *MAX_CHASSIS_VY_SPEED;
		else
		{
			if (Velocity_Y_cnt >0)
				gimbal_axes_Y =(sin(Velocity_Y_cnt/acc_time*pi -pi/2)+1) /2 *MAX_CHASSIS_VY_SPEED;
			else
				gimbal_axes_Y =(sin(Velocity_Y_cnt/acc_time*pi +pi/2)-1) /2 *MAX_CHASSIS_VY_SPEED;
		}
	}
	#endif
}

void left_right_speed_set()	//�����ƶ����ٲ���
{
	if ((rc.kb.bit.D ==1) &&(rc.kb.bit.A !=1))
		Velocity_X_cnt +=6;
	else if ((rc.kb.bit.A ==1) &&(rc.kb.bit.D !=1))
		Velocity_X_cnt -=6;
	else 
	{
		if (Velocity_X_cnt >10)
			Velocity_X_cnt -=10;
		else if (Velocity_X_cnt <-10)
			Velocity_X_cnt +=10;
		else
			Velocity_X_cnt =0;
	}
	
	if (Velocity_X_cnt >acc_time)
		Velocity_X_cnt =acc_time;
	else if (Velocity_X_cnt <-acc_time)
		Velocity_X_cnt =-acc_time;

	gimbal_axes_X =(float)Velocity_X_cnt /acc_time *MAX_CHASSIS_VX_SPEED;
}

void angle_process()	//���õ�������������Ƕȣ������������̨�ļн�
{
	if (fabs(gimbal_yaw.angle -CHASSIS_FOLLOW_CENTER) <4096)
		axes_angle =(gimbal_yaw.angle -CHASSIS_FOLLOW_CENTER )/8192.0f *2*pi;
	else if ((gimbal_yaw.angle -CHASSIS_FOLLOW_CENTER) <-4096)
		axes_angle =(gimbal_yaw.angle -CHASSIS_FOLLOW_CENTER +8192)/8192.0f *2*pi;
	else if ((gimbal_yaw.angle -CHASSIS_FOLLOW_CENTER) >4096)
		axes_angle =(gimbal_yaw.angle -CHASSIS_FOLLOW_CENTER -8192)/8192.0f *2*pi;
}

void chassis_task()	//��������
{
	static float target_omega=0;

	angle_process();
	power_control();
	
	if (robot.chassis_gimbal ==chassis_lock)
		chassis_follow_pid.pos_out =0;
	else if(robot.chassis_gimbal ==chassis_follow && fabs(axes_angle) < pi/2.0f)
		pid_calc(&chassis_follow_pid, axes_angle *360 /2.0f/pi, 0); //���̸���pid����
	else if(robot.chassis_gimbal ==chassis_follow && fabs(axes_angle) >= pi/2.0f &&axes_angle >0)
		pid_calc(&chassis_follow_pid, axes_angle *360 /2.0f/pi, 180); //���̸���pid����
	else if(robot.chassis_gimbal ==chassis_follow && fabs(axes_angle) >= pi/2.0f &&axes_angle <0)
		pid_calc(&chassis_follow_pid, axes_angle *360 /2.0f/pi, -180); //���̸���pid����

	if (mode_change_flag ==1 && axes_angle >-pi/4 && axes_angle <0)  //С�����л������̸���
	{
		mode_change_flag =0;
		robot.chassis_gimbal =chassis_follow;
	}

	if (robot.control_state ==RC)
	{
		static int16_t cnt_ch3 =0;
		static int16_t cnt_ch4 =0;
		if (rc.ch3 -cnt_ch3 >2)
			cnt_ch3 +=4;
		else if (rc.ch3 -cnt_ch3 <-2)
			cnt_ch3 -=4;
		else 
			cnt_ch3 =rc.ch3;
		
		if (rc.ch4 -cnt_ch4 >2)
			cnt_ch4 +=4;
		else if (rc.ch4 -cnt_ch4 <-2)
			cnt_ch4 -=4;
		else
			cnt_ch4 =rc.ch4;
    
		if (robot.chassis_gimbal ==chassis_follow)
		{
			mecname.Velocity_X =cos(axes_angle)*cnt_ch3/660.0f *MAX_CHASSIS_VX_SPEED -sin(axes_angle)*cnt_ch4 /660.0f*MAX_CHASSIS_VY_SPEED;
			mecname.Velocity_Y =sin(axes_angle)*cnt_ch3/660.0f *MAX_CHASSIS_VX_SPEED +cos(axes_angle)*cnt_ch4 /660.0f*MAX_CHASSIS_VY_SPEED;
			mecname.Palstance  =FOLLOW_DERICTION *chassis_follow_pid.pos_out;
		}
		else if (robot.chassis_gimbal ==chassis_rotate)
		{
			mecname.Velocity_X =cos(axes_angle)*cnt_ch3 /660.0f*MAX_CHASSIS_VX_SPEED -sin(axes_angle)*cnt_ch4 /660.0f*MAX_CHASSIS_VY_SPEED;
			mecname.Velocity_Y =sin(axes_angle)*cnt_ch3 /660.0f*MAX_CHASSIS_VX_SPEED +cos(axes_angle)*cnt_ch4 /660.0f*MAX_CHASSIS_VY_SPEED;
			target_omega =rotate_speed;
			if (target_omega >mecname.Palstance)
				mecname.Palstance +=0.6f;
			else if (target_omega <mecname.Palstance)
				mecname.Palstance -=0.6f;
		}
		else if (robot.chassis_gimbal ==chassis_lock)
		{
			mecname.Velocity_X =0;
			mecname.Velocity_Y =0;
			mecname.Palstance  =0;
		}
	}
	else if (robot.control_state ==KB)
	{
		forward_back_speed_set();
		left_right_speed_set();

		if (robot.chassis_gimbal ==chassis_follow)
		{
			mecname.Velocity_X =cos(axes_angle)*gimbal_axes_X -sin(axes_angle)*gimbal_axes_Y;
			mecname.Velocity_Y =sin(axes_angle)*gimbal_axes_X +cos(axes_angle)*gimbal_axes_Y;
			mecname.Palstance  =FOLLOW_DERICTION *chassis_follow_pid.pos_out ;
		}
		else if (robot.chassis_gimbal ==chassis_rotate)
		{
			mecname.Velocity_X =cos(axes_angle)*gimbal_axes_X -sin(axes_angle)*gimbal_axes_Y;
			mecname.Velocity_Y =sin(axes_angle)*gimbal_axes_X +cos(axes_angle)*gimbal_axes_Y;
			target_omega =rotate_speed;
			if (target_omega >mecname.Palstance)
				mecname.Palstance +=0.8f;
			else if (target_omega <mecname.Palstance)
				mecname.Palstance -=0.8f;
		}
		else if (robot.chassis_gimbal ==chassis_lock)
		{
			mecname.Velocity_X =0;
			mecname.Velocity_Y =0;
			mecname.Palstance  =0;
		}
	}
	//mecname�ֽ���
	mecname_calc();

	pid_calc(&chassis_LF_pid,chassis_LF.speed_rpm ,wheel_rpm[0]);
	pid_calc(&chassis_RF_pid,chassis_RF.speed_rpm ,wheel_rpm[1]);
	pid_calc(&chassis_RB_pid,chassis_RB.speed_rpm ,wheel_rpm[2]);
	pid_calc(&chassis_LB_pid,chassis_LB.speed_rpm ,wheel_rpm[3]);
}
		
	
