#include "shoot_task.h"

/**************** VARIABVLE ****************/
uint16_t frictionSpeed_42=0;		//42mm����
uint16_t frictionSpeed_17=0;		//17mm����
float residue_heart;						//ʣ���������



/**************** Flag ****************/
int over_heat=0;				//��������־λ
int position_flag=0;		//��ʼ����־λ





//���������ܳ�
void shoot_task(void)
{
	Shoot_42mm_speed_Select();//42mm����ѡ��
	heat1_limit();						//��������
	shoot_bullet_handle();		//���̲���
	shoot_friction_handle();	//Ħ���ֲ���
}


static void Shoot_42mm_speed_Select(void)//42mm����
{
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
}

//Ħ���ֲ���
void shoot_friction_handle(void)
{
	switch(RC_CtrlData.inputmode)
	{
		case REMOTE_INPUT:
		{
			if (1)
			{
				
			}
		}
			break;
		case KEY_MOUSE_INPUT:
		{
			if (1)
			{
				
			}
		}
			break;
		case STOP:
		{
			if (1)
			{
				
			}
		}
			break;
	}
		switch()
		{
			case	friction_stop:   //ֹͣ
			{
				friction_rotor =0;
        pid_rotate[2].set=0;
        pid_rotate[1].set=0;
			}
			break;
			case friction_on:     //��ת
			{
			  pid_rotate[1].set=-(frictionSpeed)*frictionRamp.Calc(&frictionRamp);
				pid_rotate[2].set= (frictionSpeed)*frictionRamp.Calc(&frictionRamp);
			}
			break;
			case friction_slow:   //���٣���ת
			{
				pid_rotate[1].set=-(frictionSpeed-(frictionSpeed)*frictionRamp.Calc(&frictionRamp));
				pid_rotate[2].set= (frictionSpeed-(frictionSpeed)*frictionRamp.Calc(&frictionRamp));
			}
			break;
			case friction_back:   //��ת
			{
				pid_rotate[1].set=(frictionSpeed)*frictionRamp.Calc(&frictionRamp);
				pid_rotate[2].set=-(frictionSpeed)*frictionRamp.Calc(&frictionRamp);
			}
			break;
			default:               //ֹͣ
			{
				friction_rotor =0;
				pid_rotate[2].set=0;
				pid_rotate[1].set=0;
			}
		}

		BUS1_CMEncoder_readtime2++;
		BUS1_CMEncoder_Rate1+=BUS1_CM1Encoder.rate_rpm;
		BUS1_CMEncoder_Rate2+=BUS1_CM2Encoder.rate_rpm;

		if(BUS1_CMEncoder_readtime2>=friction_rotor_DIV)//�ֱ����ٶȵ�ƽ��ֵ
		{
			pid_rotate[1].get=BUS1_CMEncoder_Rate1/friction_rotor_DIV;//BUS1_CM1Encoder.rate_rpm;//BUS1_CMEncoder_Rate1/friction_rotor_DIV;
			pid_rotate[2].get=BUS1_CMEncoder_Rate2/friction_rotor_DIV;//BUS1_CM2Encoder.rate_rpm;//BUS1_CMEncoder_Rate2/friction_rotor_DIV;
			BUS1_CMEncoder_readtime2=0;
			BUS1_CMEncoder_Rate1=0;
			BUS1_CMEncoder_Rate2=0;
		}
		
		//������ٶȴﵽҪ��Ħ����������־λ��1
		if((friction_rotor==friction_on)&&(BUS1_CM1Encoder.filter_rate<-30)&&(BUS1_CM2Encoder.filter_rate>30))
		{
			friction_normal_flag = 1;
			friction_rotor_may_lock=0;
		}
		else if((friction_rotor==friction_on)&&(BUS1_CM1Encoder.filter_rate>-5)&&(BUS1_CM1Encoder.filter_rate<5)&&(BUS1_CM2Encoder.filter_rate<5)&&(BUS1_CM2Encoder.filter_rate>-5))
		{
			friction_rotor_may_lock++;
			if(friction_rotor_may_lock==50)
			{
				friction_rotor=friction_back;
				friction_back_flag=1;
				friction_rotor_may_lock=0;
			}
		}
		else if(friction_rotor==friction_stop)         //���ģʽΪֹͣ
		{
			friction_normal_flag = 0;
		}
		
		//��ֹ��ת
		if(friction_back_flag==1)
		{
			friction_back_count++;
			if(friction_back_count==400) 
			{
				friction_back_flag=0;
				friction_rotor=friction_on;
				friction_back_count=0;
			}			
		}

		//PID����
		pid_calc(&pid_rotate[1],pid_rotate[1].get, pid_rotate[1].set);
		pid_calc(&pid_rotate[2],pid_rotate[2].get, pid_rotate[2].set);
		float current1,current2;
		current1 = pid_rotate[1].out;
		current2 = pid_rotate[2].out;
//		current1 = LADRC_control_task(&ladrc_num,pid_rotate[1].set,pid_rotate[1].get);
//    current2 = LADRC_control_task(&ladrc_angle,pid_rotate[2].set,pid_rotate[2].get);
		//��ֹ�������
		if(BUS1_CM1Encoder.temperature >= 90||BUS1_CM2Encoder.temperature >= 90)
		{
			stop_friction_flag = 1;
			over_hot_motor=2;
		}
		if(BUS1_CM1Encoder.temperature <= 70&&BUS1_CM2Encoder.temperature <= 70)
			stop_friction_flag = 0;
		
		if(GetInputMode() == STOP)
			Set_CM_Speed(CAN1,0,0,0,0);
	 	else
		{
			if(stop_friction_flag==1||turn_friction_flag==0)
				Set_CM_Speed(CAN1,0,0,0,0);
			else
        Set_CM_Speed(CAN1,pid_rotate[1].out-friction_rotor_error,pid_rotate[2].out+friction_rotor_error,0,0);//Set_CM_Speed(CAN1,pid_rotate[1].out-pid_compenstae,pid_rotate[2].out+pid_compenstae,0,0);
		}
}

