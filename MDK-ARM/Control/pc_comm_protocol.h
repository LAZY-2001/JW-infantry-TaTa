#ifndef __PC_COMM_PROTOCOL_H
#define __PC_COMM_PROTOCOL_H

#include "stm32f4xx_hal.h"

/*HEADER定义*/
enum
{
    PC_SEND_HEADER        = 0x5A,
    PC_RECV_ARMOR_HEADER  = 0xA5,
    PC_RECV_MOVE_HEADER   = 0xB5,
    PC_RECV_ENERGY_HEADER = 0xC5
};

/*mode定义*/
enum
{
    PC_MODE_SMALL_ENERGY = 0,
    PC_MODE_BIG_ENERGY,
    PC_MODE_ARMOR
};

/*发送协议*/
typedef __packed struct PCSendPacketTag
{
    uint8_t header;
    uint8_t enemy_color : 1;  // 敌方颜色  0-red 1-blue
    uint8_t game_status : 3;  // 比赛状态，哨兵专属
    
    uint8_t reset_tracker : 1;   // 重置跟踪器，可以不管 0-什么都不会发生 1-视觉装甲板自瞄程序复位，解除当前自瞄状态
    uint8_t chassis_status : 1;  // 底盘锁定  0-unlock 1-lock
    uint8_t outpost_status : 1;  // 前哨站状态，哨兵专属
    uint8_t reserved : 1;
    uint8_t mode;  // SMALL_ENERGY:0  BIG_ENERGY:1  ARMOR:2
    //当前云台pit和yaw
    float curr_pitch;
    float curr_yaw;
    //当前正在瞄准的装甲板位置，便于视觉调试
//    float aim_x;
//    float aim_y;
//    float aim_z;
    //哨兵专用
    int16_t v_liner_x;    // Linear speed in x direction. >0: forward | <0: backward (cm/s)
    int16_t v_liner_y;    // Linear speed in y direction. 0 if differential.(cm/s)
    int16_t v_angular_z;  // Angular speed in z direction >0: turn left | <0: turn right (0.01 rad/s)
    int16_t odom_yaw;     // Odometry theta (0.01 rad)
    //校验和
    uint16_t checksum;
}PCSendPacket_t;

/*task_mode定义，0-目标有4块装甲板 1-3块 2-2块*/
enum
{
    AIM_4_ARMOR = 0,
    AIM_3_ARMOR,
    AIM_2_ARMOR
};

/*装甲板自瞄接收数据*/
typedef __packed struct PCRecvPacketArmorTag
{
    uint8_t tracking : 1;
    uint8_t task_mode : 2;
    uint8_t reserve : 5;
    float x;
    float y;
    float z;
    float yaw;
    float vx;
    float vy;
    float vz;
    float v_yaw;
    float r1;
    float r2;
    float z_2;
}PCRecvPacketArmor_t;

/*哨兵专属接收数据*/
typedef __packed struct PCRecvPacketMoveTag
{
    float linear_x;   // (m/s)
    float linear_y;   // (m/s)
    float angular_z;  // (rad/s)
}PCRecvPacketMove_t;

/*能量机关接收数据*/
typedef __packed struct PCRecvPacketEnergyTag
{
    uint8_t chassis_lock : 1;
    uint8_t reserved : 7;

    float energy_pitch;
    float energy_yaw;
}PCRecvPacketEnergy_t;

/*接收协议*/
typedef __packed struct PCRecvPacketTag
{
    uint8_t header;
    __packed union
    {
        PCRecvPacketArmor_t packetArmor;
        PCRecvPacketMove_t packetMove;
        PCRecvPacketEnergy_t packetEnergy;
    }data;
    uint16_t checksum;
}PCRecvPacket_t;

#endif /* __PC_COMM_PROTOCOL_H */

