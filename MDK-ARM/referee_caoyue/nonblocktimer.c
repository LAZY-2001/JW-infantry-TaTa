//����������ʱ

//�˴���TIM6��Ϊϵͳʱ�ӣ���ʱ�Ĳ���timeMs��������С������ʱ����Ϊ1ms��

#include "nonblocktimer.h"

extern TIM_HandleTypeDef htim6;
extern __IO uint32_t uwTick;

//ȫ��ʱ���
#define  TickMs     uwTick

//���̿���һ������������ʱ
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

//���һ������������ʱ�Ƿ�ʱ�䵽��δ���ڼ�ʱ������ʱ�䵽
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
	else /* ��ʱ���ر�ʱ�涨Ϊʱ�䵽 */
	{
		return 1;
	}
}

//��ʱ�䵽������һ������������ʱ
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

//ֹͣһ������������ʱ
void NBTimerStopTimer(NBTimer_t* timer)
{
	timer->en = 0;
}

//���һ������������ʱ�Ƿ����ڼ�ʱ
uint8_t NBTimerIsTiming(NBTimer_t* timer)
{
	return timer->en;
}

//���ؿ�ʼ��ʱ���Ѿ�������ʱ��
uint32_t NBTimerTimeElapsed(NBTimer_t* timer)
{
	return (TickMs - timer->msStart);
}

//���ؼ�ʱ��ʣ���ʱ�䣬ʱ�䵽����0
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















