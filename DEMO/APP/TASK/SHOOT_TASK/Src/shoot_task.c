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
float target_angle_17=0;	//���̽Ƕ�
float target_angle_2=0;	//�������̽Ƕ�

//17mm
uint32_t start_shooting_count = 0;//��ת��ʱ
uint32_t start_lock_rotor_count = 0;//��ת��ʱ
uint32_t start_reversal_count = 0;//��ת��ʱ
float shot_limit_heart0 = 0;		//��������

shoot_mode_selection_e shoot_mode_selection;//��������ģʽ
float shoot_frequency;				//������Ƶ
int will_num_shoot;			//�������÷�����
int shoot_lock_time;		//��ת��ʱ
int click_time;				//�����ʱ
int will_time_shoot;		//���ʱ��
/********************** Flag **********************/
int over_heat=0;				//��������־λ
int position_flag=0;		//��ʼ����ɱ�־λ
friction_state_t friction_state;//Ħ����״̬��־λ

int stop_friction_flag=0;//Ħ����ֹͣ��־λ
int over_hot_motor=0;		//Ħ���ֵ������

int stop_bullet_flag=0;	//����ֹͣ��־λ
shoot_state_e shoot_state={0};
int press_l_first_in=0; //0���ν���1�ǳ��ν���
int ignore_heat=0;		//���������Ʊ�־λ
int lock_flag_1times=0;	//��ת����Ƕȿ��Ʊ�־λ
int shoot_flag=0;				//42����

u8 bulletspead_level=0;		//������Ƶ���
int have_heat0_flag=0;		//������ԣ��־λ
int lock_rotor=0;
int shot_ctrl_mode;				//�������ģʽ
int BUFF_shoot_flag;			//�����־λ
int single_shoot=1;				//������־λ
/******************** FUNCTION ********************/
//���������ܳ�
void shoot_task(void)
{
	#if TYPE==1		//42mm
		Shoot_42mm_speed_Select();//42mm����ѡ��
		heat1_limit_42mm();				//��������
		shoot_friction_handle_42();	//Ħ���ֲ���
		shoot_bullet_handle_42();		//���̲���
	#elif	TYPE==2		//17mm
		Shoot_17mm_speed_Select();
		heat1_limit_17mm();				
		Heat0_switch();						//���̷���ˮƽ
		Mode_switch();						//����ģʽѡ��
		shoot_friction_handle_17();	//Ħ���ֲ���
		shoot_bullet_handle_17();		//����
	#elif	TYPE==3		//17mm x2
		Shoot_17mm_speed_Select();					//17mm����ѡ��
		heat1_limit_17mm();									//��������
		shoot_friction_handle_17();					//Ħ���ֲ���
	#endif
}

//42mm
static void Shoot_42mm_speed_Select(void)//42mm����//������ϵͳ�������޽���ѡ��
{
  if(judge_rece_mesg.game_robot_state.shooter_id1_42mm_speed_limit==10)
    frictionSpeed_42=FRICTION_SPEED_10;
  else if(judge_rece_mesg.game_robot_state.shooter_id1_42mm_speed_limit==16)
    frictionSpeed_42=FRICTION_SPEED_16;
	else
		frictionSpeed_42=FRICTION_SPEED_10;
}

void heat1_limit_42mm(void)//42mm��������
{
//-----------ʣ����������(���-�ۼ�)----------------
//-----------������ģʽ---------------------------
	float residue_heart;						//ʣ���������
  residue_heart=(judge_rece_mesg.game_robot_state.shooter_id1_42mm_cooling_limit           //��ȡ��ص�����
                       -judge_rece_mesg.power_heat_data.shooter_id1_42mm_cooling_heat);
  if(residue_heart>=100)
      over_heat=0;
  else
      over_heat=1;

	if (RC_CtrlData.Key_Flag.Key_R_Flag)
		ignore_heat=1;		//������������
}

