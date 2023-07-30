//非阻塞性延时

//此处以TIM6作为系统时钟，计时的参数timeMs允许输入小数，计时精度为1ms。

#include "nonblocktimer.h"

extern TIM_HandleTypeDef htim6;
extern __IO uint32_t uwTick;

//全局时间刻
#define  TickMs     uwTick

//立刻开启一个非阻塞性延时
void NBTimerStartTimer(NBTimer_t* timer, int32_t timeMs)
{
	uint32_t tmp=0;

	if(timeMs < 0)
	{
		timeMs = -timeMs;
	}
	
	tmp = (uint32_t)(timeMs);

	timer->msStart = TickMs;
	timer->msMax = tmp + timer->msStart;
	timer->en = 1;
}

//检查一个非阻塞性延时是否时间到，未正在计时的算作时间到
uint8_t NBTimerTimeOut(NBTimer_t* timer)
{	
	if(timer->en == 1)
	{
		if(TickMs >= timer->msMax)
		{
			timer->en = 0;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else /* 计时器关闭时规定为时间到 */
	{
		return 1;
	}
}

//若时间到，则开启一个非阻塞性延时
uint8_t NBTimerStartTimerOne(NBTimer_t* timer, int32_t timeMs)
{
	if(NBTimerTimeOut(timer))
	{
		NBTimerStartTimer(timer, timeMs);
		return 1;
	}
	else
	{
		return 0;
	}
}

//停止一个非阻塞性延时
void NBTimerStopTimer(NBTimer_t* timer)
{
	timer->en = 0;
}

//检查一个非阻塞性延时是否正在计时
uint8_t NBTimerIsTiming(NBTimer_t* timer)
{
	return timer->en;
}

//返回开始计时后已经经过的时间
uint32_t NBTimerTimeElapsed(NBTimer_t* timer)
{
	return (TickMs - timer->msStart);
}

//返回计时器剩余的时间，时间到返回0
uint32_t NBTimerTimeLeft(NBTimer_t* timer)
{
	if(NBTimerTimeOut(timer))
	{
		return 0;
	}
	else
	{
		return (timer->msMax - TickMs);
	}
}















