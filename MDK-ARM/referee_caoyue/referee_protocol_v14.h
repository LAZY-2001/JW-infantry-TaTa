#ifndef __REFEREE_PROTOCOL_V14_H
#define __REFEREE_PROTOCOL_V14_H

#include "stm32f4xx_hal.h"

//比赛状态数据：0x0001。发送频率：3Hz
typedef __packed struct
{
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
	uint64_t SyncTimeStamp;
} ext_game_status_t;

//比赛结果数据：0x0002。发送频率：比赛结束后发送
typedef __packed struct
{
	uint8_t winner;
} ext_game_result_t;

//机器人血量数据：0x0003。发送频率：3Hz
typedef __packed struct
{
	uint16_t red_1_robot_HP;
	uint16_t red_2_robot_HP; 
	uint16_t red_3_robot_HP; 
	uint16_t red_4_robot_HP; 
	uint16_t red_5_robot_HP; 
	uint16_t red_7_robot_HP; 
	uint16_t red_outpost_HP;
	uint16_t red_base_HP; 
	uint16_t blue_1_robot_HP; 
	uint16_t blue_2_robot_HP; 
	uint16_t blue_3_robot_HP; 
	uint16_t blue_4_robot_HP; 
	uint16_t blue_5_robot_HP; 
	uint16_t blue_7_robot_HP; 
	uint16_t blue_outpost_HP;
	uint16_t blue_base_HP;
} ext_game_robot_HP_t;

//人工智能挑战赛加成\惩罚区分布与潜伏模式状态：0x0005。发送频率：1Hz 周期发送，发送范围：所有机器人
typedef __packed struct
{
	uint8_t F1_zone_status:1;
	uint8_t F1_zone_buff_debuff_status:3; 
	uint8_t F2_zone_status:1;
	uint8_t F2_zone_buff_debuff_status:3; 
	uint8_t F3_zone_status:1;
	uint8_t F3_zone_buff_debuff_status:3; 
	uint8_t F4_zone_status:1;
	uint8_t F4_zone_buff_debuff_status:3; 
	uint8_t F5_zone_status:1;
	uint8_t F5_zone_buff_debuff_status:3; 
	uint8_t F6_zone_status:1;
	uint8_t F6_zone_buff_debuff_status:3;
	uint16_t red1_bullet_left;
	uint16_t red2_bullet_left;
	uint16_t blue1_bullet_left;
	uint16_t blue2_bullet_left;
	uint8_t lurk_mode;
	uint8_t res;
} ext_ICRA_buff_debuff_zone_and_lurk_status_t;

//场地事件数据：0x0101。发送频率：3Hz
typedef __packed struct
{
	uint32_t event_type;
} ext_event_data_t;

