
#ifndef _FUNCTION_
#define _FUNCTION_
#include "stm32f4xx_hal.h"
#define key_W 0x0001
#define key_S 0x0002
#define key_A 0x0004
#define key_D 0x0008
#define key_SHIFT 0x0010
#define key_CTRL 0x0020
#define key_Q 0x0040
#define key_E 0x0080
#define key_R 0x0100
#define key_F 0x0200
#define key_G 0x0400
#define key_Z 0x0800
#define key_X 0x1000
#define key_C 0x2000
#define key_V 0x4000
#define key_B 0x8000

/* chassis maximum translation speed, unit is mm/s */

/* chassis maximum rotation speed, unit is degree/s */
#define MAX_CHASSIS_VW_SPEED 500 //5000rpm

void robot_state_init(void);

//void function_task(void);

void	chassis_gimbal_state(void);
void	gate_state(void);
void	fric_state(void);
void	shoot_state(void);
void  aim_state(void);
extern uint8_t shoot_flag;
extern uint8_t mode_change_flag;
#endif 


