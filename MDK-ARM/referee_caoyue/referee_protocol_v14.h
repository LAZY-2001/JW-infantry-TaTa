#ifndef __REFEREE_PROTOCOL_V14_H
#define __REFEREE_PROTOCOL_V14_H

#include "stm32f4xx_hal.h"

//����״̬���ݣ�0x0001������Ƶ�ʣ�3Hz
typedef __packed struct
{
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
	uint64_t SyncTimeStamp;
} ext_game_status_t;

//����������ݣ�0x0002������Ƶ�ʣ�������������
typedef __packed struct
{
	uint8_t winner;
} ext_game_result_t;

//������Ѫ�����ݣ�0x0003������Ƶ�ʣ�3Hz
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

//�˹�������ս���ӳ�\�ͷ����ֲ���Ǳ��ģʽ״̬��0x0005������Ƶ�ʣ�1Hz ���ڷ��ͣ����ͷ�Χ�����л�����
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

//�����¼����ݣ�0x0101������Ƶ�ʣ�3Hz
typedef __packed struct
{
	uint32_t event_type;
} ext_event_data_t;

//����վ������ʶ��0x0102������Ƶ�ʣ������ı����, ���ͷ�Χ������������
typedef __packed struct
{
	uint8_t supply_projectile_id; 
	uint8_t supply_robot_id; 
	uint8_t supply_projectile_step; 
	uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

//���о�����Ϣ��cmd_id (0x0104)������Ƶ�ʣ��������淢������
typedef __packed struct
{
	uint8_t level;
	uint8_t foul_robot_id; 
} ext_referee_warning_t;

//���ڷ���ڵ���ʱ��cmd_id (0x0105)������Ƶ�ʣ�3Hz ���ڷ��ͣ����ͷ�Χ������������
typedef __packed struct
{
	uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;

//����������״̬��0x0201������Ƶ�ʣ�10Hz
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

//ʵʱ�����������ݣ�0x0202������Ƶ�ʣ�50Hz
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

//������λ�ã�0x0203������Ƶ�ʣ�10Hz
typedef __packed struct
{
	float x;
	float y;
	float z;
	float yaw;
} ext_game_robot_pos_t;

//���������棺0x0204������Ƶ�ʣ�1Hz
typedef __packed struct
{
	uint8_t power_rune_buff;
}ext_buff_t;

//���л���������״̬��0x0205������Ƶ�ʣ�10Hz
typedef __packed struct
{
	uint8_t attack_time;
} aerial_robot_energy_t;

//�˺�״̬��0x0206������Ƶ�ʣ��˺���������
typedef __packed struct
{
	uint8_t armor_id : 4;
	uint8_t hurt_type : 4;
} ext_robot_hurt_t;

//ʵʱ�����Ϣ��0x0207������Ƶ�ʣ��������
typedef __packed struct
{
	uint8_t bullet_type;
	uint8_t shooter_id;
	uint8_t bullet_freq;
	float bullet_speed;
} ext_shoot_data_t;

//�ӵ�ʣ�෢������0x0208������Ƶ�ʣ�10Hz ���ڷ��ͣ����л����˷���
typedef __packed struct
{
	uint16_t bullet_remaining_num_17mm;
	uint16_t bullet_remaining_num_42mm;
	uint16_t coin_remaining_num;
} ext_bullet_remaining_t;

//������ RFID ״̬��0x0209������Ƶ�ʣ�3Hz�����ͷ�Χ����һ������
typedef __packed struct
{
	uint32_t rfid_status;
} ext_rfid_status_t;

//���ڻ����˿ͻ���ָ�����ݣ�0x020A������Ƶ�ʣ�10Hz�����ͷ�Χ����һ������
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

typedef __packed struct //robot_interactive_data_t�����˼�ͨ��
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
