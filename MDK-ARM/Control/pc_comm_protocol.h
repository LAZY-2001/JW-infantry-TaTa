#ifndef __PC_COMM_PROTOCOL_H
#define __PC_COMM_PROTOCOL_H

#include "stm32f4xx_hal.h"

/*HEADER����*/
enum
{
    PC_SEND_HEADER        = 0x5A,
    PC_RECV_ARMOR_HEADER  = 0xA5,
    PC_RECV_MOVE_HEADER   = 0xB5,
    PC_RECV_ENERGY_HEADER = 0xC5
};

/*mode����*/
enum
{
    PC_MODE_SMALL_ENERGY = 0,
    PC_MODE_BIG_ENERGY,
    PC_MODE_ARMOR
};

/*����Э��*/
typedef __packed struct PCSendPacketTag
{
    uint8_t header;
    uint8_t enemy_color : 1;  // �з���ɫ  0-red 1-blue
    uint8_t game_status : 3;  // ����״̬���ڱ�ר��
    
    uint8_t reset_tracker : 1;   // ���ø����������Բ��� 0-ʲô�����ᷢ�� 1-�Ӿ�װ�װ��������λ�������ǰ����״̬
    uint8_t chassis_status : 1;  // ��������  0-unlock 1-lock
    uint8_t outpost_status : 1;  // ǰ��վ״̬���ڱ�ר��
    uint8_t reserved : 1;
    uint8_t mode;  // SMALL_ENERGY:0  BIG_ENERGY:1  ARMOR:2
    //��ǰ��̨pit��yaw
    float curr_pitch;
    float curr_yaw;
    //��ǰ������׼��װ�װ�λ�ã������Ӿ�����
//    float aim_x;
//    float aim_y;
//    float aim_z;
    //�ڱ�ר��
    int16_t v_liner_x;    // Linear speed in x direction. >0: forward | <0: backward (cm/s)
    int16_t v_liner_y;    // Linear speed in y direction. 0 if differential.(cm/s)
    int16_t v_angular_z;  // Angular speed in z direction >0: turn left | <0: turn right (0.01 rad/s)
    int16_t odom_yaw;     // Odometry theta (0.01 rad)
    //У���
    uint16_t checksum;
}PCSendPacket_t;

/*task_mode���壬0-Ŀ����4��װ�װ� 1-3�� 2-2��*/
enum
{
    AIM_4_ARMOR = 0,
    AIM_3_ARMOR,
    AIM_2_ARMOR
};

/*װ�װ������������*/
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

/*�ڱ�ר����������*/
typedef __packed struct PCRecvPacketMoveTag
{
    float linear_x;   // (m/s)
    float linear_y;   // (m/s)
    float angular_z;  // (rad/s)
}PCRecvPacketMove_t;

/*�������ؽ�������*/
typedef __packed struct PCRecvPacketEnergyTag
{
    uint8_t chassis_lock : 1;
    uint8_t reserved : 7;

    float energy_pitch;
    float energy_yaw;
}PCRecvPacketEnergy_t;

/*����Э��*/
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

