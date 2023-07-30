#include "string.h"
#include "stdlib.h"
#include "includes.h"

uint8_t  dbus_buf[DBUS_BUFLEN];
uint8_t  visual_buf[VISUAL_MAX_LEN];
uint8_t  newdata = 0;
uint8_t  test=0;
rc_info_t rc;
rc_info_t last_rc;

PCRecvPacketEnergy_t pc_energy;
PCRecvPacketArmor_t  pc_armor;
uint32_t  visual_timer=0;
/**
  * @brief      enable global uart it and do not use DMA transfer done it
  * @param[in]  huart: uart IRQHandler id
  * @param[in]  pData: receive buff 
  * @param[in]  Size:  buff size
  * @retval     set success or fail
  */
static int uart_receive_dma_no_it(UART_HandleTypeDef* huart, uint8_t* pData, uint32_t Size)
{
  uint32_t tmp1 = 0;

  tmp1 = huart->RxState;
	
	if (tmp1 == HAL_UART_STATE_READY)
	{
		if ((pData == NULL) || (Size == 0))
		{
			return HAL_ERROR;
		}

		huart->pRxBuffPtr = pData;
		huart->RxXferSize = Size;
		huart->ErrorCode  = HAL_UART_ERROR_NONE;

		/* Enable the DMA Stream */
		HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, (uint32_t)pData, Size);

		/* 
		 * Enable the DMA transfer for the receiver request by setting the DMAR bit
		 * in the UART CR3 register 
		 */
		SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
}

/**
  * @brief      returns the number of remaining data units in the current DMAy Streamx transfer.
  * @param[in]  dma_stream: where y can be 1 or 2 to select the DMA and x can be 0
  *             to 7 to select the DMA Stream.
  * @retval     The number of remaining data units in the current DMAy Streamx transfer.
  */
uint16_t dma_current_data_counter(DMA_Stream_TypeDef *dma_stream)
{
  /* Return the number of remaining data units for DMAy Streamx */
  return ((uint16_t)(dma_stream->NDTR));
}


/**
  * @brief       handle received rc data
  * @param[out]  rc:   structure to save handled rc data
  * @param[in]   buff: the buff which saved raw rc data
  * @retval 
  */
void rc_callback_handler(rc_info_t *rc, uint8_t *buff)
{
	
  rc->ch1 = (buff[0] | buff[1] << 8) & 0x07FF;
  rc->ch1 -= 1024;
  rc->ch2 = (buff[1] >> 3 | buff[2] << 5) & 0x07FF;
  rc->ch2 -= 1024;
  rc->ch3 = (buff[2] >> 6 | buff[3] << 2 | buff[4] << 10) & 0x07FF;
  rc->ch3 -= 1024;
  rc->ch4 = (buff[4] >> 1 | buff[5] << 7) & 0x07FF;
  rc->ch4 -= 1024;

  rc->sw1 = ((buff[5] >> 4) & 0x000C) >> 2;
  rc->sw2 = (buff[5] >> 4) & 0x0003;
  /* prevent remote control zero deviation */
  if(rc->ch1 <= 5 && rc->ch1 >= -5)
    rc->ch1 = 0;
  if(rc->ch2 <= 5 && rc->ch2 >= -5)
    rc->ch2 = 0;
  if(rc->ch3 <= 5 && rc->ch3 >= -5)
    rc->ch3 = 0;
  if(rc->ch4 <= 5 && rc->ch4 >= -5)
    rc->ch4 = 0;
  		
	rc->mouse.x = buff[6] | (buff[7] << 8); // x axis
  rc->mouse.y = buff[8] | (buff[9] << 8);
  rc->mouse.z = buff[10] | (buff[11] << 8);

  rc->mouse.l = buff[12];
  rc->mouse.r = buff[13];

  rc->kb.key_code = buff[14] | buff[15] << 8; // key borad code
  rc->wheel = (buff[16] | buff[17] << 8) - 1024;
	if ((abs(rc->ch1) > 660) || \
      (abs(rc->ch2) > 660) || \
      (abs(rc->ch3) > 660) || \
      (abs(rc->ch4) > 660))
	{
    	memset(rc, 0, sizeof(rc_info_t));
	}

	if (rc->sw2 ==2&&rc->sw1==2)
		robot.control_state =KB;
	else 
		robot.control_state = RC;
		last_robot =robot;
		aim_state();
		chassis_gimbal_state();
		gate_state();
		fric_state();
		shoot_state();	
}

