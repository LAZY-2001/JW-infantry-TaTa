#ifndef __BSP_CAN__
#define __BSP_CAN__

#include "can.h"

#define FILTER_BUF_LEN		5
#ifndef PI
	#define PI    3.14159265358979f
#endif

/*??????????????*/
typedef struct{
	int16_t	 	  speed_rpm;
  int16_t  	  given_current;
  uint8_t  	  temperature;
	int16_t 	  angle;				//abs angle range:[0,8191]
	int16_t 	  last_angle;	//abs angle range:[0,8191]
	int16_t	    offset_angle;
	int32_t		  round_cnt;
	int32_t		  total_angle;
	uint8_t			buf_idx;
	uint16_t		angle_buf[FILTER_BUF_LEN];
	uint16_t		fited_angle;
	uint32_t		msg_cnt;
}moto_measure_t;

/*功率控制器反馈数据，除以100得到实际值*/
typedef struct{
	uint16_t input_vot;      //输入电压
	uint16_t cap_vot;        //电容电压
	uint16_t input_cur;      //输入电流
	uint16_t target_power;   //目标功率
}powerctrl_measure_t;

typedef struct{
	uint32_t StdId;
	uint8_t rx_data[8];
}can_rx_msg;


typedef struct{
	uint16_t heat;
	uint8_t  enemy_colour;         //蓝色为0   红色为1
	int16_t curr_speed;           
}exchange_info_t;
	
/*CAN???????ID*/
enum CAN1_ID{

	CHASSIS_LF 	= 0x201,
	CHASSIS_RF 	= 0x202,
	CHASSIS_RB 	= 0x203,
	CHASSIS_LB 	= 0x204,
	
	GIMBAL_YAW  = 0x206,
	GATE_MOTO   = 0x207,
	AMMO_BOOSTER= 0x208,
	POWERCTRL_BOARD = 0x211,
};

enum CAN2_ID
{
	FRIC_LEFT   = 0x201,
	FRIC_RIGHT  = 0x202,
	GIMBAL_PITCH= 0x205,
};

typedef struct 
{
	int16_t AccX;
	int16_t AccY;
	int16_t AccZ;
	int16_t GyoX;
	int16_t GyoY;
	int16_t GyoZ;
	int16_t Pitch;
	int16_t Roll;
	int16_t Yaw;
}gyro_info_t_new;

typedef struct {
	float AccX;
	float AccY;
	float AccZ;
	float GyoX;
	float GyoY;
	float GyoZ;
	float Pitch;
	float Roll;
	float Yaw;
}gyro_info_t_new_t;

float expect_speed_calc(moto_measure_t *ptr);
void get_moto_measure(moto_measure_t *ptr, uint8_t Data[8]);
void can_filter_config(void);

void can_transmit_msg_one(CAN_HandleTypeDef *hcan, uint8_t tx_current_data_one[8]);
void can_transmit_msg_two(CAN_HandleTypeDef *hcan,  uint8_t tx_current_data_two[8]);
void can_transmit_msg_three(CAN_HandleTypeDef *hcan, uint8_t tx_current_data_one[8]);
void can_transmit_msg_four(CAN_HandleTypeDef *hcan,uint8_t tx_power_data[8]);
void can_transmit_chassis_board(CAN_HandleTypeDef *hcan,exchange_info_t *exchange_info);

void set_current(uint8_t Data[8], int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);
void set_power(uint8_t Data[8], uint16_t power);

extern moto_measure_t  chassis_LF;
extern moto_measure_t  chassis_RF;
extern moto_measure_t  chassis_RB;
extern moto_measure_t  chassis_LB;
extern moto_measure_t  gimbal_yaw;
extern moto_measure_t  gimbal_pitch;
extern moto_measure_t  fric_left;
extern moto_measure_t  fric_right;
extern moto_measure_t  ammo_booster;
extern moto_measure_t  gate_moto;

extern powerctrl_measure_t powerctrl_board;

extern exchange_info_t exchange_info;

extern gyro_info_t_new_t gyro_data;
extern uint32_t alive_timer;
#endif

