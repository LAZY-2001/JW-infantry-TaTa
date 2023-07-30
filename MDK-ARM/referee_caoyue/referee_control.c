#include "includes.h"
#include "nonblocktimer.h"
#include "usart.h"
#include "referee_protocol_v14.h"
#include "referee_receiver.h"

RRManager_t RRManager;
RRNode_t RRNode_0x201;
ext_game_robot_status_t game_robot_status;  //最终裁判系统0x201数据在这，从这读
RRNode_t RRNode_0x202;
ext_power_heat_data_t power_heat_data;  //最终裁判系统0x202数据在这，从这读
RRNode_t RRNode_0x204;
ext_buff_t robot_buff;
RRNode_t RRNode_0x207;
ext_shoot_data_t shoot_data;
RRNode_t RRNode_0x208;
ext_bullet_remaining_t bullet_remaining;

//开机初始化
void referee_control_startupInit(void)
{
	//裁判系统接收初始化
	RRManagerInit(&RRManager, &huart6);
	//添加节点
	RRNodeInit(&RRNode_0x201, 0x201, (void*)&game_robot_status, sizeof(ext_game_robot_status_t), NULL);
	RRNodeAddToManager(&RRNode_0x201, &RRManager);
	RRNodeInit(&RRNode_0x202, 0x202, (void*)&power_heat_data, sizeof(ext_power_heat_data_t), NULL);
	RRNodeAddToManager(&RRNode_0x202, &RRManager);
	RRNodeInit(&RRNode_0x204, 0x204, (void*)&robot_buff, sizeof(ext_buff_t), NULL);
	RRNodeAddToManager(&RRNode_0x204, &RRManager);
	RRNodeInit(&RRNode_0x207, 0x207, (void*)&shoot_data, sizeof(ext_shoot_data_t), NULL);
	RRNodeAddToManager(&RRNode_0x207, &RRManager);
	RRNodeInit(&RRNode_0x208, 0x208, (void*)&bullet_remaining, sizeof(ext_bullet_remaining_t), NULL);
	RRNodeAddToManager(&RRNode_0x208, &RRManager);
	//串口使能
	RRManagerStart(&RRManager);
}

//轮询，处理fifo队列中的数据
void referee_control_loop(void)
{
	referee_unpack_fifo_data(&RRManager);
}

/*操作手客户端，UI界面图形绘制*/
void client_send_data()  //绘制一个图形
{
	static uint8_t client_data[120];
	static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single;

	ext_client_custom_graphic_single.frame_head .SOF =0xA5;
	ext_client_custom_graphic_single.frame_head .data_length =21;
	ext_client_custom_graphic_single.frame_head .seq =0;
	ext_client_custom_graphic_single.cmd_id =0x0301;
	ext_client_custom_graphic_single.student_interactive_header_data .data_cmd_id =0x0101;
	ext_client_custom_graphic_single.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
	ext_client_custom_graphic_single.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
	ext_client_custom_graphic_single.grapic_data_struct .graphic_name [0] =0x01;  //图形名
	ext_client_custom_graphic_single.grapic_data_struct .graphic_name [1] =0x01;
	ext_client_custom_graphic_single.grapic_data_struct .graphic_name [2] =0x01;
	ext_client_custom_graphic_single.grapic_data_struct .operate_tpye =1;  //增加
	ext_client_custom_graphic_single.grapic_data_struct .graphic_tpye =0;  //直线
	ext_client_custom_graphic_single.grapic_data_struct .layer =0;  //图层
	ext_client_custom_graphic_single.grapic_data_struct .color =2;  //绿色
	ext_client_custom_graphic_single.grapic_data_struct .start_angle =0;
	ext_client_custom_graphic_single.grapic_data_struct .end_angle =0;
	ext_client_custom_graphic_single.grapic_data_struct .width =2;
	ext_client_custom_graphic_single.grapic_data_struct .start_x =610;
	ext_client_custom_graphic_single.grapic_data_struct .start_y =900;
	ext_client_custom_graphic_single.grapic_data_struct .radius =0;
	ext_client_custom_graphic_single.grapic_data_struct .end_x =1295;
	ext_client_custom_graphic_single.grapic_data_struct .end_y =900;

	memcpy(client_data, &ext_client_custom_graphic_single, sizeof(ext_client_custom_graphic_single));

	Append_CRC8_Check_Sum(client_data, 5);
	Append_CRC16_Check_Sum(client_data, sizeof(ext_client_custom_graphic_single));
	HAL_UART_Transmit_DMA(&REFEREE_HUART, client_data, sizeof(ext_client_custom_graphic_single));
}

