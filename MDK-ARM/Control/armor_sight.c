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

        //�������ݾ����Ӿ���̨��������ϵ�¼��㣨ǰx ��y ��z��

        //m ��̨����a Ԥ���װ�װ壻c Ԥ��ĳ������ģ�t����װ�װ������Ŀ��ֵ
        float m_yaw;//�Լ���yaw
        float m_pit;//�Լ���pit

        float ac_yaw;//armorָ��center��������yaw
        float t_yaw;//armor��Ŀ��yaw
        float t_pit;//armor��Ŀ��pit

        float c_yaw;//center��yaw

        float c_ac_yaw_err_abs;//c_yaw��ac_yaw�Ĳ�ֵ�ľ���ֵ
        float m_t_yaw_err_abs;//m_yaw��t_yaw�Ĳ�ֵ�ľ���ֵ
        float m_t_pit_err_abs;//m_pit��t_pit�Ĳ�ֵ�ľ���ֵ
        float m_t_err_abs_larger;//m��t��������ֵ�ľ���ֵ�нϴ���Ǹ�
        float m_t_err_abs_larger_min = 2*PI;//m��t��������ֵ�ľ���ֵ�нϴ���Ǹ�����С���Ǹ�

        uint32_t selected_armor_index = 4;//ѡ��ΪĿ���װ�װ�
        float target_t_yaw;//Ŀ��ֵ
        float target_t_pit;
        float target_c_ac_yaw_err_abs;//Ŀ���Ӧ�ĸ��ֲ�ֵ�ľ���ֵ
        float target_m_t_yaw_err_abs;
        float target_m_t_pit_err_abs;

        //�Լ���pit��yaw
        m_yaw = me->input.yaw;
        m_pit = me->input.pit;

        for(i =0; i < me->predict_result.armor_num; i++)
        {
            //����center��yawֵ
            c_yaw = atan2f(me->predict_result.center_predict[i].y, me->predict_result.center_predict[i].x);
            c_yaw = RingConvertToRF(2*PI, c_yaw);

            //���װ�װ��Ƿ��ڿ����Լ��İ����������ڣ����װ�װ岻����ΪĿ��
            ac_yaw = RingConvertToRF(2*PI, me->predict_result.yaw_predict[i]);
            c_ac_yaw_err_abs = MISC_Abs(RingCloseSubRF(2*PI, c_yaw, ac_yaw));
            if(c_ac_yaw_err_abs > valid_area)
            {
                continue;
            }

            //�������װ�װ��Ŀ��yawֵ
            t_yaw = atan2f(me->predict_result.armor_predict[i].y, me->predict_result.armor_predict[i].x) + me->input.traj_yaw_offset;
            t_yaw = RingConvertToRF(2*PI, t_yaw);
            m_t_yaw_err_abs = MISC_Abs(RingCloseSubRF(2*PI, m_yaw, t_yaw));

            //�������װ�װ��Ŀ��pitֵ
            float xy_dis;
            arm_sqrt_f32(me->predict_result.armor_predict[i].x * me->predict_result.armor_predict[i].x +\
                        me->predict_result.armor_predict[i].y * me->predict_result.armor_predict[i].y, &xy_dis);
            t_pit = BulletModelGetPitch(xy_dis, me->predict_result.armor_predict[i].z, me->input.ammo_speed);
            t_pit = -t_pit + me->input.traj_pit_offset;
            m_t_pit_err_abs = MISC_Abs(m_pit - t_pit);

            //ȡpit��yaw������ֵ�ľ���ֵ�нϴ��һ��
            m_t_err_abs_larger = m_t_yaw_err_abs>m_t_pit_err_abs ? m_t_yaw_err_abs : m_t_pit_err_abs;

            //����ϴ�������ֵ������С�ģ��򽫸�װ�װ���ΪĿ��
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

        //����ҵ���Ŀ�꣬����׼��Ŀ�꣬���ж��Ƿ����
        if(selected_armor_index < 4)
        {
            //��׼Ŀ��
            me->output.tracking = 1;
            me->output.yaw_target = target_t_yaw;
            me->output.pit_target = target_t_pit;
            me->output.aim_armor = me->predict_result.armor_predict[selected_armor_index];

            //���pit��yaw�����������������������
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
