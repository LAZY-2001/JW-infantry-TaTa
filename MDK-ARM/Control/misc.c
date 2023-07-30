#include "misc.h"
#include "arm_math.h"

float Convert8192To360(int32_t value_8192)
{
    float value_360 = (float)value_8192;
    value_360 *= 360.0f;
    value_360 /= 8192.0f;
    return value_360;
}

float RPM2DPS(float rpm)
{
    float tmp = rpm /= 60.0f;
    tmp *= 360.0f;
	return tmp;
}

float MISC_ValueStep(float value, float target, float step)
{
    if(step <= 0)
        return value;
    if(value > target)
    {
        value -= step;
        if(value > target)
            return value;
        else
            return target;
    }
    else if(value < target)
    {
        value += step;
        if(value < target)
            return value;
        else
            return target;
    }
    else
    {
        return value;
    }
}