void client_send_data0()  //新建UI，显示底盘、弹仓、自瞄等信息
{
 static uint8_t client_data[120];
 static ext_client_custom_character_t       ext_client_custom_character;
 static char chassis_state[10]={0};
 static char gate_state[10]={0};
 static char aim_state[11]={0};
 
 if (robot .chassis_gimbal ==chassis_follow )
  memcpy(chassis_state,"DP:       ",10);
 else if(robot .chassis_gimbal ==chassis_rotate )
  memcpy(chassis_state,"DP:       ",10);
 else
  memcpy(chassis_state,"DP:       ",10);
 
 if (robot .gate_state ==gate_on)
  memcpy(gate_state,"DC:          ",10);
 else 
  memcpy(gate_state,"DC:          ",10);
 
 if (robot .aiming_state ==armor_aim)
  memcpy(aim_state,"AIM:       ",11);
 else if (robot .aiming_state ==bbuff_aim )
  memcpy(aim_state,"AIM:       ",11);
 else if (robot .aiming_state ==sbuff_aim )//pred_aim
  memcpy(aim_state,"AIM:       ",11);
 else 
  memcpy(aim_state,"AIM:       ",11);
 
 ext_client_custom_character.frame_head .SOF =0xA5;
 ext_client_custom_character.frame_head .data_length =51;
 ext_client_custom_character.frame_head .seq =0;
 ext_client_custom_character.cmd_id =0x0301;
 ext_client_custom_character.student_interactive_header_data .data_cmd_id =0x0110;
 ext_client_custom_character.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_character.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_character.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_character.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_character.grapic_data_struct .graphic_name [2] =0x02;
 ext_client_custom_character.grapic_data_struct .operate_tpye =1;  //增加
 ext_client_custom_character.grapic_data_struct .graphic_tpye =7;  //字符
 ext_client_custom_character.grapic_data_struct .layer =0;
 ext_client_custom_character.grapic_data_struct .color =4;
 ext_client_custom_character.grapic_data_struct .start_angle =25;
 ext_client_custom_character.grapic_data_struct .end_angle =30;
 ext_client_custom_character.grapic_data_struct .width =3;
 ext_client_custom_character.grapic_data_struct .start_x =625;
 ext_client_custom_character.grapic_data_struct .start_y =840;
 ext_client_custom_character.grapic_data_struct .radius =0;
 ext_client_custom_character.grapic_data_struct .end_x =0;
 ext_client_custom_character.grapic_data_struct .end_y =0;
// for (uint8_t i=0;i<30;i++)
// ext_client_custom_character.data [i]=0;
 memset(ext_client_custom_character.data, 0, 30);  //内存空间初始化，避免显示乱码

 memcpy(ext_client_custom_character.data,chassis_state ,10);
 memcpy(&ext_client_custom_character.data[10],gate_state ,10);
 memcpy(&ext_client_custom_character.data[20],aim_state ,11);

 memcpy(client_data,&ext_client_custom_character ,sizeof(ext_client_custom_character));
 
 Append_CRC8_Check_Sum(client_data,5);
 Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_character));
 HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_character));
}

void client_send_data0_1()  //更新UI中的底盘、弹仓、自瞄等信息
{
 static uint8_t client_data[120];
 static ext_client_custom_character_t       ext_client_custom_character;
 static char chassis_state[10]={0};
 static char gate_state[10]={0};
 static char aim_state[11]={0};
 
 if (robot .chassis_gimbal ==chassis_follow )
  memcpy(chassis_state,"DP:       ",10);
 else if(robot .chassis_gimbal ==chassis_rotate )
  memcpy(chassis_state,"DP:       ",10);
 else
  memcpy(chassis_state,"DP:       ",10);
 
 if (robot .gate_state ==gate_on)
  memcpy(gate_state,"DC:       ",10);
 else 
  memcpy(gate_state,"DC:       ",10);
 
 if (robot .aiming_state ==armor_aim)
  memcpy(aim_state,"AIM:       ",11);
 else if (robot .aiming_state ==bbuff_aim )
  memcpy(aim_state,"AIM:       ",11);
 else if (robot .aiming_state ==sbuff_aim )//pred_aim
  memcpy(aim_state,"AIM:       ",11);
 else 
  memcpy(aim_state,"AIM:       ",11);
 
 ext_client_custom_character.frame_head .SOF =0xA5;
 ext_client_custom_character.frame_head .data_length =51;
 ext_client_custom_character.frame_head .seq =0;
 ext_client_custom_character.cmd_id =0x0301;
 ext_client_custom_character.student_interactive_header_data .data_cmd_id =0x0110;
 ext_client_custom_character.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_character.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_character.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_character.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_character.grapic_data_struct .graphic_name [2] =0x02;
 ext_client_custom_character.grapic_data_struct .operate_tpye =2;  //修改
 ext_client_custom_character.grapic_data_struct .graphic_tpye =7;  //字符
 ext_client_custom_character.grapic_data_struct .layer =0;
 ext_client_custom_character.grapic_data_struct .color =4;
 ext_client_custom_character.grapic_data_struct .start_angle =25;
 ext_client_custom_character.grapic_data_struct .end_angle =30;
 ext_client_custom_character.grapic_data_struct .width =3;
 ext_client_custom_character.grapic_data_struct .start_x =625;
 ext_client_custom_character.grapic_data_struct .start_y =840;
 ext_client_custom_character.grapic_data_struct .radius =0;
 ext_client_custom_character.grapic_data_struct .end_x =0;
 ext_client_custom_character.grapic_data_struct .end_y =0;
// for (uint8_t i=0;i<30;i++)
// ext_client_custom_character.data [i]=0;
 memset(ext_client_custom_character.data, 0, 30);  //内存空间初始化，避免显示乱码

 memcpy(ext_client_custom_character.data,chassis_state ,10);
 memcpy(&ext_client_custom_character.data[10],gate_state ,10);
 memcpy(&ext_client_custom_character.data[20],aim_state ,11);

 memcpy(client_data,&ext_client_custom_character ,sizeof(ext_client_custom_character));
 
 Append_CRC8_Check_Sum(client_data,5);
 Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_character));
 HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_character));
}