void shoot_friction_handle_42(void)//42mmĦ����
{
	if (RC_CtrlData.inputmode != STOP)
	{
		if(RC_CtrlData.RemoteSwitch.s3to1 || RC_CtrlData.Key_Flag.Key_C_TFlag)
		{
			pid_friction_whell_speed[0].set=-frictionSpeed_42;
			pid_friction_whell_speed[1].set= frictionSpeed_42;
			//������ٶȴﵽҪ��Ħ����������־λ��1
			if ((general_friction.right_motor1.rate_rpm < -30)&&(general_friction.left_motor1.rate_rpm > 30))
			{
				friction_rotor_may_lock=0;
				friction_state = NORMAL;
			}
			else
			{
				friction_rotor_may_lock++;
				if(friction_rotor_may_lock==100)
				{
					friction_state = LOCK;
					friction_rotor_may_lock=0;
				}
			}
		}
		if (RC_CtrlData.Key_Flag.Key_Z_Flag)
		{
			pid_friction_whell_speed[0].set= frictionSpeed_42;
			pid_friction_whell_speed[1].set=-frictionSpeed_42;
				friction_state = LOCK;
		}
		if ((RC_CtrlData.RemoteSwitch.s3to1==0 && RC_CtrlData.Key_Flag.Key_C_TFlag==0)
					|| stop_friction_flag)
		{
			pid_friction_whell_speed[0].set= 0;
			pid_friction_whell_speed[1].set= 0;
				friction_state = Stop;
			pid_clr(&pid_friction_whell_speed[0]);
			pid_clr(&pid_friction_whell_speed[1]);

		}
	}
	else
	{
		pid_friction_whell_speed[0].set= 0;
		pid_friction_whell_speed[1].set= 0;
			friction_state = Stop;
		position_flag=0;	//���³�ʼ��
		pid_clr(&pid_friction_whell_speed[0]);
		pid_clr(&pid_friction_whell_speed[1]);
	}
			
	//Ħ���־�ֵ�˲�
		filter_rate_readtime++;
		filter_rate_left[0]	+=general_friction.left_motor1.rate_rpm;
		filter_rate_right[0]+=general_friction.right_motor1.rate_rpm;

		if(filter_rate_readtime>=friction_rotor_DIV)//�ֱ����ٶȵ�ƽ��ֵ
		{
			pid_friction_whell_speed[1].get=filter_rate_left[0]/friction_rotor_DIV;
			pid_friction_whell_speed[0].get=filter_rate_right[0]/friction_rotor_DIV;
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
			friction_state = Stop;
		}
}