//与视觉通信的回调函数：验证一下帧头、帧尾，装填结构体。
void pc_callback_handler(uint8_t *buff)
{
	if (buff[0] == PC_RECV_ARMOR_HEADER && Verify_CRC16_Check_Sum(buff, VISUAL_MAX_LEN) ==1)
	{
		memcpy(&pc_armor, buff +1, VISUAL_MAX_LEN -3);
		newdata = 1;
		visual_timer = HAL_GetTick();
	}
	else if (buff[0] == PC_RECV_ENERGY_HEADER && Verify_CRC16_Check_Sum(buff, VISUAL_MAX_LEN) ==1)
	{
		memcpy(&pc_energy, buff +1, VISUAL_MAX_LEN -3);
		visual_timer = HAL_GetTick();
	}
}
/**
  * @brief      clear idle it flag after uart receive a frame data
  * @param[in]  huart: uart IRQHandler id
  * @retval  
  */
static void uart_rx_idle_callback(UART_HandleTypeDef* huart)
{
	
	/* clear idle it flag avoid idle interrupt all the time */
	__HAL_UART_CLEAR_IDLEFLAG(huart);

	/* handle received data in idle interrupt */
	if (huart == &DBUS_HUART)
	{
		/* clear DMA transfer complete flag */
		__HAL_DMA_DISABLE(huart->hdmarx);

		/* handle dbus data dbus_buf from DMA */
		if ((DBUS_MAX_LEN - dma_current_data_counter(huart->hdmarx->Instance)) == DBUS_BUFLEN)
		{
			last_rc =rc;
			rc_callback_handler(&rc, dbus_buf);	
		}
		
		/* restart dma transmission */
		__HAL_DMA_SET_COUNTER(huart->hdmarx, DBUS_MAX_LEN);
		__HAL_DMA_ENABLE(huart->hdmarx);
	}
	
	else if (huart == &VISUAL_HUART)
	{
		/* clear DMA transfer complete flag */
		__HAL_DMA_DISABLE(huart->hdmarx);
		/* handle dbus data dbus_buf from DMA */
	//	if ((VISUAL_MAX_LEN - dma_current_data_counter(huart->hdmarx->Instance)) == VISUAL_BUFLEN)
		//{
			pc_callback_handler(visual_buf);
			
		//}
		
		/* restart dma transmission */
		__HAL_DMA_SET_COUNTER(huart->hdmarx, VISUAL_MAX_LEN);
		__HAL_DMA_ENABLE(huart->hdmarx);
	}
}

/**
  * @brief      callback this function when uart interrupt 
  * @param[in]  huart: uart IRQHandler id
  * @retval  
  */
void uart_receive_handler(UART_HandleTypeDef *huart)
{  
	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) && 
			__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE))
	{
		uart_rx_idle_callback(huart);
	}
}

/**
  * @brief   initialize dbus uart device 
  * @param   
  * @retval  
  */
void dbus_uart_init(void)
{
	/* open uart idle it */
	__HAL_UART_CLEAR_IDLEFLAG(&DBUS_HUART);
	__HAL_UART_ENABLE_IT(&DBUS_HUART, UART_IT_IDLE);

	uart_receive_dma_no_it(&DBUS_HUART, dbus_buf, DBUS_MAX_LEN);
}

void pc_uart_init(void)
{
	/* open uart idle it */
	__HAL_UART_CLEAR_IDLEFLAG(&VISUAL_HUART);
	__HAL_UART_ENABLE_IT(&VISUAL_HUART, UART_IT_IDLE);

	uart_receive_dma_no_it(&VISUAL_HUART, visual_buf, VISUAL_MAX_LEN);
}
	
