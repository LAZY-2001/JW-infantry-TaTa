#include "ring_math.h"

/**
 * Ring ���������Լ������һ�����ͣ���Ϊ Ring Int �� Ring Float ����
 * Ring ������ͬ����������������һ�֡����Ρ���������ʽ��Խ�����ֵ�ֻ�ص���Сֵ����˴�С�ĸ��������ﲢ������
 * Ring Int ��ȡֵ��Χ��[0, ring)����int32��ʾ
 * Ring Float ��ȡֵ��Χ��[0, ring), ��float��ʾ
 * ��1 RI8192��ȡֵ��Χ��[0, 8192)
 * ��2 RF360��ȡֵ��Χ��[0, 360)
*/

/*��һ����ת��ΪRing���ͣ�x��ring�������̫�󣬷���ᵼ�³�������*/
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

/*�ж�һ�����ǲ���Ring���͵���*/
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

/*�ӷ�*/
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

/*�ͽ�����*/
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

/*��ֵ����*/
int32_t RingValueStepRI(int32_t ring, int32_t value, int32_t dst, int32_t step)
{
    int32_t tmp = value;
    /*�Ϸ��Լ��*/
    if(dst>=ring || dst<0)
        return tmp;
    if(value>=ring || value<0)
        return tmp;
    if(step == 0)
        return tmp;
    if(value == dst)
        return tmp;
    /*step�ﵽ��һȦ����Ȼ���Դﵽdst*/
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
    /*�Ϸ��Լ��*/
    if(dst>=ring || dst<0)
        return tmp;
    if(value>=ring || value<0)
        return tmp;
    if(step == 0)
        return tmp;
    if(value == dst)
        return tmp;
    /*step�ﵽ��һȦ����Ȼ���Դﵽdst*/
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

/*�ؽϽ��ķ�����ֵ����*/
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
