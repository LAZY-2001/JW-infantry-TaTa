#include "ring_math.h"

/**
 * Ring 类型是我自己定义的一种类型，分为 Ring Int 和 Ring Float 两种
 * Ring 类型如同它的名字那样，是一种“环形”的数据形式，越过最大值又会回到最小值，因此大小的概念在这里并不成立
 * Ring Int 的取值范围是[0, ring)，用int32表示
 * Ring Float 的取值范围是[0, ring), 用float表示
 * 例1 RI8192的取值范围是[0, 8192)
 * 例2 RF360的取值范围是[0, 360)
*/

/*将一个数转化为Ring类型，x和ring不能相差太大，否则会导致程序阻塞*/
int32_t RingConvertToRI(int32_t ring, int32_t x)
{
    int32_t tmp = x;
    while(tmp >= ring)
    {
        tmp -= ring;
    }
    while(tmp < 0)
    {
        tmp += ring;
    }
    return tmp;
}
float RingConvertToRF(float ring, float x)
{
    float tmp = x;
    while(tmp >= ring)
    {
        tmp -= ring;
    }
    while(tmp < 0)
    {
        tmp += ring;
    }
    return tmp;
}

/*判断一个数是不是Ring类型的数*/
uint8_t RingAssertRI(int32_t ring, int32_t x)
{
    if(x < 0 || x >= ring)
    {
        return 0;
    }
    return 1;
}
uint8_t RingAssertRF(float ring, float x)
{
    if(x < 0 || x >= ring)
    {
        return 0;
    }
    return 1;
}

/*加法*/
int32_t RingAddRI(int32_t ring, int32_t x, int32_t y)
{
    int32_t tmp = x+y;
    tmp = RingConvertToRI(ring, tmp);
    return tmp;
}
float RingAddRF(float ring, float x, float y)
{
    float tmp = x+y;
    tmp = RingConvertToRF(ring, tmp);
    return tmp;
}

/*就近减法*/
int32_t RingCloseSubRI(int32_t ring, int32_t x, int32_t y)
{
    int32_t tmp = x-y;
    int32_t half_ring = ring / 2;
    if(tmp > half_ring)
    {
        tmp -= ring;
    }
    else if(tmp < -half_ring)
    {
        tmp += ring;
    }
    return tmp;
}
float RingCloseSubRF(float ring, float x, float y)
{
    float tmp = x-y;
    float half_ring = ring / 2;
    if(tmp > half_ring)
    {
        tmp -= ring;
    }
    else if(tmp < -half_ring)
    {
        tmp += ring;
    }
    return tmp;
}

/*数值步进*/
int32_t RingValueStepRI(int32_t ring, int32_t value, int32_t dst, int32_t step)
{
    int32_t tmp = value;
    /*合法性检查*/
    if(dst>=ring || dst<0)
        return tmp;
    if(value>=ring || value<0)
        return tmp;
    if(step == 0)
        return tmp;
    if(value == dst)
        return tmp;
    /*step达到了一圈，必然可以达到dst*/
    if(step>=ring || step<=-ring)
    {
        tmp = dst;
        return tmp;
    }

    if(value < dst)
    {
        if(step > 0)
        {
            tmp += step;
            if(tmp >= dst)
            {
                tmp = dst;
                return tmp;
            }
            else
            {
                return tmp;
            }
        }
        else /* step < 0 */
        {
            tmp += step;
            if(tmp >= 0)
            {
                return tmp;
            }
            else
            {
                tmp += ring;
                if(tmp <= dst)
                {
                    tmp = dst;
                    return tmp;
                }
                else
                {
                    return tmp;
                }
            }
        }
    }
    else/* value > dst */
    {
        if(step > 0)
        {
            tmp += step;
            if(tmp < ring)
            {
                return tmp;
            }
            else
            {
                tmp -= ring;
                if(tmp >= dst)
                {
                    tmp = dst;
                    return tmp;
                }
                else
                {
                    return tmp;
                }
            }
        }
        else /* step < 0 */
        {
            tmp += step;
            if(tmp <= dst)
            {
                tmp = dst;
                return tmp;
            }
            else
            {
                return tmp;
            }
        }
    }
}
float RingValueStepRF(float ring, float value, float dst, float step)
{
    float tmp = value;
    /*合法性检查*/
    if(dst>=ring || dst<0)
        return tmp;
    if(value>=ring || value<0)
        return tmp;
    if(step == 0)
        return tmp;
    if(value == dst)
        return tmp;
    /*step达到了一圈，必然可以达到dst*/
    if(step>=ring || step<=-ring)
    {
        tmp = dst;
        return tmp;
    }

    if(value < dst)
    {
        if(step > 0)
        {
            tmp += step;
            if(tmp >= dst)
            {
                tmp = dst;
                return tmp;
            }
            else
            {
                return tmp;
            }
        }
        else /* step < 0 */
        {
            tmp += step;
            if(tmp >= 0)
            {
                return tmp;
            }
            else
            {
                tmp += ring;
                if(tmp <= dst)
                {
                    tmp = dst;
                    return tmp;
                }
                else
                {
                    return tmp;
                }
            }
        }
    }
    else/* value > dst */
    {
        if(step > 0)
        {
            tmp += step;
            if(tmp < ring)
            {
                return tmp;
            }
            else
            {
                tmp -= ring;
                if(tmp >= dst)
                {
                    tmp = dst;
                    return tmp;
                }
                else
                {
                    return tmp;
                }
            }
        }
        else /* step < 0 */
        {
            tmp += step;
            if(tmp <= dst)
            {
                tmp = dst;
                return tmp;
            }
            else
            {
                return tmp;
            }
        }
    }
}

/*沿较近的方向数值步进*/
int32_t RingCloseValueStepRI(int32_t ring, int32_t value, int32_t dst, int32_t abs_step)
{
    int32_t tmp = value;
    if(abs_step <= 0)
        return tmp;
    if(RingCloseSubRI(ring, dst, tmp) > 0)
    {
        tmp = RingValueStepRI(ring, tmp, dst, abs_step);
    }
    else
    {
        tmp = RingValueStepRI(ring, tmp, dst, -abs_step);
    }
    return tmp;
}
float RingCloseValueStepRF(float ring, float value, float dst, float abs_step)
{
    float tmp = value;
    if(abs_step <= 0)
        return tmp;
    if(RingCloseSubRF(ring, dst, tmp) > 0)
    {
        tmp = RingValueStepRF(ring, tmp, dst, abs_step);
    }
    else
    {
        tmp = RingValueStepRF(ring, tmp, dst, -abs_step);
    }
    return tmp;
}
