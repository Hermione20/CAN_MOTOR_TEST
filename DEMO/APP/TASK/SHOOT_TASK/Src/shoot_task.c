#include "shoot_task.h"

/******************** VARIABVLE ******************/
uint16_t frictionSpeed_42=0;		//42mm����
uint16_t frictionSpeed_17=0;		//17mm����
float residue_heart;						//ʣ���������

int filter_rate_left[2] ={0};//Ħ����ת�پ�ֵ�˲�
int filter_rate_right[2]={0};
int filter_rate_readtime=0;
int friction_rotor_DIV=11;

int friction_rotor_may_lock=0;//Ħ���ֿ��ܶ�ת����
int set_time=0;			//�²����������Ƽ���
float target_angle=0;	//���̽Ƕ�
float target_angle_2=0;	//�������̽Ƕ�

/********************** Flag **********************/
int over_heat=0;				//��������־λ
int position_flag=0;		//��ʼ����ɱ�־λ
friction_state_t friction_state;//Ħ����״̬��־λ
int friction_normal_flag=0;//Ħ����������־λ

int stop_friction_flag=0;//Ħ����ֹͣ��־λ
int over_hot_motor=0;		//Ħ���ֵ������

int stop_bullet_flag=0;	//����ֹͣ��־λ
shoot_state_e shoot_state={0};
int press_l_first_in=1; //1���ν���0�ǳ��ν���
int ignore_heat=0;		//���������Ʊ�־λ
int lock_flag_1times=0;	//��ת����Ƕȿ��Ʊ�־λ

/******************** FUNCTION ********************/
//���������ܳ�
void shoot_task(void)
{
	Shoot_42mm_speed_Select();//42mm����ѡ��
	heat1_limit();						//��������
	shoot_friction_handle();	//Ħ���ֲ���
	shoot_bullet_handle();		//���̲���
}

static void Shoot_42mm_speed_Select(void)//42mm����
{																					//������ϵͳ�������޽���ѡ��
  if(judge_rece_mesg.game_robot_state.shooter_id1_42mm_speed_limit==10)
    frictionSpeed_42=FRICTION_SPEED_10;
  else if(judge_rece_mesg.game_robot_state.shooter_id1_42mm_speed_limit==16)
    frictionSpeed_42=FRICTION_SPEED_16;
	else
		frictionSpeed_42=FRICTION_SPEED_10;
}

//��������
void heat1_limit(void)
{
//-----------ʣ����������(���-�ۼ�)----------------
//-----------������ģʽ---------------------------
  residue_heart=(judge_rece_mesg.game_robot_state.shooter_id1_42mm_cooling_limit           //��ȡ��ص�����
                       -judge_rece_mesg.power_heat_data.shooter_id1_42mm_cooling_heat);

  if(residue_heart>=100)
      over_heat=0;
  else
      over_heat=1;

	if (RC_CtrlData.Key_Flag.Key_R_Flag)
		ignore_heat=1;		//������������

}

