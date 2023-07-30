#ifndef __REFEREE__CONTROL_H
#define __REFEREE__CONTROL_H

#include "stm32f4xx_hal.h"
#include "referee_receiver.h"

extern RRManager_t RRManager;

void referee_control_startupInit(void);
void referee_control_loop(void);

void client_send_task(void);
void client_send_data(void);
void client_send_data0(void);
void client_send_data1(void);
void client_send_data2(void);
void client_send_data3(void);
void client_send_data4(void);
void client_send_data5(void);
void client_send_data6(void);
void client_send_data7(void);
void client_send_data7_1(void);
void client_send_data6_1(void);
void robots_send_data(void ); //机器人间通信

#endif /* __REFEREE__CONTROL_H */
