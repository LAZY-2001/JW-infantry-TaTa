#ifndef __ARMOR_SIGHT_H
#define __ARMOR_SIGHT_H

#include "stm32f4xx_hal.h"
#include "armor_predictor.h"

typedef struct ArmorSightInputTag
{
    float ammo_speed;//弹丸初速度的估计值，非弹速上限
    float yaw;//rad，逆时针正，RF2PI类型
    float pit;//rad，低头正，水平为0
    float traj_yaw_offset;//yaw弹道补偿量，逆时针正，弧度
    float traj_pit_offset;//pit弹道补偿量，低头正，弧度
}ArmorSightInput_t;

typedef struct ArmorSightOutputTag
{
    uint8_t tracking : 1;
    uint8_t aim_shoot : 1;//自动开火
    float yaw_target;//rad，逆时针正，RF2PI类型
    float pit_target;//rad，低头正，水平为0
    Point3D_t aim_armor;//当前正在瞄的装甲板位置
}ArmorSightOutput_t;

typedef struct ArmorSightTag
{
    float aim_shoot_yaw_thre;//自动射击阈值，单位米
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
