#ifndef __SHOOT_TASK_H
#define __SHOOT_TASK_H

#include "public.h"

//Ħ����ת�١���42mm
#define FRICTION_SPEED_10 2000
#define FRICTION_SPEED_12 2100
#define FRICTION_SPEED_14 2575
#define FRICTION_SPEED_16 3200

#define POKE_SPEED 200			//Ӣ���²���ת��
#define POKE_MAX_OUT 6000		//Ӣ���²�����������
#define ONE_POKE_ANGLE_2 90.0f	//Ӣ���ϲ��̵�������Ƕ�
#define k_speed	0.005


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

void shoot_task(void);											//���������ܳ�
static void Shoot_42mm_speed_Select(void);	//42mm����
void heat1_limit(void);											//��������
void shoot_friction_handle(void);						//Ħ���ֲ���
void shoot_bullet_handle(void);							//���̲���

void friction_lock(void);										//Ħ���ֶ�ת����


void shot_param_init(void);									//�������PID������ʼ��

#endif