void client_send_data1()  //UI端新建，显示超级电容电压值
{
 static uint8_t client_data[120];
 static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single;

 ext_client_custom_graphic_single.frame_head .SOF =0xA5;
 ext_client_custom_graphic_single.frame_head .data_length =21;
 ext_client_custom_graphic_single.frame_head .seq =0;
 ext_client_custom_graphic_single.cmd_id =0x0301;
 ext_client_custom_graphic_single.student_interactive_header_data .data_cmd_id =0x0101;
 ext_client_custom_graphic_single.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_graphic_single.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [2] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .operate_tpye =1;  //增加
 ext_client_custom_graphic_single.grapic_data_struct .graphic_tpye =0;  //直线
 ext_client_custom_graphic_single.grapic_data_struct .layer =1;
 
 if (powerctrl_board.cap_vot >2000)
  ext_client_custom_graphic_single.grapic_data_struct .color =2;
 else
  ext_client_custom_graphic_single.grapic_data_struct .color =3;
 
 ext_client_custom_graphic_single.grapic_data_struct .start_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .width =20;
 ext_client_custom_graphic_single.grapic_data_struct .start_x =1500;
 ext_client_custom_graphic_single.grapic_data_struct .start_y =300; //900
 ext_client_custom_graphic_single.grapic_data_struct .radius =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_x =1500;
 ext_client_custom_graphic_single.grapic_data_struct .end_y =300 +(powerctrl_board.cap_vot-1400) *0.6;
 
 if (ext_client_custom_graphic_single.grapic_data_struct .end_y<200)
  ext_client_custom_graphic_single.grapic_data_struct .end_y=200;
 
 memcpy(client_data,&ext_client_custom_graphic_single ,sizeof(ext_client_custom_graphic_single));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single));	
}

void client_send_data1_1()  //UI端修改，实时显示超级电容电压值
{
 static uint8_t client_data[120];
 static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single;

 
 ext_client_custom_graphic_single.frame_head .SOF =0xA5;
 ext_client_custom_graphic_single.frame_head .data_length =21;
 ext_client_custom_graphic_single.frame_head .seq =0;
 ext_client_custom_graphic_single.cmd_id =0x0301;
 ext_client_custom_graphic_single.student_interactive_header_data .data_cmd_id =0x0101;
 ext_client_custom_graphic_single.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_graphic_single.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [2] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .operate_tpye =2;  //修改
 ext_client_custom_graphic_single.grapic_data_struct .graphic_tpye =0;  //直线
 ext_client_custom_graphic_single.grapic_data_struct .layer =1;
 
 if (powerctrl_board.cap_vot >2000)
  ext_client_custom_graphic_single.grapic_data_struct .color =2;
 else
  ext_client_custom_graphic_single.grapic_data_struct .color =3;
 
 ext_client_custom_graphic_single.grapic_data_struct .start_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .width =20;
 ext_client_custom_graphic_single.grapic_data_struct .start_x =1500;
 ext_client_custom_graphic_single.grapic_data_struct .start_y =300; //900
 ext_client_custom_graphic_single.grapic_data_struct .radius =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_x =1500;
 ext_client_custom_graphic_single.grapic_data_struct .end_y =300 +(powerctrl_board.cap_vot-1400)*0.6;
 
 if (ext_client_custom_graphic_single.grapic_data_struct .end_y<200)
  ext_client_custom_graphic_single.grapic_data_struct .end_y=200;
 
 memcpy(client_data,&ext_client_custom_graphic_single ,sizeof(ext_client_custom_graphic_single));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single));	
}

