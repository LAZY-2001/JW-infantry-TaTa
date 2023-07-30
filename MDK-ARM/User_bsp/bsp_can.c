/**
  ******************************************************************************
  * @file    bsp_can.c
  * @author  Ao Buhe 
  * @version V1.0.0
  * @date    2020/2/11
  * @brief    
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
	
#include "includes.h"
uint32_t alive_timer=0;
uint32_t tick_record = 0;
int16_t source = 0;
extern float ammo_delta_angle;
extern float angle_record;
CAN_TxHeaderTypeDef  can1_tx_msg;
CAN_TxHeaderTypeDef  can2_tx_msg;

CAN_RxHeaderTypeDef  can1_rx_msg;
CAN_RxHeaderTypeDef  can2_rx_msg;

can_rx_msg  rx_msg;
uint8_t chassis_info[8]={0};

exchange_info_t exchange_info;

moto_measure_t  chassis_LF;
moto_measure_t  chassis_RF;
moto_measure_t  chassis_RB;
moto_measure_t  chassis_LB;
moto_measure_t  gimbal_yaw;
moto_measure_t  gimbal_pitch;
moto_measure_t  fric_left;
moto_measure_t  fric_right;
moto_measure_t  ammo_booster;
moto_measure_t  gate_moto;

powerctrl_measure_t powerctrl_board;


/*******************************************************************************************
  * @Func		can_filter_config(void)
  * @Brief    CAN1 CAN2  过滤器配置
  * @Param		void
  * @Retval		None
  * @Date     2020/2/12
 *******************************************************************************************/
void can_filter_config(void)
{
	CAN_FilterTypeDef sCan1FilterConfig;
	CAN_FilterTypeDef sCan2FilterConfig;	

	sCan1FilterConfig.FilterIdHigh         = 0x0000;			
	sCan1FilterConfig.FilterIdLow          = 0x0000; 
	sCan1FilterConfig.FilterMaskIdHigh     = 0x0000;			
	sCan1FilterConfig.FilterMaskIdLow      = 0x0000;			
	sCan1FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;        
	sCan1FilterConfig.FilterBank=0;
	sCan1FilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;
	sCan1FilterConfig.FilterMode=CAN_FILTERMODE_IDMASK;
	sCan1FilterConfig.FilterActivation = ENABLE;         
	sCan1FilterConfig.SlaveStartFilterBank = 14;
	
	if(HAL_CAN_ConfigFilter(&hcan1, &sCan1FilterConfig)!=HAL_OK)
	{			 
		Error_Handler();
	} 
	if(HAL_CAN_Start(&hcan1)!=HAL_OK)
	{
		Error_Handler(); 
	}
	if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
	{
		Error_Handler(); 
	}
	  
	sCan2FilterConfig.FilterIdHigh         = 0x0000;		
	sCan2FilterConfig.FilterIdLow          = 0x0000; 
	sCan2FilterConfig.FilterMaskIdHigh     = 0x0000;		
	sCan2FilterConfig.FilterMaskIdLow      = 0x0000;			
	sCan2FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;          
	sCan2FilterConfig.FilterBank=14;
	sCan2FilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;
	sCan2FilterConfig.FilterMode=CAN_FILTERMODE_IDMASK;
	sCan2FilterConfig.FilterActivation = ENABLE;         
	sCan2FilterConfig.SlaveStartFilterBank = 14;

	if(HAL_CAN_ConfigFilter(&hcan2, &sCan2FilterConfig)!=HAL_OK)
	{
		Error_Handler(); 
	} 
	if(HAL_CAN_Start(&hcan2)!=HAL_OK)
	{
		Error_Handler(); 
	}
	if(HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO1_MSG_PENDING)!=HAL_OK)
	{
		Error_Handler(); 
	}

}


/*******************************************************************************************
  * @Func		  can_transmit_msg
  * @Brief    can 发送函数
  * @Param		@hcan  can1 or can2
  *           @StdId 发送目标id
  *           @tx_current_data  要发送的电流数据    0x200对应0-3  0x1ff对应4-7
  * @Retval		None
  * @Date     2020/2/12
 *******************************************************************************************/
