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
说明：

      1.使用前需要在cube中配置PF10脉冲输出。

      2.视觉自瞄系统流程介绍（电控方）：
         发送：触发脉冲控制相机拍照（send_pluse）->电控方采集数据并发送（send_mcu_data）-> 记录时间戳（enterlink）。         
         接受：电控方由串口3接受数据（接受数据与其他串口接受相同，见bsp_uart.c文件）-> 判断所需执行视觉任务类型（gimbal.c文件中gimbal_task）
               如果是小符 -> 按视觉要求计算所需的云台角度（无具体函数，见gimbal_task中此段代码）
               如果是大符 -> 按视觉要求计算所需的云台角度（无具体函数，见gimbal_task中此段代码）
							 如果是自瞄 -> 按视觉要求计算所需的云台角度（predict_task） -> 其中读取时间戳处理数据（checklink_1）
			   ps：大小能量机关与自瞄是分开做的所以代码没有合到一个文件中，有时间建议合到一起，还是比较简单的。
				 
			3.时间戳系统介绍：
			  首先，因为视觉处理数据，发送的yaw轴角度为相对角度
			  并且从电控方发送数据到视觉，到视觉处理完数据发送回来时有时间延迟，而这段时间电控一直在处理自己的程序（电机一定在转动，不会停留在发给视觉数据的那一刻）
				所以我们将发给视觉的那一帧yaw轴数据与时间保留，视觉方将电控发给他的时间保留，并发回电控方，此时电控方就可以根据收到的时间确认视觉处理的是哪一帧数据，并使用当时的yaw轴角度计算。
				
				
      4.GMP_target_angle_err是pitch轴限幅需要的误差，限幅代码如下

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
		
        放在云台控制中酌情修改。（视觉识别好时不需要）
*/
#endif 