void client_send_data2()  //UI端新建，显示底盘、弹仓、自瞄等信息
{
 static uint8_t client_data[120];
 static ext_client_custom_character_t  ext_client_custom_character;
 static char chassis_state[10]={0};
 static char gate_state[10]={0};
 static char aim_state[11]={0};
 
 if (robot .chassis_gimbal ==chassis_follow )
  memcpy(chassis_state,"   FOLLOW ",10);
 else if(robot .chassis_gimbal ==chassis_rotate )
  memcpy(chassis_state,"   ROTATE ",10);
 else
  memcpy(chassis_state,"   LOCK   ",10);
 
 if (robot .gate_state ==gate_on)
  memcpy(gate_state,"   OPEN   ",10);
 else 
  memcpy(gate_state,"   CLOSE  ",10);
 
 if (robot .aiming_state ==armor_aim)
  memcpy(aim_state,"    ARMOUR ",11);
 else if (robot .aiming_state ==bbuff_aim )
  memcpy(aim_state,"    B-BUFF ",11);
 else if (robot .aiming_state ==sbuff_aim )//pred_aim
  memcpy(aim_state,"    S-BUFF ",11);
 else 
  memcpy(aim_state,"    NO-AIM ",11);
 
 ext_client_custom_character.frame_head .SOF =0xA5;
 ext_client_custom_character.frame_head .data_length =51;
 ext_client_custom_character.frame_head .seq =0;
 ext_client_custom_character.cmd_id =0x0301;
 ext_client_custom_character.student_interactive_header_data .data_cmd_id =0x0110;
 ext_client_custom_character.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_character.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_character.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_character.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_character.grapic_data_struct .graphic_name [2] =0x02;
 ext_client_custom_character.grapic_data_struct .operate_tpye =1;  //增加
 ext_client_custom_character.grapic_data_struct .graphic_tpye =7;  //字符
 ext_client_custom_character.grapic_data_struct .layer =2;
 ext_client_custom_character.grapic_data_struct .color =2;
 ext_client_custom_character.grapic_data_struct .start_angle =25;
 ext_client_custom_character.grapic_data_struct .end_angle =30;
 ext_client_custom_character.grapic_data_struct .width =3;
 ext_client_custom_character.grapic_data_struct .start_x =625;
 ext_client_custom_character.grapic_data_struct .start_y =840;
 ext_client_custom_character.grapic_data_struct .radius =0;
 ext_client_custom_character.grapic_data_struct .end_x =0;
 ext_client_custom_character.grapic_data_struct .end_y =0;
 for (uint8_t i=0;i<30;i++)
  ext_client_custom_character.data [i]=0;
 memcpy(ext_client_custom_character.data,chassis_state ,10);
 memcpy(&ext_client_custom_character.data[10],gate_state ,10);
 memcpy(&ext_client_custom_character.data[20],aim_state ,11);

 memcpy(client_data,&ext_client_custom_character ,sizeof(ext_client_custom_character));
 
 Append_CRC8_Check_Sum(client_data,5);
 Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_character));
 HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_character));

}

void client_send_data2_1()  //UI端修改，修改底盘、弹仓、自瞄信息
{
 static uint8_t client_data[120];
 static ext_client_custom_character_t       ext_client_custom_character;
 static char chassis_state[10]={0};
 static char gate_state[10]={0};
 static char aim_state[11]={0};
 
 if (robot .chassis_gimbal ==chassis_follow )
  memcpy(chassis_state,"   FOLLOW ",10);
 else if(robot .chassis_gimbal ==chassis_rotate )
  memcpy(chassis_state,"   ROTATE ",10);
 else
  memcpy(chassis_state,"   LOCK   ",10);
 
 if (robot .gate_state ==gate_on)
  memcpy(gate_state,"   OPEN   ",10);
 else 
  memcpy(gate_state,"   CLOSE  ",10);
 
 if (robot .aiming_state ==armor_aim)
  memcpy(aim_state,"    ARMOUR ",11);
 else if (robot .aiming_state ==bbuff_aim )
  memcpy(aim_state,"    B-BUFF ",11);
 else if (robot .aiming_state ==sbuff_aim )//pred_aim
  memcpy(aim_state,"    S-BUFF ",11);
 else 
  memcpy(aim_state,"    NO-AIM ",11);
 
 ext_client_custom_character.frame_head .SOF =0xA5;
 ext_client_custom_character.frame_head .data_length =51;
 ext_client_custom_character.frame_head .seq =0;
 ext_client_custom_character.cmd_id =0x0301;
 ext_client_custom_character.student_interactive_header_data .data_cmd_id =0x0110;
 ext_client_custom_character.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_character.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_character.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_character.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_character.grapic_data_struct .graphic_name [2] =0x02;
 ext_client_custom_character.grapic_data_struct .operate_tpye =2;  //修改
 ext_client_custom_character.grapic_data_struct .graphic_tpye =7;  //字符
 ext_client_custom_character.grapic_data_struct .layer =2;
 ext_client_custom_character.grapic_data_struct .color =2;
 ext_client_custom_character.grapic_data_struct .start_angle =25;
 ext_client_custom_character.grapic_data_struct .end_angle =30;
 ext_client_custom_character.grapic_data_struct .width =3;
 ext_client_custom_character.grapic_data_struct .start_x =625;
 ext_client_custom_character.grapic_data_struct .start_y =840;
 ext_client_custom_character.grapic_data_struct .radius =0;
 ext_client_custom_character.grapic_data_struct .end_x =0;
 ext_client_custom_character.grapic_data_struct .end_y =0;
 for (uint8_t i=0;i<30;i++)
  ext_client_custom_character.data [i]=0;
 memcpy(ext_client_custom_character.data,chassis_state ,10);
 memcpy(&ext_client_custom_character.data[10],gate_state ,10);
 memcpy(&ext_client_custom_character.data[20],aim_state ,11);

 memcpy(client_data,&ext_client_custom_character ,sizeof(ext_client_custom_character));
 
 Append_CRC8_Check_Sum(client_data,5);
 Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_character));
 HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_character));

}