void can_transmit_msg_one(CAN_HandleTypeDef *hcan, uint8_t tx_current_data_one[8])
{						 
	CAN_TxHeaderTypeDef  can_tx_msg;
  
  can_tx_msg.DLC =8;
  can_tx_msg.StdId =0x200;
  can_tx_msg.IDE =CAN_ID_STD;
  can_tx_msg.RTR =CAN_RTR_DATA;
	if (hcan->Instance ==CAN1)
	{
		if(HAL_CAN_AddTxMessage(hcan,&can_tx_msg,tx_current_data_one,(uint32_t*)CAN_TX_MAILBOX0)!=HAL_OK)
			Error_Handler(); 
	}
	else if (hcan->Instance ==CAN2)
	{
		if(HAL_CAN_AddTxMessage(hcan,&can_tx_msg,tx_current_data_one,(uint32_t*)CAN_TX_MAILBOX1)!=HAL_OK)
			Error_Handler(); 
	}
}


void can_transmit_msg_two(CAN_HandleTypeDef *hcan,uint8_t tx_current_data_two[8])
{						 
	CAN_TxHeaderTypeDef  can_tx_msg;
  
  can_tx_msg.DLC=8;
  can_tx_msg.StdId=0x1ff;
  can_tx_msg.IDE=CAN_ID_STD;
  can_tx_msg.RTR=CAN_RTR_DATA;
	
	if (hcan->Instance ==CAN1)
		 HAL_CAN_AddTxMessage(hcan,&can_tx_msg,tx_current_data_two,(uint32_t*)CAN_TX_MAILBOX0);
	else if (hcan->Instance ==CAN2)
		 HAL_CAN_AddTxMessage(hcan,&can_tx_msg,tx_current_data_two,(uint32_t*)CAN_TX_MAILBOX1);
}

void can_transmit_msg_three(CAN_HandleTypeDef *hcan, uint8_t tx_current_data_one[8])
{						 
	CAN_TxHeaderTypeDef  can_tx_msg;
  
  can_tx_msg.DLC=8;
  can_tx_msg.StdId=0x2ff;
  can_tx_msg.IDE=CAN_ID_STD;
  can_tx_msg.RTR=CAN_RTR_DATA;
	if (hcan->Instance ==CAN1)
	{
		if(HAL_CAN_AddTxMessage(hcan,&can_tx_msg,tx_current_data_one,(uint32_t*)CAN_TX_MAILBOX0)!=HAL_OK)
		 Error_Handler(); 
	}
	else if (hcan->Instance ==CAN2)
	{
		if(HAL_CAN_AddTxMessage(hcan,&can_tx_msg,tx_current_data_one,(uint32_t*)CAN_TX_MAILBOX1)!=HAL_OK)
		 Error_Handler(); 
	}
}
/*******************************************************************************************
  * @Func		  can_transmit_msg_four（实际用于发送功率控制板的数据）
  * @Brief    can 发送函数4
  * @Param		@hcan  can1 or can2（实际只用can1）
  *           @tx_power_data  要发送的目标功率数据
  * @Retval		None
  * @Date     2020/7/9
 *******************************************************************************************/
void can_transmit_msg_four(CAN_HandleTypeDef *hcan,uint8_t tx_power_data[8])
{						 
	CAN_TxHeaderTypeDef  can_tx_msg;
  
  can_tx_msg.DLC=8;
  can_tx_msg.StdId=0x210;
  can_tx_msg.IDE=CAN_ID_STD;
  can_tx_msg.RTR=CAN_RTR_DATA;
	if (hcan->Instance ==CAN1)
	{
		HAL_CAN_AddTxMessage(hcan,&can_tx_msg,tx_power_data,(uint32_t*)CAN_TX_MAILBOX0);
	}
	else if (hcan->Instance ==CAN2)
	{
		
	}
}

/*******************************************************************************************
  * @Func			void get_moto_measure(moto_measure_t *ptr, uint8_t Data[8])
  * @Brief    解算电机回传信息
  * @Param		
  * @Retval		None
  * @Date     2020/2/12
 *******************************************************************************************/
void get_moto_measure(moto_measure_t *ptr, uint8_t Data[8])
{
	ptr->last_angle = ptr->angle;
	ptr->angle = (uint16_t)(Data[0]<<8 | Data[1]) ;
	ptr->speed_rpm = (int16_t)(Data[2]<<8 |Data[3]);
	ptr->given_current = (int16_t)(Data[4]<<8 | Data[5]);
	ptr->temperature = Data[6];
}

/*******************************************************************************************
  * @Func			void get_powerctrl_measure(powerctrl_measure_t *ptr, uint8_t Data[8])
  * @Brief    解算功率控制板回传信息
  * @Param		
  * @Retval		None
  * @Date     2020/7/9
 *******************************************************************************************/
