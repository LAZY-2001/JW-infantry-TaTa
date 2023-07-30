/********************************************************
 * �ļ���  ��time.c
 * ����    ��       
 * ����ƽ̨��
 * ��汾  ��ST3.0.0  																										  
 * ��Ŀ���ƣ�
*********************************************************/
#include "dev_timer.h"

//static __O uint32_t uwTick;//ϵͳʱ��

uint32_t usTick, msTick;
DEV_TIMERB_t gTimerBDemo;
//------------------------------------------------------------------------------
//�������: ��
//�������: ��
//�� �� ֵ: ��
//˵    ��: ��SYSTICK_IRQHandler()�жϺ����е��ã�
//------------------------------------------------------------------------------	
 void HAL_IncTick(void)
{
    uwTick++;
}

//------------------------------------------------------------------------------
//�������: ��
//�������: ��
//�� �� ֵ: ���ص�ǰ��ϵͳTickֵ
//˵    ��: ��ȡϵͳ�Կ���������ϵͳTickֵ��10ms/Tick��
//------------------------------------------------------------------------------
uint32_t DEV_TIMER_GetSysTickCur(void)
{
   return uwTick;
}

//------------------------------------------------------------------------------
//�������: u32PreTick - ֻ������һ�ε���DEV_TIMER_GetSysTickCur()ʱ��õ�ϵͳTickֵ
//�������: ��
//�� �� ֵ: ���ص�ǰϵͳTick(u32SysTickCount)��֮ǰTick(u32PreTice)�Ĳ�ֵ
//˵    ��: ��ȡ����һ�λ�ȡϵͳTick(u32PreTice)ʱ�����ŵ�Tick����10ms/Tick��
//------------------------------------------------------------------------------
uint32_t DEV_TIMER_GetSysTickLapse(uint32_t u32PreTick)
{
    volatile uint32_t u32CurTick = DEV_TIMER_GetSysTickCur();
    return ((u32CurTick >= u32PreTick) ? (u32CurTick - u32PreTick):
                                         (0xFFFFFFFF - u32PreTick + u32CurTick));
}

//�������¼�ʱ
void DEV_TIMER_StartTimer( DEV_TIMER_t *timer, uint32_t timeOut )
{
  timer->en = 1;
  timer->tick = DEV_TIMER_GetSysTickCur();//��ʱ��ʼ 
  timer->timeOut = timeOut;
}
//ֻ��Timerʱ�䵽���Ժ�Ż��ٴ����¼�ʱ
void DEV_TIMER_StartTimerOne(DEV_TIMER_t *timer, uint32_t timeOut )
{
  if (timer->en != 1)
  {
    timer->en = 1;
    timer->tick = DEV_TIMER_GetSysTickCur();//��ʱ��ʼ 
    timer->timeOut = timeOut;
  }
}

void DEV_TIMER_StopTimer(DEV_TIMER_t *timer)
{
  timer->en = 0;
  timer->tick = 0;
  timer->timeOut = 0;
}
uint8_t DEV_TIMER_TimeOut(DEV_TIMER_t *timer)
{
  uint8_t timeOutFlag = 0;
  if (timer->en)
  {
    if(DEV_TIMER_GetSysTickLapse(timer->tick) >= timer->timeOut)
    {
      //timer->tick = SysTick_GetCurrent(); // ��������ʵ���ظ���ʱ
      timeOutFlag = 1;
      timer->en = 0;
    }
  }
  return timeOutFlag;
}

//void DEV_TIMER_DelayInit(void)
//{  
//  #if DEV_TIMER_DRIVER_VER == DEV_TIMER_DRIVER_VER_CUBE
//  usTick = HAL_RCC_GetHCLKFreq()/1000000;    //����ʱʱ�䣬������ʱ�����
//  #else
//  RCC_ClocksTypeDef  rccClocks;
//  RCC_GetClocksFreq(&rccClocks);
//  usTick = rccClocks.HCLK_Frequency/1000000;    //����ʱʱ�䣬������ʱ�����
//  #endif
//  msTick = usTick*1000;
//}