void client_send_data3()  //UI增加，一条白色线段？？
{
 static uint8_t client_data[120];
 static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single;

 ext_client_custom_graphic_single.frame_head .SOF =0xA5;
 ext_client_custom_graphic_single.frame_head .data_length =21;
 ext_client_custom_graphic_single.frame_head .seq =0;
 ext_client_custom_graphic_single.cmd_id =0x0301;
 ext_client_custom_graphic_single.student_interactive_header_data .data_cmd_id =0x0101;
 ext_client_custom_graphic_single.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_graphic_single.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [2] =0x03;
 ext_client_custom_graphic_single.grapic_data_struct .operate_tpye =1;  //增加
 ext_client_custom_graphic_single.grapic_data_struct .graphic_tpye =0;  //直线
 ext_client_custom_graphic_single.grapic_data_struct .layer =3;
 ext_client_custom_graphic_single.grapic_data_struct .color =8;  //白色
 ext_client_custom_graphic_single.grapic_data_struct .start_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .width =4;
 ext_client_custom_graphic_single.grapic_data_struct .start_x =510;
 ext_client_custom_graphic_single.grapic_data_struct .start_y =0; 
 ext_client_custom_graphic_single.grapic_data_struct .radius =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_x =860;
 ext_client_custom_graphic_single.grapic_data_struct .end_y =428;

 memcpy(client_data,&ext_client_custom_graphic_single ,sizeof(ext_client_custom_graphic_single));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single));	
}

void client_send_data3_1()  //UI修改，修改上面那条白色线段？？
{
 static uint8_t client_data[120];
 static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single;

 
 ext_client_custom_graphic_single.frame_head .SOF =0xA5;
 ext_client_custom_graphic_single.frame_head .data_length =21;
 ext_client_custom_graphic_single.frame_head .seq =0;
 ext_client_custom_graphic_single.cmd_id =0x0301;
 ext_client_custom_graphic_single.student_interactive_header_data .data_cmd_id =0x0101;
 ext_client_custom_graphic_single.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_graphic_single.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [2] =0x03;
 ext_client_custom_graphic_single.grapic_data_struct .operate_tpye =2;  //修改
 ext_client_custom_graphic_single.grapic_data_struct .graphic_tpye =0;  //直线
 ext_client_custom_graphic_single.grapic_data_struct .layer =3;
 ext_client_custom_graphic_single.grapic_data_struct .color =8;  //白色
 ext_client_custom_graphic_single.grapic_data_struct .start_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .width =4;
 ext_client_custom_graphic_single.grapic_data_struct .start_x =510;
 ext_client_custom_graphic_single.grapic_data_struct .start_y =0; 
 ext_client_custom_graphic_single.grapic_data_struct .radius =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_x =860;
 ext_client_custom_graphic_single.grapic_data_struct .end_y =428;

 memcpy(client_data,&ext_client_custom_graphic_single ,sizeof(ext_client_custom_graphic_single));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single));	
}

void client_send_data4()  //UI新建，增加一条白色线段
{
 static uint8_t client_data[120];
 static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single;

 ext_client_custom_graphic_single.frame_head .SOF =0xA5;
 ext_client_custom_graphic_single.frame_head .data_length =21;
 ext_client_custom_graphic_single.frame_head .seq =0;
 ext_client_custom_graphic_single.cmd_id =0x0301;
 ext_client_custom_graphic_single.student_interactive_header_data .data_cmd_id =0x0101;
 ext_client_custom_graphic_single.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_graphic_single.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [2] =0x04;
 ext_client_custom_graphic_single.grapic_data_struct .operate_tpye =1;  //增加
 ext_client_custom_graphic_single.grapic_data_struct .graphic_tpye =0;  //直线
 ext_client_custom_graphic_single.grapic_data_struct .layer =4;
 ext_client_custom_graphic_single.grapic_data_struct .color =8;  //白色
 ext_client_custom_graphic_single.grapic_data_struct .start_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .width =4;
 ext_client_custom_graphic_single.grapic_data_struct .start_x =1410;
 ext_client_custom_graphic_single.grapic_data_struct .start_y =0; 
 ext_client_custom_graphic_single.grapic_data_struct .radius =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_x =1060;
 ext_client_custom_graphic_single.grapic_data_struct .end_y =428;

 memcpy(client_data,&ext_client_custom_graphic_single ,sizeof(ext_client_custom_graphic_single));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single));	
}

void client_send_data4_1()  //UI修改，修改上面那条白色线段
{
 static uint8_t client_data[120];
 static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single;

 
 ext_client_custom_graphic_single.frame_head .SOF =0xA5;
 ext_client_custom_graphic_single.frame_head .data_length =21;
 ext_client_custom_graphic_single.frame_head .seq =0;
 ext_client_custom_graphic_single.cmd_id =0x0301;
 ext_client_custom_graphic_single.student_interactive_header_data .data_cmd_id =0x0101;
 ext_client_custom_graphic_single.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
 ext_client_custom_graphic_single.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [0] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [1] =0x01;
 ext_client_custom_graphic_single.grapic_data_struct .graphic_name [2] =0x04;
 ext_client_custom_graphic_single.grapic_data_struct .operate_tpye =2;  //修改
 ext_client_custom_graphic_single.grapic_data_struct .graphic_tpye =0;  //直线
 ext_client_custom_graphic_single.grapic_data_struct .layer =4;
 ext_client_custom_graphic_single.grapic_data_struct .color =8;  //白色
 ext_client_custom_graphic_single.grapic_data_struct .start_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_angle =0;
 ext_client_custom_graphic_single.grapic_data_struct .width =4;
 ext_client_custom_graphic_single.grapic_data_struct .start_x =1410;
 ext_client_custom_graphic_single.grapic_data_struct .start_y =0; 
 ext_client_custom_graphic_single.grapic_data_struct .radius =0;
 ext_client_custom_graphic_single.grapic_data_struct .end_x =1060;
 ext_client_custom_graphic_single.grapic_data_struct .end_y =428;

 memcpy(client_data,&ext_client_custom_graphic_single ,sizeof(ext_client_custom_graphic_single));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single));	
}