//补给站动作标识：0x0102。发送频率：动作改变后发送, 发送范围：己方机器人
typedef __packed struct
{
	uint8_t supply_projectile_id; 
	uint8_t supply_robot_id; 
	uint8_t supply_projectile_step; 
	uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

//裁判警告信息：cmd_id (0x0104)。发送频率：己方警告发生后发送
typedef __packed struct
{
	uint8_t level;
	uint8_t foul_robot_id; 
} ext_referee_warning_t;

//飞镖发射口倒计时：cmd_id (0x0105)。发送频率：3Hz 周期发送，发送范围：己方机器人
typedef __packed struct
{
	uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;

//比赛机器人状态：0x0201。发送频率：10Hz
typedef __packed struct
{
	uint8_t robot_id;
	uint8_t robot_level;
	uint16_t remain_HP;
	uint16_t max_HP;
	uint16_t shooter_id1_17mm_cooling_rate;
	uint16_t shooter_id1_17mm_cooling_limit;
	uint16_t shooter_id1_17mm_speed_limit;
	uint16_t shooter_id2_17mm_cooling_rate;
	uint16_t shooter_id2_17mm_cooling_limit;
	uint16_t shooter_id2_17mm_speed_limit;
	uint16_t shooter_id1_42mm_cooling_rate;
	uint16_t shooter_id1_42mm_cooling_limit;
	uint16_t shooter_id1_42mm_speed_limit;
	uint16_t chassis_power_limit;
	uint8_t mains_power_gimbal_output : 1;
	uint8_t mains_power_chassis_output : 1;
	uint8_t mains_power_shooter_output : 1;
} ext_game_robot_status_t;

//实时功率热量数据：0x0202。发送频率：50Hz
typedef __packed struct
{
	uint16_t chassis_volt; 
	uint16_t chassis_current; 
	float chassis_power; 
	uint16_t chassis_power_buffer; 
	uint16_t shooter_id1_17mm_cooling_heat;
	uint16_t shooter_id2_17mm_cooling_heat;
	uint16_t shooter_id1_42mm_cooling_heat;
} ext_power_heat_data_t;

//机器人位置：0x0203。发送频率：10Hz
typedef __packed struct
{
	float x;
	float y;
	float z;
	float yaw;
} ext_game_robot_pos_t;

//机器人增益：0x0204。发送频率：1Hz
typedef __packed struct
{
	uint8_t power_rune_buff;
}ext_buff_t;

//空中机器人能量状态：0x0205。发送频率：10Hz
typedef __packed struct
{
	uint8_t attack_time;
} aerial_robot_energy_t;

//伤害状态：0x0206。发送频率：伤害发生后发送
typedef __packed struct
{
	uint8_t armor_id : 4;
	uint8_t hurt_type : 4;
} ext_robot_hurt_t;

//实时射击信息：0x0207。发送频率：射击后发送
typedef __packed struct
{
	uint8_t bullet_type;
	uint8_t shooter_id;
	uint8_t bullet_freq;
	float bullet_speed;
} ext_shoot_data_t;

//子弹剩余发射数：0x0208。发送频率：10Hz 周期发送，所有机器人发送
typedef __packed struct
{
	uint16_t bullet_remaining_num_17mm;
	uint16_t bullet_remaining_num_42mm;
	uint16_t coin_remaining_num;
} ext_bullet_remaining_t;

//机器人 RFID 状态：0x0209。发送频率：3Hz，发送范围：单一机器人
typedef __packed struct
{
	uint32_t rfid_status;
} ext_rfid_status_t;

//飞镖机器人客户端指令数据：0x020A。发送频率：10Hz，发送范围：单一机器人
typedef __packed struct
{
	uint8_t dart_launch_opening_status;
	uint8_t dart_attack_target;
	uint16_t target_change_time;
	uint16_t operate_launch_cmd_time;
} ext_dart_client_cmd_t;

//The following is the edit manual interface

typedef __packed struct
{
	uint8_t SOF; 
	uint16_t data_length;
	uint8_t seq;
}frame_head_t; 

typedef __packed struct
{
	uint16_t data_cmd_id;
	uint16_t sender_ID;
	uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

typedef __packed struct
{ 
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; 
	uint32_t layer:4; 
	uint32_t color:4; 
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10; 
	uint32_t start_x:11; 
	uint32_t start_y:11; 
	uint32_t radius:10; 
	uint32_t end_x:11; 
	uint32_t end_y:11; 
} graphic_data_struct_t;

typedef __packed struct
{
	frame_head_t frame_head;
	uint8_t CRC8;
	uint16_t cmd_id;
	uint8_t operate_tpye; 
	uint8_t layer; 
	uint16_t CRC16;
} ext_client_custom_graphic_delete_t;

typedef __packed struct
{
	frame_head_t frame_head;
	uint8_t CRC8;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t student_interactive_header_data;
	graphic_data_struct_t grapic_data_struct;
	uint16_t CRC16;
} ext_client_custom_graphic_single_t;

typedef __packed struct
{
	frame_head_t frame_head;
	uint8_t CRC8;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t student_interactive_header_data;
	graphic_data_struct_t grapic_data_struct[2];
	uint16_t CRC16;
} ext_client_custom_graphic_double_t;

typedef __packed struct
{
	frame_head_t frame_head;
	uint8_t CRC8;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t student_interactive_header_data;
	graphic_data_struct_t grapic_data_struct[5];
	uint16_t CRC16;
} ext_client_custom_graphic_five_t;

typedef __packed struct
{
	frame_head_t frame_head;
	uint8_t CRC8;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t student_interactive_header_data;
	graphic_data_struct_t grapic_data_struct;
	uint8_t data[30];
	uint16_t CRC16;
} ext_client_custom_character_t;

typedef __packed  struct
{
	frame_head_t frame_head;
	uint8_t CRC8;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t student_interactive_header_data;
	graphic_data_struct_t grapic_data_struct[7];
	uint16_t CRC16;
} ext_client_custom_graphic_seven_t;

typedef __packed struct //robot_interactive_data_t机器人间通信
{
	frame_head_t frame_head;
	uint8_t CRC8;
	uint16_t cmd_id;
	ext_student_interactive_header_data_t student_interactive_header_data;
	uint8_t data;
	uint16_t CRC16; 
} robot_interactive_data_t;

typedef enum
{
	GRAPHIC_OPERATE_EMPTY = 0,
	GRAPHIC_OPERATE_ADD,
	GRAPHIC_OPERATE_CHANGE,
	GRAPHIC_OPERATE_DELETE,
} Graphic_operate_e;

extern ext_game_robot_status_t game_robot_status;  
extern ext_power_heat_data_t power_heat_data;  
extern ext_buff_t robot_buff;
extern ext_shoot_data_t shoot_data;
extern ext_bullet_remaining_t bullet_remaining;
#endif /* __REFEREE_PROTOCOL_V14_H */
