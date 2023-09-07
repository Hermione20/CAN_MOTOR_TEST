#include "stm32f4xx.h"                  // Device header
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "can.h"

uint8_t usart_RxData;
uint8_t usart_RxFlag;
uint8_t CAN_RxData[8];
uint8_t CAN_RxMsgDLC;
int i=0;


int main(void)
{ 
	delay_init(168);	//��ʼ����ʱ����
	uart_Init();			//��ʼ������,������Ϊ115200
	LED_Init();				//��ʼ��LED 
	CAN1_Init();			//CAN��ʼ������ģʽ,������1Mbps

	CAN1_MF9025(-50);		//�����ٶ�

	while(1)
	{

    i++;
    if(i%84000 == 0)
   {


			CAN1_M3508_2006(500);	//ת��M3508�������������÷�Χ-16384~0~16384����Ӧ��Χ-20A~20A,819.2=1A
													//ת��M2006�������������÷�Χ-10000~0~10000����Ӧ��Χ-10A~10A,1000=1A
			CAN1_GM6020(15000);	//ת��GM6020��������ѹ�������÷�Χ-30000~0~30000
//			CAN1_GM6020_p(15000);	//ת��GM6020��������ѹ�������÷�Χ-30000~0~30000

			i=0;
    }
	}
}