void client_send_data5()  //UI新建，增加两条橙色直线
{
	static uint8_t client_data[120];
	static ext_client_custom_graphic_double_t  ext_client_custom_graphic_single5;

	ext_client_custom_graphic_single5.frame_head .SOF =0xA5;
	ext_client_custom_graphic_single5.frame_head .data_length =36;
	ext_client_custom_graphic_single5.frame_head .seq =0;
	ext_client_custom_graphic_single5.cmd_id =0x0301;
	ext_client_custom_graphic_single5.student_interactive_header_data .data_cmd_id =0x0102;
	ext_client_custom_graphic_single5.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
	ext_client_custom_graphic_single5.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
	ext_client_custom_graphic_single5.grapic_data_struct[0] .graphic_name [0] =0x07;
	ext_client_custom_graphic_single5.grapic_data_struct[0] .graphic_name [1] =0x08;
	ext_client_custom_graphic_single5.grapic_data_struct [0].graphic_name [2] =0x09;
	ext_client_custom_graphic_single5.grapic_data_struct [0].operate_tpye =1;  //增加
	ext_client_custom_graphic_single5.grapic_data_struct [0].graphic_tpye =0;  //直线
	ext_client_custom_graphic_single5.grapic_data_struct [0].layer =0;
	ext_client_custom_graphic_single5.grapic_data_struct [0].color =3;  //橙色
	ext_client_custom_graphic_single5.grapic_data_struct [0].start_angle =0;
	ext_client_custom_graphic_single5.grapic_data_struct [0].end_angle =0;
	ext_client_custom_graphic_single5.grapic_data_struct [0].width =2;
	ext_client_custom_graphic_single5.grapic_data_struct [0].start_x =930;
	ext_client_custom_graphic_single5.grapic_data_struct [0].start_y =490;
	ext_client_custom_graphic_single5.grapic_data_struct [0].radius =0;
	ext_client_custom_graphic_single5.grapic_data_struct [0].end_x =990;
	ext_client_custom_graphic_single5.grapic_data_struct [0].end_y =490;//hengxian
	
	ext_client_custom_graphic_single5.grapic_data_struct[1] .graphic_name [0] =0x11;
	ext_client_custom_graphic_single5.grapic_data_struct [1].graphic_name [1] =0x12;
	ext_client_custom_graphic_single5.grapic_data_struct [1].graphic_name [2] =0x13;
	ext_client_custom_graphic_single5.grapic_data_struct [1].operate_tpye =1;
	ext_client_custom_graphic_single5.grapic_data_struct [1].graphic_tpye =0;
	ext_client_custom_graphic_single5.grapic_data_struct [1].layer =0;
	ext_client_custom_graphic_single5.grapic_data_struct [1].color =3;
	ext_client_custom_graphic_single5.grapic_data_struct [1].start_angle =0;
	ext_client_custom_graphic_single5.grapic_data_struct [1].end_angle =0;
	ext_client_custom_graphic_single5.grapic_data_struct [1].width =2;
	ext_client_custom_graphic_single5.grapic_data_struct [1].start_x =950;
	ext_client_custom_graphic_single5.grapic_data_struct [1].start_y = 440;
	ext_client_custom_graphic_single5.grapic_data_struct [1].radius =0;
	ext_client_custom_graphic_single5.grapic_data_struct [1].end_x =970;
	ext_client_custom_graphic_single5.grapic_data_struct [1].end_y =440;//hengxian
	
	memcpy(client_data,&ext_client_custom_graphic_single5 ,sizeof(ext_client_custom_graphic_single5));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single5));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single5));
}

void robots_send_data() //机器人间通信
{
	static robot_interactive_data_t robots;
	static uint8_t robots_data[20];
	
	robots.frame_head.SOF =0xA5;
	robots.frame_head.data_length =7;
	robots.frame_head.seq =0;
	robots.cmd_id =0x0301;
	robots.student_interactive_header_data.data_cmd_id =0x0200;
	robots.student_interactive_header_data.sender_ID =game_robot_status .robot_id;
	/*自己的id + 某个数字 = 其他机器人的id*/
	robots.student_interactive_header_data.receiver_ID =game_robot_status .robot_id+6;
	/*通信数据从1开始,第几个功能发数字几*/
	robots.data =7;
	
	memcpy(robots_data,&robots ,sizeof(robots));
	
	Append_CRC8_Check_Sum(robots_data, 5);
	Append_CRC16_Check_Sum(robots_data, sizeof(robots));
	HAL_UART_Transmit_DMA (&REFEREE_HUART,robots_data,sizeof(robots));
}