void shoot_bullet_handle_42(void)//42mm����
{
	switch(friction_state)
		{
			case NORMAL:
			{
				//����û������
				if((over_heat==1)||(ignore_heat==1)||(stop_bullet_flag==1))
				{
					if(RC_CtrlData.mouse.press_l==1||RC_CtrlData.RemoteSwitch.trigger==1)
					{
						//������������ǵ���ģʽʱ���������ģʽΪ�������ʱ���������ģʽ
							if (press_l_first_in==0)
							{
								press_l_first_in=1;
								shoot_flag=1;
								//��������
								target_angle_2 += ONE_POKE_ANGLE_42;                               //Ŀ��Ƕȵ���ԭ�Ƕȼ�ȥÿһ�����跢��ĽǶ�
								pid_42mm_poke2_angle.set=target_angle_2;        //λ�õ�setֵΪĿ��Ƕ�
								pid_42mm_poke2_angle.iout=0;                 //��������
							}
							else
							{shoot_flag=0;}
					}
					else
					{
						press_l_first_in=0;                                           
					}
				}
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
		
		
	if(!position_flag)            //��ʼ����ʱ�����
	{
			target_angle_2 = general_poke.right_poke.ecd_angle/36.0f;
			position_flag=1;
			pid_clr(&pid_42mm_poke2_angle);
			pid_clr(&pid_42mm_poke2_speed);
			pid_42mm_poke_speed.max_out = 9000;
	}
	else if(friction_state==NORMAL)
    //���	Ħ���ֿ�����������	&& ����ﵽһ����ת�� && ��ʼ����־λΪ1	�ſ��Է���
	{
		pid_calc(&pid_42mm_poke_speed,general_poke.left_poke.rate_rpm,POKE_SPEED);
		
	//��ֹ������ȣ��²�����������
		if(shoot_flag==1)
			pid_42mm_poke_speed.max_out=POKE_MAX_OUT;
		else
		{
			if(pid_42mm_poke_speed.out<-4200 && abs(general_poke.left_poke.rate_rpm)<20)
				set_time++;
			else
				set_time = 0;
			if(set_time == 20)
				pid_42mm_poke_speed.max_out=2000;
		}

		if(general_poke.left_poke.temperature >= 80)
		{
			stop_bullet_flag = 1;
			over_hot_motor=1;
		}
		if(general_poke.left_poke.temperature <= 60)
			stop_bullet_flag = 0;
		
				pid_42mm_poke2_angle.get=(general_poke.right_poke.ecd_angle)/36.0f;
				pid_calc(&pid_42mm_poke2_angle, pid_42mm_poke2_angle.get, pid_42mm_poke2_angle.set);
				pid_42mm_poke2_speed.get=general_poke.right_poke.rate_rpm;
				pid_calc(&pid_42mm_poke2_speed, pid_42mm_poke2_speed.get, pid_42mm_poke2_angle.out);
		
	}
	else   //û�п���Ħ���֣��Ҳ�Ϊ��ʼ��ʱ���롪��Ϊ�˲��ò��̶�����
	{
			//��������
			pid_42mm_poke2_angle.set=target_angle_2;
			pid_42mm_poke2_angle.get=general_poke.right_poke.ecd_angle;
		
					pid_clr(&pid_42mm_poke_speed);
					pid_clr(&pid_42mm_poke2_speed);
	}
}

//17mm
static void Shoot_17mm_speed_Select(void)//17mm����//������ϵͳ�������޽���ѡ��
{																					
  if(judge_rece_mesg.game_robot_state.shooter_id1_17mm_speed_limit==10)
    frictionSpeed_17=FRICTION_SPEED_15;
  else if(judge_rece_mesg.game_robot_state.shooter_id1_17mm_speed_limit==16)
    frictionSpeed_17=FRICTION_SPEED_18;
	else if (judge_rece_mesg.game_robot_state.shooter_id1_17mm_speed_limit==30)
		frictionSpeed_17=FRICTION_SPEED_30;
	else
		frictionSpeed_17=FRICTION_SPEED_15;
}

void heat1_limit_17mm(void)//17mm��������
{
	float shot_limit_heart0;			//ʣ���������
	float will_num_shoot;			//ʣ�෢����
//-----------ʣ����������(���-�ۼ�)----------------
//-----------������ģʽ---------------------------
  shot_limit_heart0=(judge_rece_mesg.game_robot_state.shooter_id1_17mm_cooling_limit           //��ȡ��ص�����
                       -judge_rece_mesg.power_heat_data.shooter_id1_17mm_cooling_heat);
  will_num_shoot=shot_limit_heart0/10;
	
	if(shot_limit_heart0 > 15)
    shot_ctrl_mode=1;
  else
	{
		shot_ctrl_mode=0;
	}
}

void Mode_switch(void)//�����������ı䲻ͬ��ģʽ 
{
  if(judge_rece_mesg.game_robot_state.shooter_id1_17mm_speed_limit==30)          //����������30Ϊ�ٶ�ģʽ
	{shoot_mode_selection=speed;}
  else if(judge_rece_mesg.game_robot_state.shooter_id1_17mm_speed_limit==15&judge_rece_mesg.game_robot_state.shooter_id1_17mm_cooling_limit==50)    //��������Ϊ15��������ȴ����Ϊ50
	{ shoot_mode_selection=cooling;}
  else if(judge_rece_mesg.game_robot_state.shooter_id1_17mm_speed_limit==15&judge_rece_mesg.game_robot_state.shooter_id1_17mm_cooling_limit==150)   //��������Ϊ15��������ȴ����Ϊ150
	{ shoot_mode_selection=outburst;}
}

void Heat0_switch(void)
{
	//ʣ�෢����=����������-����ֵ��/10      ��������һ�����������Ϊ10
  will_num_shoot=(judge_rece_mesg.game_robot_state.shooter_id1_17mm_cooling_limit-judge_rece_mesg.power_heat_data.shooter_id1_17mm_cooling_heat)/10;    //ʣ�෢����
	//������Ƶ������ʵ����ûʲô�ã�ֻ������һ�ּ��㷽ʽ
		if(judge_rece_mesg.game_robot_state.robot_level==1)//level_��ȴ
		{
			shoot_frequency=10;						
		}
		else if(judge_rece_mesg.game_robot_state.robot_level==2)//level_2
		{
			shoot_frequency=14;
		}
		else if(judge_rece_mesg.game_robot_state.robot_level==3)//level_3
		{
			shoot_frequency=14;
		}
		else
		{
			shoot_frequency=10;
		}
		
		#if STANDARD == 3				
				if(bulletspead_level==1)
				{
					shoot_frequency=1.5*shoot_frequency;
				}
//				shoot_frequency=24; 
		#elif STANDARD == 4
				if(bulletspead_level==1)
				{
					shoot_frequency=15; 
				}
//				shoot_frequency=20; 
		#elif STANDARD == 5			
				if(bulletspead_level==1)
				{
					shoot_frequency=1.5*shoot_frequency ;
				}
		#endif				
}

void shoot_friction_handle_17(void)//17mmĦ����
{
	if (RC_CtrlData.inputmode != STOP)
	{
		if(RC_CtrlData.RemoteSwitch.s3to1 || RC_CtrlData.Key_Flag.Key_C_TFlag)
		{
			pid_friction_whell_speed[0].set= frictionSpeed_17;
			pid_friction_whell_speed[1].set=-frictionSpeed_17;
			//������ٶȴﵽҪ��Ħ����������־λ��1
			if ((general_friction.left_motor1.rate_rpm < -30)&&(general_friction.right_motor1.rate_rpm > 30))
			{
				friction_rotor_may_lock=0;
				friction_state = NORMAL;
			}
			else
			{
				friction_rotor_may_lock++;
				if(friction_rotor_may_lock==100)
				{
					friction_state = LOCK;
					friction_rotor_may_lock=0;
				}
			}
		}
		if (RC_CtrlData.Key_Flag.Key_Z_Flag)
		{
			pid_friction_whell_speed[0].set=-frictionSpeed_17;
			pid_friction_whell_speed[1].set= frictionSpeed_17;
				friction_state = LOCK;
			pid_clr(&pid_friction_whell_speed[0]);
			pid_clr(&pid_friction_whell_speed[1]);

		}
		if ((RC_CtrlData.RemoteSwitch.s3to1==0 && RC_CtrlData.Key_Flag.Key_C_TFlag==0))
		{
			pid_friction_whell_speed[0].set= 0;
			pid_friction_whell_speed[1].set= 0;
				friction_state = Stop;
			pid_clr(&pid_friction_whell_speed[0]);
			pid_clr(&pid_friction_whell_speed[1]);

		}
	}
	else
	{
		pid_friction_whell_speed[0].set= 0;
		pid_friction_whell_speed[1].set= 0;
			friction_state = Stop;
		position_flag=0;	//���³�ʼ��
		pid_clr(&pid_friction_whell_speed[0]);
		pid_clr(&pid_friction_whell_speed[1]);
	}
			
		//Ħ���־�ֵ�˲�
		filter_rate_readtime++;
		filter_rate_left[0]	+=general_friction.left_motor1.rate_rpm;
		filter_rate_right[0]+=general_friction.right_motor1.rate_rpm;

		if(filter_rate_readtime>=friction_rotor_DIV)//�ֱ����ٶȵ�ƽ��ֵ
		{
			pid_friction_whell_speed[1].get=filter_rate_left[0]/friction_rotor_DIV;
			pid_friction_whell_speed[0].get=filter_rate_right[0]/friction_rotor_DIV;
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
			friction_state = Stop;
		}
}

int cnt=0;
void shoot_bullet_handle_17(void)//17mm����
{
	switch(friction_state)
	{
			case NORMAL:
			{
				//����û������
				if((over_heat==0)||(ignore_heat==1)||(stop_bullet_flag==1))
				{
						if(RC_CtrlData.mouse.press_l==1||RC_CtrlData.RemoteSwitch.trigger==1)
						{
							if(single_shoot==1)//����
							{
							//������������ǵ���ģʽʱ���������ģʽΪ�������ʱ���������ģʽ
								if (press_l_first_in==0)
								{
									press_l_first_in=1;
									shoot_flag=1;
									//��������
									target_angle_17 += ONE_POKE_ANGLE_17;                               //Ŀ��Ƕȵ���ԭ�Ƕȼ�ȥÿһ�����跢��ĽǶ�
									pid_17mm_poke_angle.set=target_angle_17;        //λ�õ�setֵΪĿ��Ƕ�
									pid_17mm_poke_angle.iout=0;                 //��������
								}
								else
								{shoot_flag=0;}
							}
							else
							{
								cnt++;
								if (cnt%50==0)
								{
								target_angle_17 += ONE_POKE_ANGLE_17;                               //Ŀ��Ƕȵ���ԭ�Ƕȼ�ȥÿһ�����跢��ĽǶ�
								pid_17mm_poke_angle.set=target_angle_17;        //λ�õ�setֵΪĿ��Ƕ�
								pid_17mm_poke_angle.iout=0;                 //��������
								}
							}
						}
						else
						{
							press_l_first_in=0;                                           
						}
					}
				}
				break;
			case LOCK:            //����Ƕ�תģʽ
			{
				
			}
				break;
			case Stop:
			{
				pid_clr(&pid_17mm_poke_angle);
				pid_clr(&pid_17mm_poke_speed);
			}
			default:
				break;
		}
	if(!position_flag)            //��ʼ����ʱ�����
	{
			target_angle_17 = general_poke.left_poke.ecd_angle/36.0f;
			position_flag=1;
			pid_clr(&pid_17mm_poke_angle);
			pid_clr(&pid_17mm_poke_speed);
	}
	else if(friction_state==NORMAL)
    //���	Ħ���ֿ�����������	&& ����ﵽһ����ת�� && ��ʼ����־λΪ1	�ſ��Է���
	{
		if(general_poke.left_poke.temperature >= 80)
		{
			stop_bullet_flag = 1;
			over_hot_motor=1;
		}
		if(general_poke.left_poke.temperature <= 60)
			stop_bullet_flag = 0;
		
				pid_17mm_poke_angle.get=(general_poke.left_poke.ecd_angle)/36.0f;
				pid_calc(&pid_17mm_poke_angle, pid_17mm_poke_angle.get, pid_17mm_poke_angle.set);
				pid_17mm_poke_speed.get=general_poke.left_poke.rate_rpm;
				pid_calc(&pid_17mm_poke_speed, pid_17mm_poke_speed.get, pid_17mm_poke_angle.out);
	}
	else   //û�п���Ħ���֣��Ҳ�Ϊ��ʼ��ʱ���롪��Ϊ�˲��ò��̶�����
	{
			pid_17mm_poke_angle.set=target_angle_17;
			pid_17mm_poke_angle.get=general_poke.left_poke.ecd_angle;
		
					pid_clr(&pid_17mm_poke_speed);
					pid_clr(&pid_17mm_poke_angle);
	}
}

void friction_lock(void)
{
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
}

//PID������ʼ��
void shot_param_init(void)
{
#if TYPE ==1
//42mmĦ����
  PID_struct_init(&pid_friction_whell_speed[0], POSITION_PID,10000,1000, 0, 70 , 0.01f ,100);//����Ħ����
  PID_struct_init(&pid_friction_whell_speed[1], POSITION_PID,10000,1000, 0, 72 , 0.01f ,10);
#endif
#if TYPE ==2
//17mmĦ����
  PID_struct_init(&pid_friction_whell_speed[0], POSITION_PID,10000,1000, 0, 8 , 0 ,0);//����Ħ����
  PID_struct_init(&pid_friction_whell_speed[1], POSITION_PID,10000,1000, 0, 8 , 0 ,0);
#endif
	//17mmĦ����2��
  PID_struct_init(&pid_friction_whell_speed[2], POSITION_PID,15000,1000, 0, 70 , 0.01f ,100);//����Ħ����
  PID_struct_init(&pid_friction_whell_speed[3], POSITION_PID,15000,1000, 0, 72 , 0.01f ,10);
//42mm����
  PID_struct_init(&pid_42mm_poke_speed,   POSITION_PID, 6000 , 13000, 0, 5  , 0.5, 0 );//�²����ٶȻ�
	PID_struct_init(&pid_42mm_poke2_angle, POSITION_PID, 2000 , 0    , 0, 120, 5  , 10); //��������
  PID_struct_init(&pid_42mm_poke2_speed, POSITION_PID, 9900 , 5500 , 0, 20 , 0  , 0 );
//17mm����
	PID_struct_init(&pid_17mm_poke_angle, POSITION_PID, 5000, 1000,0,5,0.2,15);
	PID_struct_init(&pid_17mm_poke_speed, POSITION_PID,15000,10000,0,50,0.1,4);
//���ģʽ
	PID_struct_init(&pid_17mm_poke_angle_buf, POSITION_PID, 6000, 1000,0,5,0.2,5);
	PID_struct_init(&pid_17mm_poke_speed_buf, POSITION_PID,19000,10000,0,300,0.3,4);

}