void get_powerctrl_measure(powerctrl_measure_t *ptr, uint8_t Data[8])
{
	ptr->input_vot    = (uint16_t)(Data[0] | Data[1]<<8);
	ptr->cap_vot      = (uint16_t)(Data[2] | Data[3]<<8);
	ptr->input_cur    = (uint16_t)(Data[4] | Data[5]<<8);
	ptr->target_power = (uint16_t)(Data[6] | Data[7]<<8);
}

void can1_data_process(can_rx_msg  rx_msg)
{
	static uint8_t first_flag=1;
	switch (rx_msg.StdId)
	{
		case CHASSIS_LF:
			get_moto_measure(&chassis_LF,rx_msg.rx_data );
			alive_timer =HAL_GetTick ();
			break;
		case CHASSIS_RF:
			get_moto_measure(&chassis_RF,rx_msg.rx_data );
			alive_timer =HAL_GetTick ();
			break;
		case CHASSIS_LB:
			get_moto_measure(&chassis_LB,rx_msg.rx_data);
			break;
		case CHASSIS_RB:
			get_moto_measure(&chassis_RB,rx_msg.rx_data);
			break;
		case AMMO_BOOSTER:
		{
			get_moto_measure(&ammo_booster,rx_msg.rx_data);
			ammo_angle_process();
			tick_record = HAL_GetTick();
			if (source == 0)
			{
				real_angle=0;
				target_angle=0;
				ammo_delta_angle=0;
				angle_record=0;
				source = 1;
			}
			if (first_flag ==1)
			{
				target_angle =real_angle;
				first_flag=0;
			}
			break;
		}
		case GIMBAL_YAW :
			get_moto_measure(&gimbal_yaw,rx_msg.rx_data);
			break;

		case GATE_MOTO :
			get_moto_measure(&gate_moto,rx_msg.rx_data);
			gate_angle_process();
			break;
		
		case POWERCTRL_BOARD :
			get_powerctrl_measure(&powerctrl_board, rx_msg.rx_data);
			break;		
	}		
}

void can2_data_process(can_rx_msg  rx_msg)
{
	switch (rx_msg.StdId)
	{
		case FRIC_LEFT :
			get_moto_measure(&fric_left,rx_msg.rx_data);
			break;
		case FRIC_RIGHT :
			get_moto_measure(&fric_right,rx_msg.rx_data);
			break;
		case GIMBAL_PITCH :
			get_moto_measure(&gimbal_pitch,rx_msg.rx_data);
			#ifdef INFANTRY_MIKEY
			gimbal_pitch.angle -=4000;
			if (gimbal_pitch.angle <0)
				gimbal_pitch.angle += 8191;
			#endif
			
			#ifdef INFANTRY_LEO
			gimbal_pitch.angle -=4000;
			if (gimbal_pitch.angle <0)
				gimbal_pitch.angle += 8191;
			#endif
			break;
	}		
	alive_timer =HAL_GetTick();
}

//装载目标数据，高位在前
void set_current(uint8_t Data[8], int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
	Data[0] = (iq1 >> 8)&0xff;
	Data[1] = iq1&0xff;
	Data[2] = (iq2 >> 8)&0xff;
  Data[3] = iq2&0xff;
	Data[4] = (iq3 >> 8)&0xff;
	Data[5] = iq3&0xff;
	Data[6] = (iq4 >> 8)&0xff;
	Data[7] = iq4&0xff;
	
}

//装载目标功率数据
void set_power(uint8_t Data[8], uint16_t power)
{
	Data[0] = (power >> 8) &0xff;
	Data[1] = power &0xff;
	Data[2] = 0;
  Data[3] = 0;
	Data[4] = 0;
	Data[5] = 0;
	Data[6] = 0;
	Data[7] = 0;
}

/*******************************************************************************************
  * @Func			void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
  * @Brief    回调函数  用于接收回传的信息
  * @Param		
  * @Retval		None
  * @Date     2020/2/12
 *******************************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) //can1使用FIFO 0缓存区
{
  if(hcan->Instance ==CAN1)
  {
    HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0, &can1_rx_msg,rx_msg.rx_data);
		rx_msg.StdId =can1_rx_msg.StdId ;
		can1_data_process(rx_msg);
  }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) //can2使用FIFO 1缓存区
{
	if(hcan->Instance ==CAN2)
  {
    HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO1,&can2_rx_msg,rx_msg.rx_data );
		rx_msg.StdId =can2_rx_msg.StdId;
		can2_data_process(rx_msg);
  }
}
