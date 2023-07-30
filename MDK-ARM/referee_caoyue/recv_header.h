#ifndef __RECV_HEADER
#define __RECV_HEADER

#include "stm32f4xx_hal.h"

typedef __packed struct RecvHeaderTag
{
    uint32_t t;//Ê±¼ä´Á
    uint32_t n;//Ö¡ÐòºÅ
}RecvHeader_t;

void RecvHeaderInit(RecvHeader_t* pheader);
void RecvHeaderUpdate(RecvHeader_t* pheader);
void RecvHeaderCopy(RecvHeader_t* pSrc, RecvHeader_t* pDst);

#endif /* __RECV_HEADER */