void client_send_data6()  //UI新建，一个白色的圆
{
	static uint8_t client_data[120];
	static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single6;

	ext_client_custom_graphic_single6.frame_head .SOF =0xA5;
	ext_client_custom_graphic_single6.frame_head .data_length =21;
	ext_client_custom_graphic_single6.frame_head .seq =0;
	ext_client_custom_graphic_single6.cmd_id =0x0301;
	ext_client_custom_graphic_single6.student_interactive_header_data .data_cmd_id =0x0101;
	ext_client_custom_graphic_single6.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
	ext_client_custom_graphic_single6.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
	ext_client_custom_graphic_single6.grapic_data_struct.graphic_name [0] =0x06;
	ext_client_custom_graphic_single6.grapic_data_struct.graphic_name [1] =0x06;
	ext_client_custom_graphic_single6.grapic_data_struct.graphic_name [2] =0x06;
	ext_client_custom_graphic_single6.grapic_data_struct.operate_tpye =1;  //增加
	ext_client_custom_graphic_single6.grapic_data_struct.graphic_tpye =2;  //圆
	ext_client_custom_graphic_single6.grapic_data_struct.layer =6;
	ext_client_custom_graphic_single6.grapic_data_struct.color =8;  //白色
	ext_client_custom_graphic_single6.grapic_data_struct.width =4;
	ext_client_custom_graphic_single6.grapic_data_struct.start_x =960;
	ext_client_custom_graphic_single6.grapic_data_struct.start_y =540;
	ext_client_custom_graphic_single6.grapic_data_struct.radius =60;
	
	memcpy(client_data,&ext_client_custom_graphic_single6 ,sizeof(ext_client_custom_graphic_single6));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single6));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single6));
}

void client_send_data6_1()  //UI修改，根据底盘模式修改上面那个圆的颜色
{
	static uint8_t client_data[120];
	static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single6_1;

	ext_client_custom_graphic_single6_1.frame_head .SOF =0xA5;
	ext_client_custom_graphic_single6_1.frame_head .data_length =21;
	ext_client_custom_graphic_single6_1.frame_head .seq =0;
	ext_client_custom_graphic_single6_1.cmd_id =0x0301;
	ext_client_custom_graphic_single6_1.student_interactive_header_data .data_cmd_id =0x0101;
	ext_client_custom_graphic_single6_1.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
	ext_client_custom_graphic_single6_1.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
	ext_client_custom_graphic_single6_1.grapic_data_struct.graphic_name [0] =0x06;
	ext_client_custom_graphic_single6_1.grapic_data_struct.graphic_name [1] =0x06;
	ext_client_custom_graphic_single6_1.grapic_data_struct.graphic_name [2] =0x06;
	ext_client_custom_graphic_single6_1.grapic_data_struct.operate_tpye =2;  //修改
	ext_client_custom_graphic_single6_1.grapic_data_struct.graphic_tpye =2;  //圆
	ext_client_custom_graphic_single6_1.grapic_data_struct.layer =6;

	if (robot .chassis_gimbal ==chassis_follow )
		ext_client_custom_graphic_single6_1.grapic_data_struct.color =8;
	else if(robot .chassis_gimbal ==chassis_rotate )
		ext_client_custom_graphic_single6_1.grapic_data_struct.color =0;
	
	ext_client_custom_graphic_single6_1.grapic_data_struct.width =4;
	ext_client_custom_graphic_single6_1.grapic_data_struct.start_x =960;
	ext_client_custom_graphic_single6_1.grapic_data_struct.start_y =540;
	ext_client_custom_graphic_single6_1.grapic_data_struct.radius =60;
	
	memcpy(client_data,&ext_client_custom_graphic_single6_1 ,sizeof(ext_client_custom_graphic_single6_1));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single6_1));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single6_1));
}

float temp_angle;
void client_send_data7()  //UI增加，根据底盘和云台的相对姿态，在准心处绘制旋转的直线
{
	static uint8_t client_data[120];
	static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single7;

	float temp_angle=axes_angle;
	
	ext_client_custom_graphic_single7.frame_head .SOF =0xA5;
	ext_client_custom_graphic_single7.frame_head .data_length =21;
	ext_client_custom_graphic_single7.frame_head .seq =0;
	ext_client_custom_graphic_single7.cmd_id =0x0301;
	ext_client_custom_graphic_single7.student_interactive_header_data .data_cmd_id =0x0101;
	ext_client_custom_graphic_single7.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
	ext_client_custom_graphic_single7.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
	ext_client_custom_graphic_single7.grapic_data_struct.graphic_name [0] =0x06;
	ext_client_custom_graphic_single7.grapic_data_struct.graphic_name [1] =0x06;
	ext_client_custom_graphic_single7.grapic_data_struct.graphic_name [2] =0x07;
	ext_client_custom_graphic_single7.grapic_data_struct.operate_tpye =1;  //增加
	ext_client_custom_graphic_single7.grapic_data_struct.graphic_tpye =0;  //直线
	ext_client_custom_graphic_single7.grapic_data_struct.layer =7;
	ext_client_custom_graphic_single7.grapic_data_struct.color =8;
	ext_client_custom_graphic_single7.grapic_data_struct.width =4;
	ext_client_custom_graphic_single7.grapic_data_struct.start_x =960+30*sin(temp_angle);
	ext_client_custom_graphic_single7.grapic_data_struct.start_y =540+30*cos(temp_angle);
	ext_client_custom_graphic_single7.grapic_data_struct.end_x =960+30*sin(temp_angle)+30*sin(temp_angle);
	ext_client_custom_graphic_single7.grapic_data_struct.end_y =540+30*cos(temp_angle)+30*cos(temp_angle);
	
	memcpy(client_data,&ext_client_custom_graphic_single7 ,sizeof(ext_client_custom_graphic_single7));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single7));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single7));
}

