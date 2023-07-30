#include "bullet_model.h"
#include "arm_math.h"
#include "math.h"
#include "misc.h"

const float k = 0.01903f;   //   const float k = 0.01903f;    //25°C,1atm,小弹丸
                            //   const double k = 0.000556f;  //25°C,1atm,大弹丸
                            //   const double k = 0.000530f;  //25°C,1atm,发光大弹丸
const float g = 9.781f;

static float priBulletModel(float x, float v, float angle)
{   
    //x:m, v:m/s, angle:rad
    float y, t;
    t = (float)((expf(k * x) - 1) / (k * v * arm_cos_f32(angle)));
    y = (float)(v * arm_sin_f32(angle) * t - g * t * t / 2);
    return y;
}
/**
 * dis 目标水平距离
 * height 目标垂直距离，向上为正
 * ammo_speed 弹速，m/s
 * return pit弧度，水平为0，抬高为正
 * 
*/
float BulletModelGetPitch(float dis, float height, float ammo_speed)
{
    float h_temp, h_actual, dh;
    float a;
    h_temp = height;
    // by iteration
    for (int i = 0; i < 20; i++) {
        a = atan2f(h_temp, dis);
        h_actual = priBulletModel(dis, ammo_speed, a);
        dh = height - h_actual;
        h_temp = h_temp + dh; 
        if (MISC_Abs(dh) < 0.001f) {
            break;
        }
    }
    return a;
}