/********************************************************************************************************************
** ��������: void DEV_TIMER_DelayUs(INT32U dwTime)
** ��������: ������΢�뼶��ʱ����                         
** �䡡  ��: INT32U dwTime: ��ʱʱ��                 
** �� �� ��:                                                 
********************************************************************************************************************/
void DEV_TIMER_DelayUs(uint32_t dwTime)
{
  uint32_t dwCurCounter=0;                                //��ǰʱ�����ֵ
  uint32_t dwPreTickVal=SysTick->VAL;                     //��һ��SYSTICK����ֵ
  uint32_t dwCurTickVal;                                  //��һ��SYSTICK����ֵ
  //RCC_ClocksTypeDef  rccClocks;
  //RCC_GetClocksFreq(&rccClocks);
  dwTime=dwTime*usTick;    //����ʱʱ�䣬������ʱ�����
  for(;;)
  {
    dwCurTickVal = SysTick->VAL;
    if(dwCurTickVal < dwPreTickVal)
    {
      dwCurCounter = dwCurCounter+dwPreTickVal-dwCurTickVal;
    }
    else
    {
      dwCurCounter = dwCurCounter+dwPreTickVal+SysTick->LOAD-dwCurTickVal;
    }
    dwPreTickVal = dwCurTickVal;
    if(dwCurCounter >= dwTime)
    {
      return;
    }
  }
}
/********************************************************************************************************************
** ��������: void DEV_TIMER_DelayMs(INT32U dwTime)
** ��������: �����Ժ��뼶��ʱ����                         
** �䡡  ��: INT32U dwTime: ��ʱʱ��                 
** �� �� ��:                                                 
********************************************************************************************************************/
void DEV_TIMER_DelayMs(uint32_t dwTime)
{
  uint32_t dwCurCounter=0;                                //��ǰʱ�����ֵ
  uint32_t dwPreTickVal=SysTick->VAL;                     //��һ��SYSTICK����ֵ
  uint32_t dwCurTickVal;                                  //��һ��SYSTICK����ֵ
  //RCC_ClocksTypeDef  rccClocks;
  //RCC_GetClocksFreq(&rccClocks);
  dwTime=dwTime*msTick;    //����ʱʱ�䣬������ʱ�����
  for(;;)
  {
    dwCurTickVal = SysTick->VAL;
    if(dwCurTickVal < dwPreTickVal)
    {
      dwCurCounter = dwCurCounter+dwPreTickVal-dwCurTickVal;
    }
    else
    {
      dwCurCounter = dwCurCounter+dwPreTickVal+SysTick->LOAD-dwCurTickVal;
    }
    dwPreTickVal = dwCurTickVal;
    if(dwCurCounter >= dwTime)
    {
      return;
    }
  }
}

//===============================================
#if 1
void DEV_TIMERB_DelayMs(DEV_TIMERB_t *timerB, uint32_t ms)
{
  DEV_TIMER_StartTimerOne(&timerB->timer, ms);
  timerB->isDelaying = 1;
  longjmp(timerB->jbufEnd, 1);
}
#endif

#if 1
void DEV_TIMERB_Check(DEV_TIMERB_t *timerB)
{
  if (timerB->isDelaying)
  {
    if (DEV_TIMER_TimeOut(&timerB->timer))
    {
      timerB->isDelaying = 0;
      longjmp(timerB->jbufDelay, 1);
    }else
    {
      longjmp(timerB->jbufEnd, 1);
    }
  }
}
#endif

#if 1
//void DEV_TIMERB_Demo_WaitFunc(DEV_TIMERB_t *timerB)
//{
//  if (!setjmp(timerB->jbufDelay))
//  {
//    DEV_TIMERB_DelayMs(timerB, 4000);
//  }
//  printf("\r\nend wait Func");
//}
#endif

#if 1
//void DEV_TIMERB_Demo_Task(DEV_TIMERB_t *timerB)
//{
//  static uint8_t taskState = 0;
//  switch (taskState)
//  {
//    case 0:
//      taskState = 1;
//      timerB->firstFlag = 1;
//      timerB->isDelaying = 0;
//      break;
//    case 1:
//      DEV_TIMERB_Check(timerB);
//      if (!timerB->firstFlag)
//      {
//        if (!timerB->isDelaying)
//        {
//          // delay 1
//          printf("\r\nstart delay 1");
//          if (!setjmp(timerB->jbufDelay))
//          {
//            DEV_TIMERB_DelayMs(timerB, 1000);
//          }
//          printf("\r\nend delay 1");
//          // delay 2
//          printf("\r\nstart delay 2");
//          if (!setjmp(timerB->jbufDelay))
//          {
//            DEV_TIMERB_DelayMs(timerB, 2000);
//          }
//          printf("\r\nend delay 2");
//          // delay 3
//          printf("\r\nstart delay 3");
//          if (!setjmp(timerB->jbufDelay))
//          {
//            DEV_TIMERB_DelayMs(timerB, 3000);
//          }
//          printf("\r\nend delay 3");
//          //===========================
//          DEV_TIMERB_Demo_WaitFunc(timerB);
//          //===========================
//        }
//      }else
//      {
//        timerB->firstFlag = 0;
//        setjmp(timerB->jbufEnd);
//      }
//      break;
//    default:
//      break;
//  }
//}
#endif



