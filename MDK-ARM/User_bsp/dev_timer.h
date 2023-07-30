#ifndef __DEV_TIMER_H
#define __DEV_TIMER_H

#include "stm32f4xx_hal.h"
#include <setjmp.h>

#define DEV_TIMER_DRIVER_VER_OLD  0
#define DEV_TIMER_DRIVER_VER_CUBE  1  

#define DEV_TIMER_DRIVER_VER  DEV_TIMER_DRIVER_VER_OLD

typedef struct
{
  uint8_t en;
  uint32_t tick;
  uint32_t timeOut;
}DEV_TIMER_t;

// 阻塞性延时必须要以非阻塞性延时为基础
typedef struct
{
  uint8_t isDelaying; // 0:没有阻塞性延时,1:正在进行阻塞性延时
  uint8_t firstFlag;
  jmp_buf jbufDelay;
  jmp_buf jbufEnd;
  DEV_TIMER_t timer; // 延时定时器
}DEV_TIMERB_t; // 阻塞性延时

 void HAL_IncTick(void);
uint32_t DEV_TIMER_GetSysTickCur(void);
uint32_t DEV_TIMER_GetSysTickLapse(uint32_t u32PreTick);
void DEV_TIMER_StartTimer( DEV_TIMER_t *timer, uint32_t timeOut );
void DEV_TIMER_StartTimerOne(DEV_TIMER_t *timer, uint32_t timeOut );
void DEV_TIMER_StopTimer(DEV_TIMER_t *timer);
uint8_t DEV_TIMER_TimeOut(DEV_TIMER_t *timer);
void DEV_TIMER_DelayInit(void);
void DEV_TIMER_DelayUs(uint32_t dwTime);
void DEV_TIMER_DelayMs(uint32_t dwTime);


#endif

