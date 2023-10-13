#include "CanBus.h"


/**
  ******************************************************************************
  * @file    CanBus.c
  * @author  Lee_ZEKAI
  * @version V1.1.0
  * @date    03-October-2023
  * @brief   ���ļ���������can���ߵķ������������������ģ��id��ȥͷ�ļ�����
						 
	* @notice  �й�can��ģ��Ľ��պ�����ʹ�����Ʋ���can_bus.c�ļ��в������еĽ���
						 ������ѡ��ģ��id�����ý��㺯����Ҳ����can_bus.h�ļ����޸�ģ��id
						 �й�can�ķ��ͺ�����ʹ��Ҳ�Ʋ���can_bus.c�ļ��������ܷ�����������
						 ����Ҫ���͵ĺ���������ؽ�can_bus_send_task�����ĵ��÷���controltask
						 �С�
@verbatim
 ===============================================================================
 **/
 
 


void Can1ReceiveMsgProcess(CanRxMsg * msg)
{
    switch (msg->StdId)
    {
			case LEFT_FRICTION1:
			{
				M3508orM2006EncoderTask(&general_friction.left_motor1,msg);
			}break;
			case RIGHT_FRICTION1:
			{
				M3508orM2006EncoderTask(&general_friction.right_motor1,msg);
			}break;
		#if TYPE==1
			case POKE_2:
			{
				M3508orM2006EncoderTask(&general_poke.right_poke,msg);
			}break;
		#endif
		#if TYPE==2
			case POKE_3:
			{
				M3508orM2006EncoderTask(&general_poke.left_poke,msg);
			}break;
		#endif
    default:
        break;
    }
}

void Can2ReceiveMsgProcess(CanRxMsg *msg)
{
    switch (msg->StdId)
    {
		#if TYPE==1
			case POKE_1:
			{
				M3508orM2006EncoderTask(&general_poke.left_poke,msg);
			}break;
		#endif


			default:
					break;
    }
}

void can_bus_send_task(void)
{
//	CAN_9015torsionControl(CAN2,gimbal_data.gim_ref_and_fdb.yaw_motor_input,GIMBAL_YAW_MOTOR);
//	CAN_9015torsionControl(CAN1,gimbal_data.gim_ref_and_fdb.pitch_motor_input,GIMBAL_PITCH_MOTOR);
	
//	Set_C620andC610_IQ1(CAN2,chassis.current[0],chassis.current[1],chassis.current[2],chassis.current[3]);
#if TYPE==1
		Set_C620andC610_IQ1(CAN1,pid_friction_whell_speed[0].out,pid_friction_whell_speed[1].out,0,0);
		Set_C620andC610_IQ2(CAN1,pid_42mm_poke2_speed.out,0,0,0);
		Set_C620andC610_IQ2(CAN2,pid_42mm_poke_speed.out,0,0,0);
#endif
#if TYPE==2
	Set_C620andC610_IQ1(CAN1,pid_friction_whell_speed[0].out,pid_friction_whell_speed[1].out,pid_17mm_poke_speed.out,0);
#endif
}

