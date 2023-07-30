#include "referee_receiver.h"
#include "string.h"

#include "CRC8_CRC16.h"

static void priRRManagerUARTInit(UART_HandleTypeDef* phuart, uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
    //enable the DMA transfer for the receiver and tramsmit request
    //使能DMA串口接收和发送
    SET_BIT(phuart->Instance->CR3, USART_CR3_DMAR);

    //enalbe idle interrupt
    //使能空闲中断
    __HAL_UART_ENABLE_IT(phuart, UART_IT_IDLE);

    //disable DMA
    //失效DMA
    __HAL_DMA_DISABLE(phuart->hdmarx);
    
    while(phuart->hdmarx->Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(phuart->hdmarx);
    }

    __HAL_DMA_CLEAR_FLAG(phuart->hdmarx, DMA_LISR_TCIF1);

    phuart->hdmarx->Instance->PAR = (uint32_t) &(USART6->DR);
    //memory buffer 1
    //内存缓冲区1
    phuart->hdmarx->Instance->M0AR = (uint32_t)(rx1_buf);
    //memory buffer 2
    //内存缓冲区2
    phuart->hdmarx->Instance->M1AR = (uint32_t)(rx2_buf);
    //data length
    //数据长度
    __HAL_DMA_SET_COUNTER(phuart->hdmarx, dma_buf_num);

    //enable double memory buffer
    //使能双缓冲区
    SET_BIT(phuart->hdmarx->Instance->CR, DMA_SxCR_DBM);

    //enable DMA
    //使能DMA
    __HAL_DMA_ENABLE(phuart->hdmarx);
}

//裁判系统核心数据识别、处理过程
static void referee_data_solve(RRManager_t* me)
{
    uint8_t *frame = me->unpack_obj.protocol_packet;
    frame_header_struct_t referee_receive_header;
    RRNode_t* pnode;

    uint16_t cmd_id = 0;
    uint8_t index = 0;

    memcpy(&referee_receive_header, frame, sizeof(frame_header_struct_t));
    index += sizeof(frame_header_struct_t);

    memcpy(&cmd_id, frame + index, sizeof(uint16_t));
    index += sizeof(uint16_t);

    uint16_t id0 = (cmd_id & 0xFF00) >> 8;
    uint16_t id1 = cmd_id & 0x00FF;

    if(id0 > RR_CMD_ID_0_MAX || id1 > RR_CMD_ID_1_MAX)
        return;//ID超限

    pnode = me->node_list[id0][id1];
    if(pnode == NULL)
        return;//未启用对应的节点
    
    if(referee_receive_header.data_length != pnode->data_len)
        return;//数据长度不正确

    RecvHeaderUpdate(&pnode->header);
    memcpy(pnode->pdata, frame + index, pnode->data_len);

    if(pnode->recv_callback != NULL)
        pnode->recv_callback(pnode);
}

