#ifndef __ARMOR_SIGHT_H
#define __ARMOR_SIGHT_H

#include "stm32f4xx_hal.h"
#include "armor_predictor.h"

typedef struct ArmorSightInputTag
{
    float ammo_speed;//������ٶȵĹ���ֵ���ǵ�������
    float yaw;//rad����ʱ������RF2PI����
    float pit;//rad����ͷ����ˮƽΪ0
    float traj_yaw_offset;//yaw��������������ʱ����������
    float traj_pit_offset;//pit��������������ͷ��������
}ArmorSightInput_t;

typedef struct ArmorSightOutputTag
{
    uint8_t tracking : 1;
    uint8_t aim_shoot : 1;//�Զ�����
    float yaw_target;//rad����ʱ������RF2PI����
    float pit_target;//rad����ͷ����ˮƽΪ0
    Point3D_t aim_armor;//��ǰ�������װ�װ�λ��
}ArmorSightOutput_t;

typedef struct ArmorSightTag
{
    float aim_shoot_yaw_thre;//�Զ������ֵ����λ��
    float aim_shoot_pit_thre;

    FlagStatus ready_flag;

    ArmorSightInput_t input;
    ArmorPredictorOutput_t predict_result;
		float PC_receive_yaw;
	
    ArmorSightOutput_t output;
}ArmorSight_t;

extern ArmorSight_t armor_sight;

void ArmorSightLoop(ArmorSight_t* me);
void ArmorSightInit(ArmorSight_t* me, float aim_shoot_yaw_thre, float aim_shoot_pit_thre);
void ArmorSightReset(ArmorSight_t* me);
void ArmorSightSetInput(ArmorSight_t* me, ArmorSightInput_t* pin, ArmorPredictorOutput_t* ppre, float PC_yaw);
void ArmorSightGetOutput(ArmorSight_t* me, ArmorSightOutput_t* pout);

#endif /* __ARMOR_SIGHT_H */
