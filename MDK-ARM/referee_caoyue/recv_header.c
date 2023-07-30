/*通用的接收数据头部*/

#include "recv_header.h"

void RecvHeaderInit(RecvHeader_t* pheader)
{
    pheader->t = 0;
    pheader->n = 0;
}

void RecvHeaderUpdate(RecvHeader_t* pheader)
{
    pheader->t = HAL_GetTick();
    pheader->n += 1;
}

void RecvHeaderCopy(RecvHeader_t* pSrc, RecvHeader_t* pDst)
{
    pDst->n = pSrc->n;
    pDst->t = pSrc->t;
}
