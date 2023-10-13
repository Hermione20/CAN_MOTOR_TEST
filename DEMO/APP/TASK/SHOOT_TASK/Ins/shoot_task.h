#ifndef __SHOOT_TASK_H
#define __SHOOT_TASK_H

#include "public.h"

/********** pid�ṹ��˵�� ************
	��ǹ�ܷ��������Ħ����
		pid_friction_whell_speed[0]
		pid_friction_whell_speed[1]
	�ձ�˫ǹ�ܣ�Ħ����
		pid_friction_whell_speed[2]
		pid_friction_whell_speed[3]
	Ӣ���²���
		pid_42mm_poke_speed
	Ӣ���ϲ���
		pid_42mm_poke2_angle
		pid_42mm_poke2_speed
	�������ɻ����ձ�����
		pid_17mm_poke_angle
		pid_17mm_poke_speed
**************************************/

/********** ����������ṹ��˵�� *********
	��ǹ�ܷ��������Ħ����
		general_friction.left_motor1
		general_friction.right_motor1
	�ձ�˫ǹ�ܣ�Ħ����
		general_friction.left_motor2
		general_friction.right_motor2
	Ӣ���²��̣��ڱ����̣���������
		general_poke.left_poke
	Ӣ���ϲ��̣��ڱ��Ҳ���
		general_poke.right_poke
**************************************/
//�����������ѡ��
#define TYPE 1			//1��42mm		2��17mm		3:17mm x2
#define STANDARD 3
// <o> STANDARD  - ���Ų���
// <3=> NUM_3
// <4=> NUM_4
// <5=> NUM_5
#define STANDARD 			6
//ѡ�񲽱�Ϊ3�� ����4��

//Ħ����ת�١���42mm
#define FRICTION_SPEED_10 2000
//#define FRICTION_SPEED_12 2100
//#define FRICTION_SPEED_14 2575
#define FRICTION_SPEED_16 3200

#define POKE_SPEED 200			//Ӣ���²���ת��
#define POKE_MAX_OUT 6000		//Ӣ���²�����������
#define ONE_POKE_ANGLE_42 90.0f	//42mm��������Ƕ�
#define ONE_POKE_ANGLE_17 30.0f	//17mm��������Ƕ�
#define k_speed	0.005


//Ħ����ת�١���17mm
#define FRICTION_SPEED_15 2000
#define FRICTION_SPEED_18 2100
#define FRICTION_SPEED_30 2575

#define SINGLE_ANGLE  60

typedef enum
{
	Stop=0,
  NORMAL=1,
	BACK=2,
  LOCK=3,
} friction_state_t ;  //1���� 2��ת

typedef enum
{
	NOSHOOTING=0,
	SHOOTINT=1,
}	shoot_state_e;		//0������	1����

typedef enum
{
  outburst  = 0,
  cooling   = 1,
  speed     = 2,
} shoot_mode_selection_e;		//��������ģʽѡ��

void shoot_task(void);		//���������ܳ�
void Mode_switch(void);		//��������ģʽѡ��

static void Shoot_42mm_speed_Select(void);		//42mm����
static void Shoot_17mm_speed_Select(void);		//17mm����
static void Shoot_17mm_speed_Select_double(void);//17mm����,˫ǹ��

void heat1_limit_42mm(void);								//��������,42mm
void heat1_limit_17mm(void);								//��������,17mm
void heat1_limit_17mm_double(void);					//��������,17mm,˫ǹ��

void shoot_friction_handle_42(void);				//Ħ���ֲ��֣�42mm
void shoot_friction_handle_17(void);				//Ħ���ֲ��֣�17mm
void shoot_friction_handle_17_double(void);	//Ħ���ֲ��֣�17mm,˫ǹ��

void shoot_bullet_handle_42(void);					//42mm���̲���

void friction_lock(void);										//Ħ���ֶ�ת����
void Heat0_switch(void);


void shot_param_init(void);									//�������PID������ʼ��

#endif
