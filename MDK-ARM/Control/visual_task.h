#ifndef _VISUAL
#define _VISUAL
#include "includes.h"

extern PCSendPacket_t mcu_data;

void predict_task(void);
void aim_task(void);
void send_pluse(void);
extern float GMY_target_angle_pc;
extern float GMP_target_angle_pc;
extern float GMP_target_angle_err;

/*
˵����

      1.ʹ��ǰ��Ҫ��cube������PF10���������

      2.�Ӿ�����ϵͳ���̽��ܣ���ط�����
         ���ͣ������������������գ�send_pluse��->��ط��ɼ����ݲ����ͣ�send_mcu_data��-> ��¼ʱ�����enterlink����         
         ���ܣ���ط��ɴ���3�������ݣ������������������ڽ�����ͬ����bsp_uart.c�ļ���-> �ж�����ִ���Ӿ��������ͣ�gimbal.c�ļ���gimbal_task��
               �����С�� -> ���Ӿ�Ҫ������������̨�Ƕȣ��޾��庯������gimbal_task�д˶δ��룩
               ����Ǵ�� -> ���Ӿ�Ҫ������������̨�Ƕȣ��޾��庯������gimbal_task�д˶δ��룩
							 ��������� -> ���Ӿ�Ҫ������������̨�Ƕȣ�predict_task�� -> ���ж�ȡʱ����������ݣ�checklink_1��
			   ps����С���������������Ƿֿ��������Դ���û�кϵ�һ���ļ��У���ʱ�佨��ϵ�һ�𣬻��ǱȽϼ򵥵ġ�
				 
			3.ʱ���ϵͳ���ܣ�
			  ���ȣ���Ϊ�Ӿ��������ݣ����͵�yaw��Ƕ�Ϊ��ԽǶ�
			  ���Ҵӵ�ط��������ݵ��Ӿ������Ӿ����������ݷ��ͻ���ʱ��ʱ���ӳ٣������ʱ����һֱ�ڴ����Լ��ĳ��򣨵��һ����ת��������ͣ���ڷ����Ӿ����ݵ���һ�̣�
				�������ǽ������Ӿ�����һ֡yaw��������ʱ�䱣�����Ӿ�������ط�������ʱ�䱣���������ص�ط�����ʱ��ط��Ϳ��Ը����յ���ʱ��ȷ���Ӿ����������һ֡���ݣ���ʹ�õ�ʱ��yaw��Ƕȼ��㡣
				
				
      4.GMP_target_angle_err��pitch���޷���Ҫ�����޷���������

									GMP_target_angle += 0.1 *GMP_target_angle_err;
										if(GMP_target_angle_err>0){
											if(GMP_target_angle>=GMP_target_angle_pc){
													GMP_target_angle=GMP_target_angle_pc;
												}
										}
										else{
											if(GMP_target_angle<=GMP_target_angle_pc){
													GMP_target_angle=GMP_target_angle_pc;
												}
										}
		
        ������̨�����������޸ġ����Ӿ�ʶ���ʱ����Ҫ��
*/
#endif 