//Ħ���ֲ���
void shoot_friction_handle(void)
{
	if (RC_CtrlData.inputmode != Stop && position_flag==0)
	{
		if(RC_CtrlData.RemoteSwitch.s3to1 || RC_CtrlData.Key_Flag.Key_C_TFlag)
		{
			pid_friction_whell_speed[0].set=-frictionSpeed_42;
			pid_friction_whell_speed[1].set= frictionSpeed_42;
		}
		if (RC_CtrlData.Key_Flag.Key_Z_Flag)
		{
			pid_friction_whell_speed[0].set= frictionSpeed_42;
			pid_friction_whell_speed[1].set=-frictionSpeed_42;
				friction_state = LOCK;
		}
		if ((friction_state != Stop && (RC_CtrlData.RemoteSwitch.s3to1==0 || RC_CtrlData.Key_Flag.Key_C_TFlag==0))
					|| stop_friction_flag)
		{
			pid_friction_whell_speed[0].set= 0;
			pid_friction_whell_speed[1].set= 0;
				friction_state = Stop;
		}
	}
	else
	{
		pid_friction_whell_speed[0].set= 0;
		pid_friction_whell_speed[1].set= 0;
			friction_state = Stop;
		position_flag=1;	//���³�ʼ��
	}
	
	//������ٶȴﵽҪ��Ħ����������־λ��1
		if(RC_CtrlData.Key_Flag.Key_C_TFlag && (general_friction.left_motor1.rate_rpm < -50)&&(general_friction.right_motor1.rate_rpm > 50))
		{
			friction_normal_flag = 1;
			friction_rotor_may_lock=0;
				friction_state = NORMAL;
		}
		else if(RC_CtrlData.Key_Flag.Key_C_TFlag &&(general_friction.left_motor1.rate_rpm>-30)&&(general_friction.left_motor1.rate_rpm<30)
																			&&(general_friction.right_motor1.rate_rpm<30)&&(general_friction.right_motor1.rate_rpm>-30))
		{
			friction_rotor_may_lock++;
			if(friction_rotor_may_lock==100)
			{
				friction_state = LOCK;
				friction_rotor_may_lock=0;
				friction_normal_flag = 0;
			}
		}
		else if(friction_state==Stop) //���ģʽΪֹͣ
		{
			friction_normal_flag = 0;
			friction_rotor_may_lock = 0;
		}
		
	//Ħ���־�ֵ�˲�
		filter_rate_readtime++;
		filter_rate_left[0]	+=general_friction.left_motor1.rate_rpm;
		filter_rate_right[0]+=general_friction.right_motor1.rate_rpm;

		if(filter_rate_readtime>=friction_rotor_DIV)//�ֱ����ٶȵ�ƽ��ֵ
		{
			pid_friction_whell_speed[0].get=filter_rate_left[0]/friction_rotor_DIV;
			pid_friction_whell_speed[1].get=filter_rate_right[0]/friction_rotor_DIV;
			filter_rate_readtime=0;
			filter_rate_left[0]=0;
			filter_rate_right[0]=0;
		}
				
	//PID����
		pid_calc(&pid_friction_whell_speed[0],pid_friction_whell_speed[0].get, pid_friction_whell_speed[0].set);
		pid_calc(&pid_friction_whell_speed[1],pid_friction_whell_speed[1].get, pid_friction_whell_speed[1].set);
	//��ֹ�������
		if(general_friction.left_motor1.temperature >= 90||general_friction.right_motor1.temperature >= 90)
		{
			stop_friction_flag = 1;
		}
		if(general_friction.left_motor1.temperature <= 70 && general_friction.right_motor1.temperature <= 70)
			stop_friction_flag = 0;
}

//����
void shoot_bullet_handle(void)
{
	if(!position_flag)            //��ʼ����ʱ�����
	{
			target_angle_2 = general_poke.right_poke.ecd_angle;
			position_flag=1;
			pid_clr(&pid_42mm_poke2_angle);
			pid_clr(&pid_42mm_poke2_speed);
	}
	else if((general_friction.left_motor1.rate_rpm<-1000&&general_friction.right_motor1.rate_rpm>1000) && position_flag && (friction_state==NORMAL))
    //���	Ħ���ֿ�����������	&& ����ﵽһ����ת�� && ��ʼ����־λΪ1	�ſ��Է���
	{
		static int l_key_delay_time=0;
		l_key_delay_time++;
		pid_calc(&pid_42mm_poke_speed,general_poke.left_poke.rate_rpm,POKE_SPEED);
		
	//��ֹ������ȣ��²�����������
		if(press_l_first_in==0)
			pid_42mm_poke_speed.max_out=POKE_MAX_OUT;
		else
		{
			if(pid_42mm_poke_speed.out<-4200 && abs(general_poke.left_poke.rate_rpm)<20)
				set_time++;
			else
				set_time = 0;
			if(set_time == 2)
				pid_42mm_poke_speed.max_out=2000;
		}

		if(general_poke.left_poke.temperature >= 80)
		{
			stop_bullet_flag = 1;
			over_hot_motor=1;
		}
		if(general_poke.left_poke.temperature <= 60)
			stop_bullet_flag = 0;
				
		switch(friction_state)
		{
			case NORMAL:
			{
				//����û������
				if((over_heat==0)||(ignore_heat==1))
				{
					if((RC_CtrlData.mouse.press_l==1)&&(RC_CtrlData.mouse.last_press_l==0))
					{
						//������������ǵ���ģʽʱ���������ģʽΪ�������ʱ���������ģʽ
						if((press_l_first_in==1)&&(l_key_delay_time>150))
						{
							//��������
							target_angle_2 += ONE_POKE_ANGLE_2;                               //Ŀ��Ƕȵ���ԭ�Ƕȼ�ȥÿһ�����跢��ĽǶ�
							pid_42mm_poke2_angle.set=target_angle_2;        //λ�õ�setֵΪĿ��Ƕ�
							pid_42mm_poke2_angle.iout=0;                 //��������

							press_l_first_in=0;                                                                         
							l_key_delay_time=0;                                          //�����ʱ
						}
					}
					else
					{
						press_l_first_in=1;                                           
					}
				}
				pid_42mm_poke2_angle.get=general_poke.right_poke.ecd_angle;
				pid_calc(&pid_42mm_poke2_angle, pid_42mm_poke2_angle.get, pid_42mm_poke2_speed.set);
				pid_42mm_poke2_speed.get=general_poke.right_poke.rate_rpm+pid_42mm_poke2_speed.out*k_speed;
				pid_calc(&pid_42mm_poke2_speed, pid_42mm_poke2_speed.get, pid_42mm_poke2_speed.out);
			}
				break;
			case LOCK:            //����Ƕ�תģʽ
			{					
			}
				break;
			case Stop:
			{
				pid_clr(&pid_42mm_poke_speed);
				pid_clr(&pid_42mm_poke2_angle);
				pid_clr(&pid_42mm_poke2_speed);
			}
			default:
				break;
		}
	}
	else   //û�п���Ħ���֣��Ҳ�Ϊ��ʼ��ʱ���롪��Ϊ�˲��ò��̶�����
	{
			//��������
			pid_42mm_poke2_angle.set=target_angle_2;
			pid_42mm_poke2_angle.get=general_poke.right_poke.ecd_angle;
			pid_calc(&pid_42mm_poke2_angle,pid_42mm_poke2_angle.get, pid_42mm_poke2_angle.set);
			pid_42mm_poke2_speed.get=general_poke.right_poke.rate_rpm;
			pid_calc(&pid_42mm_poke2_speed,pid_42mm_poke2_speed.get,pid_42mm_poke2_speed.out);
		
					pid_clr(&pid_42mm_poke_speed);
					pid_clr(&pid_42mm_poke2_speed);
	}
}