//����
void shoot_bullet_handle(void)
{
	if(Init_flag)            //��ʼ����ʱ�����
	{
			taget_angle_2=BUS1_CM5_Poke_Encoder.rotor_out;
			position_flag=0;
			position_time=0;
			position_set_flag=1;
			pid_clr(&pid_shoot_bullet_position_angle_loop_2);
			pid_clr(&pid_shoot_bullet_position_speed_loop_2);
	}
	else if((!sac_flag)&&(BUS1_CM1Encoder.rate_rpm<-1000&&BUS1_CM2Encoder.rate_rpm>1000)&&(!position_flag)&&(friction_rotor==friction_on))
    //���	�ϵ���	����	����ﵽһ����ת�٣�����Ħ���֣�	����	��ʼ����־λΪ0	�ſ��Է���
	{
		static int l_key_delay_time=0;
		l_key_delay_time++;
		pid_calc(&pid_shoot_bullet_position_speed_loop,BUS2_CM5_Poke_Encoder.rate_rpm,POKE_SPEED);
		
	//��ֹ�������
		if(press_l_first_in==0)
			pid_shoot_bullet_position_speed_loop.max_out=poke_max_out;
		else
		{
			if(pid_shoot_bullet_position_speed_loop.out<-4200 && abs(BUS2_CM5_Poke_Encoder.rate_rpm)<20)
				set_time++;
			else	
				set_time = 0;
			if(set_time == 2)
				pid_shoot_bullet_position_speed_loop.max_out=2000;
		}

		if(BUS2_CM5_Poke_Encoder.temperature >= 80)
		{
			stop_bullet_flag = 1;
			over_hot_motor=1;
		}
		if(BUS2_CM5_Poke_Encoder.temperature <= 60)
			stop_bullet_flag = 0;

		switch(shot_state)
		{
			case NOMAL:
			{
				//�¶�û������
				if((over_heat==0)||(ignore_heat==1))////////&&BUS1_CM1Encoder.rate_rpm<-500&&BUS1_CM2Encoder.rate_rpm>500
				{
					//������������ǵ���ģʽʱ���������ģʽΪ�������ʱ���������ģʽ
					if(((shootState==SHOOTING)||((shot_flag)&&(autoshoot_mode==AUTO_STATION_SHOOT)))&&(GetInputMode()!= STOP)&&(friction_rotor==1))//������ʱ
					{
						if((press_l_first_in==1)&&(l_key_delay_time>150))
						{
							//��������
							taget_angle_2 += ONE_POKE_ANGLE_2;                               //Ŀ��Ƕȵ���ԭ�Ƕȼ�ȥÿһ�����跢��ĽǶ�
							pid_shoot_bullet_position_angle_loop_2.set=taget_angle_2;        //λ�õ�setֵΪĿ��Ƕ�
							pid_shoot_bullet_position_angle_loop_2.iout=0;                 //��������

							press_l_first_in=0;                                                                         
							l_key_delay_time=0;                                          //�����ʱ
							shot_flag=0;                                                 
							shot_judge_flag=0;
						}
					}
					else
					{
						press_l_first_in=1;                                           
					}
				}
			pid_shoot_bullet_position_angle_loop_2.get=BUS1_CM5_Poke_Encoder.rotor_out;
			pid_calc(&pid_shoot_bullet_position_angle_loop_2, pid_shoot_bullet_position_angle_loop_2.get, pid_shoot_bullet_position_angle_loop_2.set); //2006
			pid_shoot_bullet_position_speed_loop_2.get=BUS1_CM5_Poke_Encoder.rotor_speed+pid_shoot_bullet_position_speed_loop_2.out*k_speed;
			pid_calc(&pid_shoot_bullet_position_speed_loop_2, pid_shoot_bullet_position_speed_loop_2.get, pid_shoot_bullet_position_angle_loop_2.out);
			}
			break;
			case LOCK:            //����Ƕ�תģʽ
			{
			}
			break;
		}
		//�ؿ�
		if(GetInputMode() == STOP)
			{
				Set_GM_CM_Current(CAN2,0,0,0,0);
				Set_GM_CM_Current(CAN1,0,0,0,0);
			}
		else //���ؿ�
			{
				//���û�п�����ת���̡������������Ͳ����������ʲô�����𣿣���������������������
				if(shot_state!=LOCK)
				{
					if(stop_bullet_flag||bullet_flag ||friction_rotor !=friction_on)//���ȡ�û���ۼӡ�������������������������������������������������������������������������������������
					{
						Set_GM_CM_Current(CAN2,0,0,0,0);
						Set_GM_CM_Current(CAN1,0,0,0,aim_scope_speed.out);
					}
					else //����
					{
						Set_GM_CM_Current(CAN2,pid_shoot_bullet_position_speed_loop.out,0,0,0);//pid_shoot_bullet_position_speed_loop.out
						Set_GM_CM_Current(CAN1,pid_shoot_bullet_position_speed_loop_2.out,0,0,aim_scope_speed.out);
					}
				}
			}
	}
	else   //û�п���Ħ���֣��Ҳ�Ϊ��ʼ��ʱ���롪��Ϊ�˲��ò��̶�����
	{
//		if(position_set_flag)
//		{
//			if(GetInputMode() == STOP)
//			{
//				pid_clr(&pid_shoot_bullet_position_speed_loop);
//				pid_clr(&pid_shoot_bullet_position_speed_loop_2);
//				pid_shoot_bullet_position_speed_loop_2.iout=0;//�������̻�������
//				pid_shoot_bullet_position_speed_loop.out=0;//��������
//			}
//			//��������
			pid_shoot_bullet_position_angle_loop_2.set=taget_angle_2;
			pid_shoot_bullet_position_angle_loop_2.get=BUS1_CM5_Poke_Encoder.rotor_out;
			pid_calc(&pid_shoot_bullet_position_angle_loop_2,pid_shoot_bullet_position_angle_loop_2.get, pid_shoot_bullet_position_angle_loop_2.set);
			pid_shoot_bullet_position_speed_loop_2.get=BUS1_CM5_Poke_Encoder.rotor_speed;
			pid_calc(&pid_shoot_bullet_position_speed_loop_2,pid_shoot_bullet_position_speed_loop_2.get,pid_shoot_bullet_position_angle_loop_2.out);

		  
//			else
//				{
					
//				}
      if(GetInputMode() == STOP)
				{
					Set_GM_CM_Current(CAN2,0,0,0,0);
					Set_GM_CM_Current(CAN1,0,0,0,aim_scope_speed.out);
					pid_clr(&pid_shoot_bullet_position_speed_loop);
					pid_clr(&pid_shoot_bullet_position_speed_loop_2);
				}else 
        {
          Set_GM_CM_Current(CAN1,pid_shoot_bullet_position_speed_loop_2.out,0,0,aim_scope_speed.out);
					Set_GM_CM_Current(CAN2,0,0,0,0);
        }
//    Set_GM_CM_Current(CAN2,0,0,0,0);
//		Set_GM_CM_Current(CAN1,0,0,0,aim_scope_speed.out);
	}
}


