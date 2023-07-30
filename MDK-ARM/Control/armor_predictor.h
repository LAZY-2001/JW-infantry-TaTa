#ifndef __ARMOR_PREDICTOR_H
#define __ARMOR_PREDICTOR_H

#include "stm32f4xx_hal.h"
#include "pc_comm_protocol.h"

typedef struct Point3DTag
{
    float x;
    float y;
    float z;
}Point3D_t;

typedef struct ArmorPredictorInputTag
{
    float ammo_speed;//弹丸初速度的估计值，非弹速上限
}ArmorPredictorInput_t;

typedef struct ArmorPredictorOutputTag
{
    uint8_t tracking : 1;//是否预测到目标，数据有效标志
    int32_t armor_num;//装甲板数量
    Point3D_t armor_predict[4];//armor预测位置
    float yaw_predict[4];//armor指向center的向量的预测角度 rad 非Ring类型
    float dis_predict[4];//armor的预测距离
    float time_predict[4];//预测时间长度，即预测结果是该时间之后的装甲板位置
    Point3D_t center_predict[4];//center预测位置
}ArmorPredictorOutput_t;

typedef struct ArmorPredictorTag
{
    float shoot_delay_time;

    PCRecvPacketArmor_t pc;
    ArmorPredictorInput_t input;

    FlagStatus ready_flag;

    float x_now;
    float y_now;
    float z_now;
    float yaw_now;
    float z_2_now;

    Point3D_t armor_now[4];

    ArmorPredictorOutput_t output;
}ArmorPredictor_t;

extern ArmorPredictor_t armor_predictor;

void ArmorPredictorInit(ArmorPredictor_t* me, float shoot_delay_time);
void ArmorPredictorReset(ArmorPredictor_t* me);
void ArmorPredictorNowStateUpdate(ArmorPredictor_t* me);
void ArmorPredictorLoop(ArmorPredictor_t* me);
void ArmorPredictorSetInput(ArmorPredictor_t* me, ArmorPredictorInput_t* pin, PCRecvPacketArmor_t* ppc);
void ArmorPredictorGetOutput(ArmorPredictor_t* me, ArmorPredictorOutput_t* pout);

#endif /* __ARMOR_PREDICTOR_H */
