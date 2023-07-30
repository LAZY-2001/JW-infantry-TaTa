/**
 ***************************************(C) COPYRIGHT 2018 DJI***************************************
 * @file       bsp_uart.h
 * @brief      this file contains the common defines and functions prototypes for 
 *             the bsp_uart.c driver
 * @note         
 * @Version    V1.0.0
 * @Date       Jan-30-2018      
 ***************************************(C) COPYRIGHT 2018 DJI***************************************
 */

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "usart.h"
#include "pc_comm_protocol.h"

#define UART_RX_DMA_SIZE (1024)
#define DBUS_MAX_LEN     (54)
#define DBUS_BUFLEN      (18)
#define DBUS_HUART       huart1 /* for dji remote controler reciever */

#define REFEREE_MAX_LEN     (50)
#define REFEREE_BUFLEN      (50)
#define REFEREE_HUART       huart6

#define VISUAL_MAX_LEN     (sizeof(PCRecvPacket_t))
#define VISUAL_BUFLEN      (sizeof(PCRecvPacket_t))
#define VISUAL_HUART        huart8

#define HEADER_LENGTH 5
#define ID_LENGTH     2
#define TAIL_LENGTH   2


/** 
  * @brief  remote control information
  */
typedef  struct
{
  /* rocker channel information */
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;
  /* left and right lever information */
  uint8_t sw1;
  uint8_t sw2;
  /* mouse movement and button information */
  struct
  {
    int16_t x;
    int16_t y;
    int16_t z;

    uint8_t l;
    uint8_t r;
  } mouse;
  /* keyboard key information */
  union {
    uint16_t key_code;
    struct
    {
      uint16_t W : 1;
      uint16_t S : 1;
      uint16_t A : 1;
      uint16_t D : 1;
      uint16_t SHIFT : 1;
      uint16_t CTRL : 1;
      uint16_t Q : 1;
      uint16_t E : 1;
      uint16_t R : 1;
      uint16_t F : 1;
      uint16_t G : 1;
      uint16_t Z : 1;
      uint16_t X : 1;
      uint16_t C : 1;
      uint16_t V : 1;
      uint16_t B : 1;
    } bit;
  } kb;
  int16_t wheel;
} rc_info_t;

void uart_receive_handler(UART_HandleTypeDef *huart);
void dbus_uart_init(void);
void referee_uart_init(void);
void pc_uart_init(void);
extern rc_info_t rc;
extern rc_info_t last_rc;

extern PCRecvPacketEnergy_t pc_energy;
extern PCRecvPacketArmor_t  pc_armor;
extern uint8_t visual_buf[VISUAL_BUFLEN];
extern uint8_t referee_buf[REFEREE_BUFLEN];
extern uint32_t visual_timer;
#endif