void friction_lock(void)
{
	if (!lock_flag)
	{
		taget_angle_2-=30;
		lock_flag=1;
	}
	
	pid_shoot_bullet_position_angle_loop_2.get=BUS1_CM5_Poke_Encoder.rotor_out;
	pid_calc(&pid_shoot_bullet_position_angle_loop_2, pid_shoot_bullet_position_angle_loop_2.get, pid_shoot_bullet_position_angle_loop_2.set); //2006
	pid_shoot_bullet_position_speed_loop_2.get=BUS1_CM5_Poke_Encoder.rotor_speed;//+pid_shoot_bullet_position_speed_loop_2.out*k_speed;
	pid_calc(&pid_shoot_bullet_position_speed_loop_2, pid_shoot_bullet_position_speed_loop_2.get, pid_shoot_bullet_position_angle_loop_2.out);

	pid_calc(&pid_shoot_bullet_position_speed_loop,BUS2_CM5_Poke_Encoder.rate_rpm,LOCK_SPEED);

	if(GetInputMode() == STOP)
	{
		Set_GM_CM_Current(CAN2,0,0,0,0);
		Set_GM_CM_Current(CAN1,0,0,0,0);
	}
	else
	{
		Set_GM_CM_Current(CAN2,pid_shoot_bullet_position_speed_loop.out,0,0,0);
		Set_GM_CM_Current(CAN1,pid_shoot_bullet_position_speed_loop_2.out,0,0,aim_scope_speed.out);
	}
}

//PID������ʼ��
void shot_param_init(void)
{
		Set_GM_CM_Current(CAN1,0,0,0,0);

  PID_struct_init(&pid_friction_whell_speed[0], POSITION_PID,15000,1000, 0, 70 , 0.01f ,100);//����Ħ����
  PID_struct_init(&pid_friction_whell_speed[1], POSITION_PID,15000,1000, 0, 72 , 0.01f ,10);

  PID_struct_init(&pid_42mm_poke_speed,   POSITION_PID, 6000 , 13000, 0, 5  , 0.5, 0 );//�²����ٶȻ�
	PID_struct_init(&pid_42mm_poke2_angle, POSITION_PID, 2000 , 0    , 0, 120, 5  , 10); //��������
  PID_struct_init(&pid_42mm_poke2_speed, POSITION_PID, 9900 , 5500 , 0, 20 , 0  , 0 );
}
