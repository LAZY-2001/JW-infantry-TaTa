#ifndef __REFEREE_RECEIVER_H
#define __REFEREE_RECEIVER_H

#include "stm32f4xx_hal.h"
#include "recv_header.h"

#include "fifo.h"
#include "protocol.h"

#define USART_RX_BUF_LENGHT     512
#define REFEREE_FIFO_BUF_LENGTH 1024

#define RR_CMD_ID_0_MAX (0x03)
#define RR_CMD_ID_1_MAX (0x0C)

struct RRManagerTag;

typedef struct RRNodeTag
{
    uint16_t cmd_id;
    uint32_t data_len;//���ݶε���ȷ����
    RecvHeader_t header;//���յ���ȷ��������ʱ������,���ڼ���Ƶ��
    void* pdata;//����ָ��
    void (*recv_callback)(struct RRNodeTag* me);
    struct RRManagerTag* pmanager;
}RRNode_t;

typedef struct RRManagerTag
{
    UART_HandleTypeDef* phuart;
    fifo_s_t fifo;
    uint8_t fifo_buf[REFEREE_FIFO_BUF_LENGTH];
    unpack_data_t unpack_obj;
    uint8_t usart_buf[2][USART_RX_BUF_LENGHT];
    RRNode_t* node_list[RR_CMD_ID_0_MAX + 1][RR_CMD_ID_1_MAX + 1];
}RRManager_t;

void RRNodeInit(RRNode_t* me, uint16_t cmd_id, void* pdata, uint32_t data_len, void (*recv_callback)(struct RRNodeTag* me));
void RRNodeAddToManager(RRNode_t* me, RRManager_t* pmanager);

void RRManagerInit(RRManager_t* me, UART_HandleTypeDef* phuart);
void RRManagerStart(RRManager_t* me);
void RRManagerRecv(RRManager_t* me);
void RRManager_USART_IRQHandler(RRManager_t* me);

void referee_unpack_fifo_data(RRManager_t* me);
#endif /* __REFEREE_RECEIVER_H */