void referee_unpack_fifo_data(RRManager_t* me)
{
    uint8_t byte = 0;
    uint8_t sof = HEADER_SOF;
    unpack_data_t *p_obj = &me->unpack_obj;

    while ( fifo_s_used(&me->fifo) )
    {
        byte = fifo_s_get(&me->fifo);
        switch(p_obj->unpack_step)
        {
            case STEP_HEADER_SOF:
            {
                if(byte == sof)
                {
                    p_obj->unpack_step = STEP_LENGTH_LOW;
                    p_obj->protocol_packet[p_obj->index++] = byte;
                }
                else
                    p_obj->index = 0;
            }break;
            
            case STEP_LENGTH_LOW:
            {
                p_obj->data_len = byte;
                p_obj->protocol_packet[p_obj->index++] = byte;
                p_obj->unpack_step = STEP_LENGTH_HIGH;
            }break;
            
            case STEP_LENGTH_HIGH:
            {
            p_obj->data_len |= (byte << 8);
            p_obj->protocol_packet[p_obj->index++] = byte;

            if(p_obj->data_len < (REF_PROTOCOL_FRAME_MAX_SIZE - REF_HEADER_CRC_CMDID_LEN))
            {
                p_obj->unpack_step = STEP_FRAME_SEQ;
            }
            else
            {
                p_obj->unpack_step = STEP_HEADER_SOF;
                p_obj->index = 0;
            }
            }break;

            case STEP_FRAME_SEQ:
            {
                p_obj->protocol_packet[p_obj->index++] = byte;
                p_obj->unpack_step = STEP_HEADER_CRC8;
            }break;

            case STEP_HEADER_CRC8:
            {
                p_obj->protocol_packet[p_obj->index++] = byte;

                if (p_obj->index == REF_PROTOCOL_HEADER_SIZE)
                {
                    if ( Verify_CRC8_Check_Sum(p_obj->protocol_packet, REF_PROTOCOL_HEADER_SIZE) )
                    {
                        p_obj->unpack_step = STEP_DATA_CRC16;
                    }
                    else
                    {
                        p_obj->unpack_step = STEP_HEADER_SOF;
                        p_obj->index = 0;
                    }
                }
            }break;  
            
            case STEP_DATA_CRC16:
            {
                if (p_obj->index < (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
                {
                    p_obj->protocol_packet[p_obj->index++] = byte;  
                }
                if (p_obj->index >= (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
                {
                    p_obj->unpack_step = STEP_HEADER_SOF;
                    p_obj->index = 0;

                    if ( Verify_CRC16_Check_Sum(p_obj->protocol_packet, REF_HEADER_CRC_CMDID_LEN + p_obj->data_len) )
                    {
                        referee_data_solve(me);
                    }
                }
            }break;

            default:
            {
                p_obj->unpack_step = STEP_HEADER_SOF;
                p_obj->index = 0;
            }break;
        }
    }
}

void RRNodeInit(RRNode_t* me, uint16_t cmd_id, void* pdata, uint32_t data_len, void (*recv_callback)(struct RRNodeTag* me))
{
    uint16_t id0 = (cmd_id & 0xFF00) >> 8;
    uint16_t id1 = cmd_id & 0x00FF;
    if(id0 > RR_CMD_ID_0_MAX || id1 > RR_CMD_ID_1_MAX)
        return;

    if(pdata == NULL)
        return;
    
    if(data_len == 0)
        return;
    
    me->cmd_id = cmd_id;
    me->data_len = data_len;
    RecvHeaderInit(&me->header);
    me->pdata = pdata;
    me->recv_callback = recv_callback;
}

void RRNodeAddToManager(RRNode_t* me, RRManager_t* pmanager)
{
    uint16_t id0 = (me->cmd_id & 0xFF00) >> 8;
    uint16_t id1 = me->cmd_id & 0x00FF;
    if(id0 > RR_CMD_ID_0_MAX || id1 > RR_CMD_ID_1_MAX)
    {
        return;
    }
    pmanager->node_list[id0][id1] = me;
    me->pmanager = pmanager;
}

void RRManagerInit(RRManager_t* me, UART_HandleTypeDef* phuart)
{
    memset(me, 0, sizeof(RRManager_t));
    fifo_s_init(&me->fifo, me->fifo_buf, REFEREE_FIFO_BUF_LENGTH);
    me->phuart = phuart;
}

void RRManagerStart(RRManager_t* me)
{
    priRRManagerUARTInit(me->phuart, me->usart_buf[0], me->usart_buf[1], USART_RX_BUF_LENGHT);
}

void RRManager_USART_IRQHandler(RRManager_t* me)
{
    static volatile uint8_t res;
    if(me->phuart->Instance->SR & UART_FLAG_IDLE)
    {
        __HAL_UART_CLEAR_PEFLAG(me->phuart);

        static uint16_t this_time_rx_len = 0;

        if ((me->phuart->hdmarx->Instance->CR & DMA_SxCR_CT) == RESET)
        {
            __HAL_DMA_DISABLE(me->phuart->hdmarx);
            this_time_rx_len = USART_RX_BUF_LENGHT - __HAL_DMA_GET_COUNTER(me->phuart->hdmarx);
            __HAL_DMA_SET_COUNTER(me->phuart->hdmarx, USART_RX_BUF_LENGHT);
            me->phuart->hdmarx->Instance->CR |= DMA_SxCR_CT;
            __HAL_DMA_ENABLE(me->phuart->hdmarx);
            fifo_s_puts(&me->fifo, (char*)me->usart_buf[0], this_time_rx_len);
        }
        else
        {
            __HAL_DMA_DISABLE(me->phuart->hdmarx);
            this_time_rx_len = USART_RX_BUF_LENGHT - __HAL_DMA_GET_COUNTER(me->phuart->hdmarx);
            __HAL_DMA_SET_COUNTER(me->phuart->hdmarx, USART_RX_BUF_LENGHT);
            me->phuart->hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
            __HAL_DMA_ENABLE(me->phuart->hdmarx);
            fifo_s_puts(&me->fifo, (char*)me->usart_buf[1], this_time_rx_len);
        }
    }
}

