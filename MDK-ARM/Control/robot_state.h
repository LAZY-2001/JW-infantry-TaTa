
#ifndef _STATE
#define _STATE

typedef enum {
	chassis_follow,
	chassis_rotate,
	chassis_lock,
}chassis_gimbal_e;

typedef enum {
	shoot_one_bullet =0,
	shoot_continuous=1,

}shoot_state_e;

typedef enum {
	armor_aim='a',
	sbuff_aim='s',
	bbuff_aim='b',
	no_aim,
}aiming_state_e;

typedef enum {
	gate_on,
	gate_off,
}gate_state_e;

typedef enum {
	shoot_freq_high,
	shoot_freq_low,
}shoot_freq_state_e;


typedef enum {
	RC,
	KB,
}control_state_e;


typedef enum {
	block,
	normal,
}ammo_state_e;

typedef enum {
	fric_on,
	fric_off,
}fric_state_e;

typedef struct {
	chassis_gimbal_e  chassis_gimbal;
	aiming_state_e    aiming_state;
  gate_state_e      gate_state;
	shoot_freq_state_e  shoot_freq;
	control_state_e   control_state;
	ammo_state_e      ammo_state;
  fric_state_e      fric_state;
	shoot_state_e     shoot_state;
}robot_t;

extern robot_t robot;
extern robot_t last_robot;
#endif


