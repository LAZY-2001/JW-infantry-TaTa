#include "armor_predictor.h"

#include "string.h"
#include "arm_math.h"

#define ARMOR_PREDICTOR_INTERVAL_MS (2)
#define ARMOR_PREDICTOR_INTERVAL (0.002f)

#define ARMOR_PREDICTOR_ITERATE_MAX (5) //最大迭代次数

ArmorPredictor_t armor_predictor;

//从车体中心换算到装甲板
static void priCenter2Armor(Point3D_t c, float yaw, float r, Point3D_t* pa)
{
    pa->x = c.x - r * arm_cos_f32(yaw);
    pa->y = c.y - r * arm_sin_f32(yaw);
    pa->z = c.z;
}

//计算距离
static float priArmor2Dis(Point3D_t a)
{
    float dis;
    arm_sqrt_f32(a.x*a.x + a.y*a.y + a.z*a.z, &dis);
    return dis;
}

//计算命中延迟时间
static float priCalcHitDelayTime(float dis, float ammo_speed, float shoot_delay_time)
{
    return dis / ammo_speed + shoot_delay_time;
}

//预测
static void priPredict(Point3D_t* pc, float* pyaw, float time, float vx, float vy, float vz, float vyaw)
{
    pc->x += time * vx;
    pc->y += time * vy;
    pc->z += time * vz;
    *pyaw += time * vyaw;
}

void ArmorPredictorInit(ArmorPredictor_t* me, float shoot_delay_time)
{
    me->shoot_delay_time = shoot_delay_time;
    me->ready_flag = RESET;
    memset(&me->output, 0, sizeof(ArmorPredictorOutput_t));
}

void ArmorPredictorReset(ArmorPredictor_t* me)
{
    me->ready_flag = RESET;
}

void ArmorPredictorNowStateUpdate(ArmorPredictor_t* me)
{
    if(me->ready_flag == SET && me->pc.tracking == 1)
    {
        //插值更新车体当前位置
        me->x_now += me->pc.vx * ARMOR_PREDICTOR_INTERVAL;
        me->y_now += me->pc.vy * ARMOR_PREDICTOR_INTERVAL;
        me->z_now += me->pc.vz * ARMOR_PREDICTOR_INTERVAL;
        me->yaw_now += me->pc.v_yaw * ARMOR_PREDICTOR_INTERVAL;
        me->z_2_now += me->pc.vz * ARMOR_PREDICTOR_INTERVAL;
    }
    
}

float iterate_time[4][ARMOR_PREDICTOR_ITERATE_MAX];
Point3D_t iterate_a[4][ARMOR_PREDICTOR_ITERATE_MAX];
void ArmorPredictorLoop(ArmorPredictor_t* me)
{
    if(me->ready_flag == SET && me->pc.tracking == 1)
    {
		me->output.tracking = 1;

        //装甲板块数
        int32_t armor_num;
        if(me->pc.task_mode == AIM_2_ARMOR)
        {
            armor_num = 2;
        }
        else if(me->pc.task_mode == AIM_3_ARMOR)
        {
            armor_num = 3;
        }
        else//不是2和3就当作4
        {
            armor_num = 4;
        }
        me->output.armor_num = armor_num;
		
        //迭代
        uint32_t use_1 = 1;
        int32_t i;
        int32_t j;

        Point3D_t c;
        Point3D_t c_pre;
        float yaw;
        float yaw_pre;
        float r;

        Point3D_t a;
        float dis;
        float time;

        for(i=0; i<armor_num; i++)
        {
            c.x = me->x_now;
            c.y = me->y_now;
            c.z = use_1 ? me->z_now : me->z_2_now;
            yaw = me->yaw_now + i * 2*PI / armor_num;
            r = use_1 ? me->pc.r1 : me->pc.r2;

            priCenter2Armor(c, yaw, r, &a);
            me->armor_now[i] = a;
            dis = priArmor2Dis(a);
            c_pre = c;
            yaw_pre = yaw;
            time = 0;

            for(j=0; j<ARMOR_PREDICTOR_ITERATE_MAX; j++)
            {
                time = priCalcHitDelayTime(dis, me->input.ammo_speed, me->shoot_delay_time);
                iterate_time[i][j] = time;
                c_pre = c;
                yaw_pre = yaw;
                priPredict(&c_pre, &yaw_pre, time, me->pc.vx, me->pc.vy, me->pc.vz, me->pc.v_yaw);
                priCenter2Armor(c_pre, yaw_pre, r, &a);
                dis = priArmor2Dis(a);
                iterate_a[i][j] = a;
            }

            me->output.armor_predict[i] = a;
            me->output.yaw_predict[i] = yaw_pre;
            me->output.dis_predict[i] = dis;
            me->output.time_predict[i] = time;
            me->output.center_predict[i] = c_pre;

            if(armor_num == 4)//仅装甲板为4块时才用两个r和z值，否则都只用1
            {
                use_1 ^= 1;
            }
        }
    }
	else
	{
		me->output.tracking = 0;
	}
}

void ArmorPredictorSetInput(ArmorPredictor_t* me, ArmorPredictorInput_t* pin, PCRecvPacketArmor_t* ppc)
{
    me->input = *pin;
    me->pc = *ppc;

    me->ready_flag = SET;

    me->x_now = me->pc.x;
    me->y_now = me->pc.y;
    me->z_now = me->pc.z;
    me->yaw_now = me->pc.yaw;
    me->z_2_now = me->pc.z_2;
}
void ArmorPredictorGetOutput(ArmorPredictor_t* me, ArmorPredictorOutput_t* pout)
{
    if(me->ready_flag == SET && me->pc.tracking == 1)
    {
        *pout = me->output;
    }
    else
    {
        pout->tracking = 0;
    }
}

