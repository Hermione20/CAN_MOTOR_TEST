#include "stm32f4xx.h"                  // Device header
#include "can.h"
#include "led.h"
#include "delay.h"

//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,
//�������CAN1_Mode_Init(CAN_SJW_1tq, CAN_BS2_6tq, CAN_BS1_7tq, 3, CAN_Mode_LoopBack),
//������Ϊ:42M/((6+7+1)*3)=1Mbps.
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 

void CAN1_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��

//���Ÿ���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11����ΪCAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12����ΪCAN1

//��ʼ��GPIO
	GPIO_InitTypeDef GPIO_InitStructure; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11,PA12
//��ʼ��CAN1
	CAN_InitTypeDef CAN_InitStructure;
	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������

	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	 //ģʽ���� 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=3;  //��Ƶϵ��(Fdiv)Ϊbrp+1	

	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 
//���ù�����
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
//��ʼ��CAN1(Receive)NVIC�ж�
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
	CanTxMsg TxMessage;

u8 CAN1_Send_Msg(u8* msg,int stdID)
{	
	u8 mbox;
	u16 i=0;
	TxMessage.StdId=stdID;	 // ��׼��ʶ��
	TxMessage.ExtId=0x12;	 // ������չ��ʾ����29λ��
	TxMessage.IDE=CAN_ID_STD;		  // ʹ����չ��ʶ��
	TxMessage.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC=8;							 // ������Ϣ����
	for(i=0;i<8;i++)
		TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
	mbox=CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	if(i>=0XFFF)	return 1;
		return 0;		
}

void CAN1_M3508_2006(int current)
{
	u8 M3508_2006[8];	
	M3508_2006[0] = (uint8_t)(current >> 8) ;
	M3508_2006[1] = (uint8_t)current ;
	M3508_2006[2] = (uint8_t)(current >> 8) ;
	M3508_2006[3] = (uint8_t)current ;
	M3508_2006[4] = (uint8_t)(current >> 8) ;
	M3508_2006[5] = (uint8_t)current ;
	M3508_2006[6] = (uint8_t)(current >> 8) ;
	M3508_2006[7] = (uint8_t)current ;
	CAN1_Send_Msg(M3508_2006,0x200);//���ñ�ʶ����ID1~4Ϊ0x200��5~8Ϊ0x1FF
}

void CAN1_GM6020(int current)
{
	u8 GM6020[8];
	GM6020[0] = (current >> 8) ;
	GM6020[1] = current ;
	GM6020[2] = (current >> 8) ;
	GM6020[3] = current ;
	GM6020[4] = (current >> 8) ;
	GM6020[5] = current ;
	GM6020[6] = (current >> 8) ;
	GM6020[7] = current ;
	CAN1_Send_Msg(GM6020,0x1FF);//���ñ�ʶ����ID1~4Ϊ0x1FF��5~7Ϊ0x2FF
}

void CAN1_GM6020_p(int current)
{
	u8 GM6020[8];
	GM6020[0] = (current >> 8) ;
	GM6020[1] = current ;
	GM6020[2] = (current >> 8) ;
	GM6020[3] = current ;
	GM6020[4] = (current >> 8) ;
	GM6020[5] = current ;
	CAN1_Send_Msg(GM6020,0x2FF);//���ñ�ʶ����ID1~4Ϊ0x1FF��5~7Ϊ0x2FF
}

void CAN1_MF9025(int32_t speed)
{
	u8 MF9025[8];
	MF9025[0] = 0xA1 ;
	MF9025[1] = 0x00 ;
	MF9025[2] = 0x00 ;
	MF9025[3] = 0x00 ;
	MF9025[4] = (uint8_t)speed ;
	MF9025[5] = (uint8_t)(speed >> 8);
	MF9025[6] = (uint8_t)(speed >> 16);
	MF9025[7] = (uint8_t)(speed >> 24);
//	MF9025[0] = 0x19 ;
//	MF9025[1] = 0x00 ;
//	MF9025[2] = 0x00 ;
//	MF9025[3] = 0x00 ;
//	MF9025[4] = 0x00 ;
//	MF9025[5] = 0x00;
//	MF9025[6] = 0x00;
//	MF9025[7] = 0x00;
	CAN1_Send_Msg(MF9025,0x141);//���ñ�ʶ��,0x140+ID		(uint8_t)speed		*(uint8_t*)(&speed)
}

//�жϷ�����			  
CanRxMsg rx_message;

void CAN1_RX0_IRQHandler(void)
{
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
    {

				
			CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);//|CAN_IT_FF0|CAN_IT_FOV0
      
			CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
		
			//������������ݴ���

	 		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
		
		CAN_ClearITPendingBit(CAN1, CAN_IT_EWG|CAN_IT_EPV|CAN_IT_BOF|CAN_IT_LEC|CAN_IT_ERR);
}


