#include "armor_sight.h"
#include "ring_math.h"
#include "arm_math.h"
#include "misc.h"
#include "math.h"
#include "bullet_model.h"

ArmorSight_t armor_sight;

void ArmorSightLoop(ArmorSight_t* me)
{
    if(me->ready_flag == SET && me->predict_result.tracking == 1)
    {
        int32_t i;

        float valid_area;
        if(me->predict_result.armor_num == 4)
        {
            valid_area = 0.3f * PI;
        }
        else
        {
            valid_area = 0.4f * PI;
        }

        //所有数据均在视觉云台惯性坐标系下计算（前x 左y 上z）

        //m 云台本身；a 预测的装甲板；c 预测的车体中心；t击打装甲板所需的目标值
        float m_yaw;//自己的yaw
        float m_pit;//自己的pit

        float ac_yaw;//armor指向center的向量的yaw
        float t_yaw;//armor的目标yaw
        float t_pit;//armor的目标pit

        float c_yaw;//center的yaw

        float c_ac_yaw_err_abs;//c_yaw和ac_yaw的差值的绝对值
        float m_t_yaw_err_abs;//m_yaw和t_yaw的差值的绝对值
        float m_t_pit_err_abs;//m_pit和t_pit的差值的绝对值
        float m_t_err_abs_larger;//m和t的两个差值的绝对值中较大的那个
        float m_t_err_abs_larger_min = 2*PI;//m和t的两个差值的绝对值中较大的那个中最小的那个

        uint32_t selected_armor_index = 4;//选中为目标的装甲板
        float target_t_yaw;//目标值
        float target_t_pit;
        float target_c_ac_yaw_err_abs;//目标对应的各种差值的绝对值
        float target_m_t_yaw_err_abs;
        float target_m_t_pit_err_abs;

        //自己的pit和yaw
        m_yaw = me->input.yaw;
        m_pit = me->input.pit;

        for(i =0; i < me->predict_result.armor_num; i++)
        {
            //计算center的yaw值
            c_yaw = atan2f(me->predict_result.center_predict[i].y, me->predict_result.center_predict[i].x);
            c_yaw = RingConvertToRF(2*PI, c_yaw);

            //检查装甲板是否在靠近自己的半幅，如果不在，则该装甲板不能作为目标
            ac_yaw = RingConvertToRF(2*PI, me->predict_result.yaw_predict[i]);
            c_ac_yaw_err_abs = MISC_Abs(RingCloseSubRF(2*PI, c_yaw, ac_yaw));
            if(c_ac_yaw_err_abs > valid_area)
            {
                continue;
            }

            //计算击打装甲板的目标yaw值
            t_yaw = atan2f(me->predict_result.armor_predict[i].y, me->predict_result.armor_predict[i].x) + me->input.traj_yaw_offset;
            t_yaw = RingConvertToRF(2*PI, t_yaw);
            m_t_yaw_err_abs = MISC_Abs(RingCloseSubRF(2*PI, m_yaw, t_yaw));

            //计算击打装甲板的目标pit值
            float xy_dis;
            arm_sqrt_f32(me->predict_result.armor_predict[i].x * me->predict_result.armor_predict[i].x +\
                        me->predict_result.armor_predict[i].y * me->predict_result.armor_predict[i].y, &xy_dis);
            t_pit = BulletModelGetPitch(xy_dis, me->predict_result.armor_predict[i].z, me->input.ammo_speed);
            t_pit = -t_pit + me->input.traj_pit_offset;
            m_t_pit_err_abs = MISC_Abs(m_pit - t_pit);

            //取pit和yaw两个差值的绝对值中较大的一个
            m_t_err_abs_larger = m_t_yaw_err_abs>m_t_pit_err_abs ? m_t_yaw_err_abs : m_t_pit_err_abs;

            //如果较大的这个差值不是最小的，则将该装甲板作为目标
            if(m_t_err_abs_larger < m_t_err_abs_larger_min)
            {
                m_t_err_abs_larger_min = m_t_err_abs_larger;
                target_t_pit = t_pit;
                target_t_yaw = t_yaw;
                target_c_ac_yaw_err_abs = c_ac_yaw_err_abs;
                target_m_t_yaw_err_abs = m_t_yaw_err_abs;
                target_m_t_pit_err_abs = m_t_pit_err_abs;
                selected_armor_index = i;
            }
        }

        //如果找到了目标，则瞄准该目标，并判断是否射击
        if(selected_armor_index < 4)
        {
            //瞄准目标
            me->output.tracking = 1;
            me->output.yaw_target = target_t_yaw;
            me->output.pit_target = target_t_pit;
            me->output.aim_armor = me->predict_result.armor_predict[selected_armor_index];

            //如果pit和yaw方向均符合射击条件，则射击
            if( target_m_t_pit_err_abs * me->predict_result.dis_predict[selected_armor_index] < me->aim_shoot_pit_thre &&\
                target_m_t_yaw_err_abs * me->predict_result.dis_predict[selected_armor_index] < me->aim_shoot_yaw_thre * arm_cos_f32(target_c_ac_yaw_err_abs))
            {
                me->output.aim_shoot = 1;
            }
            else
            {
                me->output.aim_shoot = 0;
            }
        }
        else
        {
            me->output.tracking = 0;
            me->output.aim_shoot = 0;
        }
    }
    else
    {
        me->output.tracking = 0;
        me->output.aim_shoot = 0;
    }
}
void ArmorSightInit(ArmorSight_t* me, float aim_shoot_yaw_thre, float aim_shoot_pit_thre)
{
    me->aim_shoot_yaw_thre = aim_shoot_yaw_thre;
    me->aim_shoot_pit_thre = aim_shoot_pit_thre;

    me->ready_flag = RESET;

    me->output.tracking = 0;
    me->output.aim_shoot = 0;
}
void ArmorSightReset(ArmorSight_t* me)
{
    me->ready_flag = RESET;

    me->output.tracking = 0;
    me->output.aim_shoot = 0;
}
void ArmorSightSetInput(ArmorSight_t* me, ArmorSightInput_t* pin, ArmorPredictorOutput_t* ppre, float PC_yaw)
{
    me->input = *pin;
    me->predict_result = *ppre;

    me->ready_flag = SET;
		me->PC_receive_yaw = PC_yaw;
}
void ArmorSightGetOutput(ArmorSight_t* me, ArmorSightOutput_t* pout)
{
    if(me->ready_flag == SET)
    {
        *pout = me->output;
    }
    else
    {
        pout->tracking = 0;
        pout->aim_shoot = 0;
    }
}