void client_send_data7_1()  //
{
	static uint8_t client_data[120];
	static ext_client_custom_graphic_single_t  ext_client_custom_graphic_single7_1;

	temp_angle = -(gimbal_yaw.angle -CHASSIS_FOLLOW_CENTER -8192.0f)/8192.0f *2.0f *PI;
	
	ext_client_custom_graphic_single7_1.frame_head .SOF =0xA5;
	ext_client_custom_graphic_single7_1.frame_head .data_length =21;
	ext_client_custom_graphic_single7_1.frame_head .seq =0;
	ext_client_custom_graphic_single7_1.cmd_id =0x0301;
	ext_client_custom_graphic_single7_1.student_interactive_header_data .data_cmd_id =0x0101;
	ext_client_custom_graphic_single7_1.student_interactive_header_data .sender_ID =game_robot_status .robot_id;
	ext_client_custom_graphic_single7_1.student_interactive_header_data .receiver_ID =game_robot_status .robot_id+256;
	ext_client_custom_graphic_single7_1.grapic_data_struct.graphic_name [0] =0x06;
	ext_client_custom_graphic_single7_1.grapic_data_struct.graphic_name [1] =0x06;
	ext_client_custom_graphic_single7_1.grapic_data_struct.graphic_name [2] =0x07;
	ext_client_custom_graphic_single7_1.grapic_data_struct.operate_tpye =2;  //修改
	ext_client_custom_graphic_single7_1.grapic_data_struct.graphic_tpye =0;  //直线
	ext_client_custom_graphic_single7_1.grapic_data_struct.layer =7;

	if (robot .chassis_gimbal ==chassis_follow )
		ext_client_custom_graphic_single7_1.grapic_data_struct.color =8;
	else if(robot .chassis_gimbal ==chassis_rotate )
	{
		ext_client_custom_graphic_single7_1.grapic_data_struct.color =0;
		temp_angle =-temp_angle;
	}
	
	ext_client_custom_graphic_single7_1.grapic_data_struct.width =4;
	ext_client_custom_graphic_single7_1.grapic_data_struct.start_x =960+30*sin(temp_angle);
	ext_client_custom_graphic_single7_1.grapic_data_struct.start_y =540+30*cos(temp_angle);

	ext_client_custom_graphic_single7_1.grapic_data_struct.end_x =960+30*sin(temp_angle)+30*sin(temp_angle);
	ext_client_custom_graphic_single7_1.grapic_data_struct.end_y =540+30*cos(temp_angle)+30*cos(temp_angle);
	
	memcpy(client_data,&ext_client_custom_graphic_single7_1 ,sizeof(ext_client_custom_graphic_single7_1));

	Append_CRC8_Check_Sum(client_data,5);
	Append_CRC16_Check_Sum(client_data,sizeof(ext_client_custom_graphic_single7_1));
	HAL_UART_Transmit_DMA(&REFEREE_HUART,client_data,sizeof(ext_client_custom_graphic_single7_1));
}

void client_send_task(void)  //操作手客户端绘图
{
	static uint16_t add_cnt =0, change_cnt =0;
	//客户端数据发送
	add_cnt++;
	change_cnt++;
	//修改
	if (add_cnt == 5)
		client_send_data0_1();  
	else if (add_cnt == 10)
		client_send_data1_1();
	else if (add_cnt == 15)
		client_send_data2_1();
	else if (add_cnt == 20)
		client_send_data3_1();
	else if (add_cnt == 25)
		client_send_data4_1();
	else if (add_cnt == 30)
		client_send_data6_1();
	else if(change_cnt == 35)
	{
		client_send_data7_1();
		add_cnt =0;
	}
	//增加
	if (change_cnt == 500)
		client_send_data0();  
	else if (change_cnt == 1000)
		client_send_data1();
	else if (change_cnt == 1500)
		client_send_data2();
	else if (change_cnt == 2000)
		client_send_data3();
	else if (change_cnt == 2500)
		client_send_data4();
	else if (change_cnt == 3000)
		client_send_data6();
	else if(change_cnt == 3500)
	{
		client_send_data7();
		change_cnt =0;
	}
}