//void friction_lock(void)
//{
//	if (!lock_flag)
//	{
//		taget_angle_2-=30;
//		lock_flag=1;
//	}
//	
//	pid_shoot_bullet_position_angle_loop_2.get=BUS1_CM5_Poke_Encoder.rotor_out;
//	pid_calc(&pid_shoot_bullet_position_angle_loop_2, pid_shoot_bullet_position_angle_loop_2.get, pid_shoot_bullet_position_angle_loop_2.set); //2006
//	pid_shoot_bullet_position_speed_loop_2.get=BUS1_CM5_Poke_Encoder.rotor_speed;//+pid_shoot_bullet_position_speed_loop_2.out*k_speed;
//	pid_calc(&pid_shoot_bullet_position_speed_loop_2, pid_shoot_bullet_position_speed_loop_2.get, pid_shoot_bullet_position_angle_loop_2.out);

//	pid_calc(&pid_shoot_bullet_position_speed_loop,BUS2_CM5_Poke_Encoder.rate_rpm,LOCK_SPEED);

//	if(GetInputMode() == STOP)
//	{
//		Set_GM_CM_Current(CAN2,0,0,0,0);
//		Set_GM_CM_Current(CAN1,0,0,0,0);
//	}
//	else
//	{
//		Set_GM_CM_Current(CAN2,pid_shoot_bullet_position_speed_loop.out,0,0,0);
//		Set_GM_CM_Current(CAN1,pid_shoot_bullet_position_speed_loop_2.out,0,0,aim_scope_speed.out);
//	}
//}

//PID������ʼ��
void shot_param_init(void)
{
//		Set_GM_CM_Current(CAN1,0,0,0,0);

  PID_struct_init(&pid_friction_whell_speed[0], POSITION_PID,15000,1000, 0, 70 , 0.01f ,100);//����Ħ����
  PID_struct_init(&pid_friction_whell_speed[1], POSITION_PID,15000,1000, 0, 72 , 0.01f ,10);

  PID_struct_init(&pid_42mm_poke_speed,   POSITION_PID, 6000 , 13000, 0, 5  , 0.5, 0 );//�²����ٶȻ�
	PID_struct_init(&pid_42mm_poke2_angle, POSITION_PID, 2000 , 0    , 0, 120, 5  , 10); //��������
  PID_struct_init(&pid_42mm_poke2_speed, POSITION_PID, 9900 , 5500 , 0, 20 , 0  , 0 );
}
