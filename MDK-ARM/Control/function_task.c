#include "includes.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"
robot_t robot;
robot_t last_robot;
uint8_t shoot_flag=0;
int32_t sp = 0;
uint8_t autofire = 0;
int32_t aim = 0;
static int flag = 0;
void robot_state_init()  //������״̬��ʼ��
{
	//robot.aiming_state   =sbuff_aim;
	//robot.aiming_state   =no_aim;
	robot.aiming_state   =armor_aim;
	//robot.aiming_state   =no_aim;
	robot.ammo_state     =normal;
	robot.chassis_gimbal =chassis_follow;
	robot.fric_state     =fric_off;
	robot.gate_state     =gate_off;
	robot.shoot_state    =shoot_one_bullet;
	robot.shoot_freq 	 =shoot_freq_low;
}

uint8_t mode_change_flag=0;

void chassis_gimbal_state()  //������̨����
{
	static uint8_t cnt_q=0;
	static uint8_t cnt_aim=0;
	if (robot.control_state == KB)
	{	
		if (last_rc.kb.bit.Q ==0 && rc.kb.bit.Q ==1)   //QС����
		{
			cnt_q++;

			if (cnt_q%2 ==1)
				robot.chassis_gimbal =chassis_rotate;
			else
				mode_change_flag =1;
		}
		if (rc.kb.bit.CTRL ==1 && last_rc.kb.bit.F ==0 && rc.kb.bit.F ==1)  //һ����ͷ
			GMY_target_angle += 180;
	}
	else if (robot.control_state == RC)            //ң����sw2����ȥ����������  ż���ε��̸���
	{																							//ң����sw2����ȥ�����δ��  ż����С��
		if (rc.sw2==1 && last_rc.sw2 !=1)
		{
			cnt_aim++;
			if (cnt_aim%2==1)
				robot.aiming_state  =armor_aim;
			else 
				robot.aiming_state   =no_aim;
		}
		if(rc.wheel >600)  //���������ؼ�⣬���̸���͵�������֮��ת��
		{
			autofire = 1;
			robot.chassis_gimbal =chassis_rotate;
		}
		else {
			autofire =0;
			robot.chassis_gimbal = chassis_follow;
		}
	}

	if(pc_energy.chassis_lock == 10 && (robot.aiming_state == sbuff_aim || robot.aiming_state == bbuff_aim ) ) //�Ӿ��л�����״̬
	{
		robot.chassis_gimbal = chassis_follow;//
		PID_struct_init(&yaw_aim_pid, 10, 2, 1.2,	0.1, 1);//����
		#ifdef INFANTRY_RAPH
		PID_struct_init(&yaw_aim_pid, 10, 2, 1.6,	0.1, 1.2);//����
		#endif
	}
	else if(pc_energy.chassis_lock == 11 && (robot.aiming_state == sbuff_aim || robot.aiming_state == bbuff_aim ) )
	{
		robot.chassis_gimbal = chassis_lock;
		PID_struct_init(&yaw_aim_pid, 10, 2, 2,	0, 5);//����
	}
}
	

void gate_state()  //�����Ź���
{
	static uint8_t cnt_g =0;
	if (robot.control_state ==KB)
	{
		if (last_rc.kb.bit.G ==0 &&rc.kb.bit.G ==1)   //G����
			cnt_g++;
		if (cnt_g%2 ==1)
			robot.gate_state =gate_on;
		else
			robot.gate_state =gate_off;
	}
	else if (robot.control_state ==RC)           //ң����sw2����     sw1���¿�������
	{
		if (rc.sw2 ==3 &&rc.sw1 ==2)
			robot.gate_state =gate_on;
		else 
			robot.gate_state =gate_off;
	}
}

void aim_state()  //�Ӿ�����
{
	if (robot.control_state ==KB)
	{
		if (rc.mouse.r ==1)   //����Ҽ�������
			robot.aiming_state =armor_aim;
		if (last_rc.mouse.r ==1 && rc.mouse.r ==0)   //����Ҽ�����������
			robot.aiming_state =no_aim;
		if (last_rc.kb.bit.X ==0 && rc.kb.bit.X ==1)   //X  ?
		{
			flag++;
			if(flag % 2 ==1)				
			{
				autofire = 1;
			}
			else autofire = 0;
		}
	}
}

void  fric_state()  //Ħ���ֹ���
{
	if(robot.control_state ==RC)
	{
		if(rc.sw2 ==2 &&rc.sw1 !=2)
			robot.fric_state =fric_on;
		else
			robot.fric_state =fric_off;
	}
	else if(robot.control_state ==KB)
	{
		robot.fric_state =fric_on;
	}
}
			
void shoot_state()  //�������
{
	if (robot.control_state ==RC)
	{
		if (robot.fric_state ==fric_on && rc.sw1 ==1)
		{
			if (last_rc.sw1 ==3)  //����
			{
			#ifdef RC_SHOOT_ONE
				shoot_flag =1;
				robot.shoot_state =shoot_one_bullet;
			#endif
			}
			#ifdef RC_SHOOT_CONTINUOUS
				robot.shoot_state =shoot_continuous;
			  shoot_flag =1;
			#endif
				
			
		}
	}
	else if(robot.control_state ==KB)
	{
		if (last_rc.kb.bit.R ==0 && rc.kb.bit.R ==1 && rc.kb.bit.CTRL == 0) //�л����ģʽ
		{
			aim++;
		}
		if (last_rc.kb.bit.E ==0 && rc.kb.bit.E ==1 && rc.kb.bit.CTRL == 0) 
		{
			aim--;
		}
		if (last_rc.kb.bit.R ==0 && rc.kb.bit.R ==1 && rc.kb.bit.CTRL == 1) //�л����ģʽ
		{
			aim++;
		}
		if (last_rc.kb.bit.E ==0 && rc.kb.bit.E ==1 && rc.kb.bit.CTRL == 1) 
		{
			aim--;
		}
		if (last_rc.kb.bit.V ==0 && rc.kb.bit.V ==1)
			sp +=1;
		if (last_rc.kb.bit.B ==0 && rc.kb.bit.B ==1)
			sp-=1;
			
		if (robot.shoot_state ==shoot_one_bullet)  //����
		{
			if (last_rc.mouse.l==0 &&rc.mouse.l ==1) //������·���
				shoot_flag=1;
			else
				shoot_flag=0;
		}
		
		if (robot.shoot_state ==shoot_continuous)  //����
		{
			if (rc.mouse.l ==1)
				shoot_flag=1;
			else 
				shoot_flag=0;
		}
	}
}
