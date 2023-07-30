#ifndef __NONBLOCKTIMER_H
#define __NONBLOCKTIMER_H

#include "stm32f4xx_hal.h"

typedef struct
{
	uint8_t en;
	uint32_t msMax;
	uint32_t msStart;
}NBTimer_t;

void NBTimerStartTimer(NBTimer_t* timer, int32_t timeMs);
uint8_t NBTimerTimeOut(NBTimer_t* timer);
uint8_t NBTimerStartTimerOne(NBTimer_t* timer, int32_t timeMs);
void NBTimerStopTimer(NBTimer_t* timer);
uint8_t NBTimerIsTiming(NBTimer_t* timer);
uint32_t NBTimerTimeElapsed(NBTimer_t* timer);
uint32_t NBTimerTimeLeft(NBTimer_t* timer);

#endif /* __NONBLOCKTIMER_H */




















