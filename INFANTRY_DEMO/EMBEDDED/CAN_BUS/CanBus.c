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
 
 
static uint32_t can1_count = 0;
static uint32_t can2_count = 0;

void Can1ReceiveMsgProcess(CanRxMsg * msg)
{
    can1_count++;
    switch (msg->StdId)
    {
    case GIMBAL_YAW_MOTOR:
		{
			
		}break;
    default:
        break;
    }
}

void Can2ReceiveMsgProcess(CanRxMsg * msg)
{
    can2_count++;
    switch (msg->StdId)
    {
    case GIMBAL_YAW_MOTOR:
			GM6020EncoderTask(can2_count,&yaw_Encoder,msg,GMYawEncoder_Offset);
        /* code */
        break;
		case GIMBAL_PITCH_MOTOR:
			GM6020EncoderTask(can2_count,&Pitch_Encoder,msg,GMPitchEncoder_Offset);

    default:
        break;
    }
}








void can_bus_send_task(void)
{
	can_chassis_task(CAN2,chassis.follow_gimbal,
							chassis.chassis_speed_mode ,
							chassis.ctrl_mode,yaw_Encoder.ecd_angle,
							yaw_Encoder.filter_rate,
							chassis.ChassisSpeed_Ref.left_right_ref,
							chassis.ChassisSpeed_Ref.forward_back_ref,
							550,
							judge_rece_mesg.power_heat_data.chassis_power,
							judge_rece_mesg.power_heat_data.chassis_power_buffer,
							judge_rece_mesg.game_robot_state.chassis_power_limit);
	Set_GM6020_IQ1(CAN2,gimbal_data.gim_ref_and_fdb.yaw_motor_input,gimbal_data.gim_ref_and_fdb.pitch_motor_input,0,0);
	
}

