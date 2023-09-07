#ifndef __CAN_H
#define __CAN_H	 

#include "stm32f4xx.h"

void CAN1_Init(void);			//CAN��ʼ��
 
u8 CAN1_Send_Msg(u8* msg,int stdID);				//��������
u8 CAN1_Receive_Msg(u8 *buf);			//��������
void CAN1_RX0_IRQHandler(void);

void CAN1_M3508_2006(int current);			//M3508/M2006���
void CAN1_GM6020(int current);		//GM6020���
void CAN1_GM6020_p(int current);
void CAN1_MF9025(int32_t speed);



#endif


