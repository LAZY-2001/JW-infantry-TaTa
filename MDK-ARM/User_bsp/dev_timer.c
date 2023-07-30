/********************************************************
 * 文件名  ：time.c
 * 描述    ：       
 * 开发平台：
 * 库版本  ：ST3.0.0  																										  
 * 项目名称：
*********************************************************/
#include "dev_timer.h"

//static __O uint32_t uwTick;//系统时钟

uint32_t usTick, msTick;
DEV_TIMERB_t gTimerBDemo;
//------------------------------------------------------------------------------
//输入参数: 无
//输出参数: 无
//返 回 值: 无
//说    明: 在SYSTICK_IRQHandler()中断函数中调用，
//------------------------------------------------------------------------------	
 void HAL_IncTick(void)
{
    uwTick++;
}

//------------------------------------------------------------------------------
//输入参数: 无
//输出参数: 无
//返 回 值: 返回当前的系统Tick值
//说    明: 获取系统自开机以来的系统Tick值【10ms/Tick】
//------------------------------------------------------------------------------
uint32_t DEV_TIMER_GetSysTickCur(void)
{
   return uwTick;
}

//------------------------------------------------------------------------------
//输入参数: u32PreTick - 只能是上一次调用DEV_TIMER_GetSysTickCur()时获得的系统Tick值
//输出参数: 无
//返 回 值: 返回当前系统Tick(u32SysTickCount)与之前Tick(u32PreTice)的差值
//说    明: 获取自上一次获取系统Tick(u32PreTice)时已流逝的Tick数【10ms/Tick】
//------------------------------------------------------------------------------
uint32_t DEV_TIMER_GetSysTickLapse(uint32_t u32PreTick)
{
    volatile uint32_t u32CurTick = DEV_TIMER_GetSysTickCur();
    return ((u32CurTick >= u32PreTick) ? (u32CurTick - u32PreTick):
                                         (0xFFFFFFFF - u32PreTick + u32CurTick));
}

//马上重新计时
void DEV_TIMER_StartTimer( DEV_TIMER_t *timer, uint32_t timeOut )
{
  timer->en = 1;
  timer->tick = DEV_TIMER_GetSysTickCur();//计时开始 
  timer->timeOut = timeOut;
}
//只有Timer时间到了以后才会再次重新计时
void DEV_TIMER_StartTimerOne(DEV_TIMER_t *timer, uint32_t timeOut )
{
  if (timer->en != 1)
  {
    timer->en = 1;
    timer->tick = DEV_TIMER_GetSysTickCur();//计时开始 
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
      //timer->tick = SysTick_GetCurrent(); // 加入该语句实现重复定时
      timeOutFlag = 1;
      timer->en = 0;
    }
  }
  return timeOutFlag;
}

//void DEV_TIMER_DelayInit(void)
//{  
//  #if DEV_TIMER_DRIVER_VER == DEV_TIMER_DRIVER_VER_CUBE
//  usTick = HAL_RCC_GetHCLKFreq()/1000000;    //需延时时间，共多少时间节拍
//  #else
//  RCC_ClocksTypeDef  rccClocks;
//  RCC_GetClocksFreq(&rccClocks);
//  usTick = rccClocks.HCLK_Frequency/1000000;    //需延时时间，共多少时间节拍
//  #endif
//  msTick = usTick*1000;
//}

/********************************************************************************************************************
** 函数名称: void DEV_TIMER_DelayUs(INT32U dwTime)
** 功能描述: 阻塞性微秒级延时函数                         
** 输　  入: INT32U dwTime: 延时时间                 
** 输 　 出:                                                 
********************************************************************************************************************/
void DEV_TIMER_DelayUs(uint32_t dwTime)
{
  uint32_t dwCurCounter=0;                                //当前时间计数值
  uint32_t dwPreTickVal=SysTick->VAL;                     //上一次SYSTICK计数值
  uint32_t dwCurTickVal;                                  //上一次SYSTICK计数值
  //RCC_ClocksTypeDef  rccClocks;
  //RCC_GetClocksFreq(&rccClocks);
  dwTime=dwTime*usTick;    //需延时时间，共多少时间节拍
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
** 函数名称: void DEV_TIMER_DelayMs(INT32U dwTime)
** 功能描述: 阻塞性毫秒级延时函数                         
** 输　  入: INT32U dwTime: 延时时间                 
** 输 　 出:                                                 
********************************************************************************************************************/
void DEV_TIMER_DelayMs(uint32_t dwTime)
{
  uint32_t dwCurCounter=0;                                //当前时间计数值
  uint32_t dwPreTickVal=SysTick->VAL;                     //上一次SYSTICK计数值
  uint32_t dwCurTickVal;                                  //上一次SYSTICK计数值
  //RCC_ClocksTypeDef  rccClocks;
  //RCC_GetClocksFreq(&rccClocks);
  dwTime=dwTime*msTick;    //需延时时间，共多少时间节拍
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



