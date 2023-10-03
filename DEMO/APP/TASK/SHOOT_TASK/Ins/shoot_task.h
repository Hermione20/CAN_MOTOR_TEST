#ifndef __SHOOT_TASK_H
#define __SHOOT_TASK_H

#include "public.h"

//Ħ����ת�١���42mm
#define FRICTION_SPEED_10 2000
#define FRICTION_SPEED_12 2100
#define FRICTION_SPEED_14 2575
#define FRICTION_SPEED_16 3200

typedef enum
{
	STOP=0��
  NOMAL=1,
  LOCK=2,
} shoot_state_e;  //0���� 1��ת




void shoot_task(void);											//���������ܳ�
static void Shoot_42mm_speed_Select(void);	//42mm����
void heat1_limit(void);											//��������
void shoot_bullet_handle(void);							//���̲���
void shoot_friction_handle(void);						//Ħ���ֲ���

void friction_lock(void);										//Ħ���ֶ�ת����


void shot_param_init(void);									//�������PID������ʼ��


#endif
