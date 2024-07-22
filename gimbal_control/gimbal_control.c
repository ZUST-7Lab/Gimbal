/*
 * gimbal_control.c
 *
 *	云台控制层，仅控制两个电机
 *  Created on: Jun 17, 2024
 *      Author: admin
 */

#include "gimbal_control.h"

extern GIMBAL_MOTOR 			PitchMotor;
extern GIMBAL_MOTOR 			YawMotor;
extern GIMBAL_GPIO 				Pitch_Zero_Point;
extern GIMBAL_GPIO 				High_OUTPUT;
extern MT6835_HardWare_Set 		MT6835;

float Yaw_Angle;
float Pitch_Angle;


float GIMBAL_fmin(float num1,float num2)
{
	if (num1<=num2) {
		return num1;
	}
	if (num1>num2) {
		return num2;
	}
}

float GIMBAL_fmin_3(float num1,float num2,float num3)
{
	if(num1<=num2&&num1<=num3)
		return num1;
	if (num1>num2&&num2<=num3)
		return num2;
	if(num1>num3&&num2>num3)
		return num3;
}




/*
 * 角度限幅
 */
float GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(float in)
{
	float out = in;
	while(1){
	if(out>=360.0)out=out-360.0;
	if(out<0.0)out=out+360.0;
	if(0.0<=out&&out<360.0)return out;
	}
}
/*
 * 获得旋转角度
 */
float GIMBAL_Axel_Yaw_GetRevolveAngle(float angle1 , float angle2)
{
	float min_angle;
	if(0.0<=angle1&&angle1<=360.0&&0.0<=angle2&&angle2<=360.0)//限幅
		min_angle = GIMBAL_fmin_3(fabs(angle1-angle2),fabs(angle1-angle2+360),fabs(angle2-angle1+360));
	else
	{
		float angl1=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(angle1);
		float angl2=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(angle2);
		min_angle = GIMBAL_fmin_3(fabs(angl1-angl2),fabs(angl1-angl2+360),fabs(angl2-angl1+360));
	}
	return min_angle;
}


/*
 * 顺时针增大的环形，angel1->angel2
 * 获得旋转方向
 */
GIMBAL_MOTOR_DIR GIMBAL_Axel_Yaw_GetRevolveDIR(float angle1 , float angle2)
{	if(0.0<=angle1&&angle1<=360.0&&0.0<=angle2&&angle2<=360.0)//限幅
	{
		if(fabs(angle1-angle2)<=GIMBAL_fmin(fabs(angle1-angle2+360),fabs(angle1-angle2+360)))
		{
			if(angle1>=angle2)return CLKWISE;
			if(angle1<angle2)return COUNTERCLKWISE;
		}
		if(fabs(angle1-angle2)>GIMBAL_fmin(fabs(angle1-angle2+360),fabs(angle1-angle2+360)))
		{
			if(angle1>=angle2)return COUNTERCLKWISE;
			if(angle1<angle2)return CLKWISE;
		}
	}
	else
	{
		float angl1=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(angle1);
		float angl2=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(angle2);
		if(fabs(angl1-angl2)<=GIMBAL_fmin(fabs(angl1-angl2+360),fabs(angl1-angl2+360)))
		{
			if(angl1>=angl2)return COUNTERCLKWISE;
			if(angl1<angl2)return CLKWISE;
		}
		if(fabs(angl1-angl2)>GIMBAL_fmin(fabs(angl1-angl2+360),fabs(angl1-angl2+360)))
		{
			if(angl1>=angl2)return CLKWISE;
			if(angl1<angl2)return COUNTERCLKWISE;
		}
	}
}


/**
 * user function
 */


void GIMBAL_INIT()
{
	GIMBAL_MOTOR_INIT(&PitchMotor);
	GIMBAL_MOTOR_INIT(&YawMotor);
	HAL_GPIO_WritePin(High_OUTPUT.GPIOx, High_OUTPUT.GPIO_Pin, GPIO_PIN_SET);
	GIMBAL_Axel_Yaw_GoToZero();
	GIMBAL_Axel_Yaw_CleanRoad();
	GIMBAL_Axel_Yaw_GoToPreciseZero();
	GIMBAL_Delay(250);
	GIMBAL_Axel_Pitch_GoToZero();
	GIMBAL_Axel_Pitch_CleanRoad();
	GIMBAL_Axel_Pitch_GoToPreciseZero();


}

/*
 * 到达目标角度
 */
void GIMBAL_Axel_Both_GotoAngle(float YawAngle,float PitchAngle,int time_us)
{
	float tgtYawAngle = GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(Yaw_ZeroPoint_Angle-YawAngle);
	float tgtPitchAngle = GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(Pitch_ZeroPoint_Angle+PitchAngle);
	float dYawAngel = GIMBAL_Axel_Yaw_GetRevolveAngle(Yaw_Angle,tgtYawAngle);//转的度数
	float dPitchAngel = GIMBAL_Axel_Yaw_GetRevolveAngle(Pitch_Angle,tgtPitchAngle);//转的度数
	//确认方向
	GIMBAL_MOTOR_DIR YawDir = GIMBAL_Axel_Yaw_GetRevolveDIR(Yaw_Angle,tgtYawAngle);
	GIMBAL_MOTOR_DIR PitchDir = GIMBAL_Axel_Yaw_GetRevolveDIR(Pitch_Angle,tgtPitchAngle);
	//设置方向
	GIMBAL_MOTOR_SetDIR(&YawMotor,YawDir);
	GIMBAL_MOTOR_SetDIR(&PitchMotor,PitchDir);
	GIMBAL_MOTOR_RevolveAngle(&PitchMotor, fabs(dPitchAngel), fabs(dPitchAngel/(time_us/1000)));
	while(GIMBAL_Axel_Yaw_GetRevolveAngle(Yaw_Angle,tgtYawAngle)>0.1)
	{
		GIMBAL_MOTOR_RevolveAngle(&YawMotor,fabs(dYawAngel),fabs(dYawAngel/(time_us/1000)));
		GIMBAL_Delay(1);
		Yaw_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(MT6835_Angle_Read());
	}
	GIMBAL_MOTOR_StopOutput(&YawMotor);
	GIMBAL_MOTOR_StopOutput(&PitchMotor);
	Yaw_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(MT6835_Angle_Read());
	Pitch_Angle=tgtPitchAngle;
}
void GIMBAL_Axel_Yaw_GoToAngle(float YawAngle,int time_us)
{
	float tgtYawAngle = GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(Yaw_ZeroPoint_Angle-YawAngle);
	float dYawAngel = GIMBAL_Axel_Yaw_GetRevolveAngle(Yaw_Angle,tgtYawAngle);//转的度数
	//确认方向
	GIMBAL_MOTOR_DIR YawDir = GIMBAL_Axel_Yaw_GetRevolveDIR(Yaw_Angle,tgtYawAngle);
	//设置方向
	GIMBAL_MOTOR_SetDIR(&YawMotor,YawDir);
	while(GIMBAL_Axel_Yaw_GetRevolveAngle(Yaw_Angle,tgtYawAngle)>0.1)
	{
		GIMBAL_MOTOR_RevolveAngle(&YawMotor,fabs(dYawAngel),fabs(dYawAngel/(time_us/1000)));
		GIMBAL_Delay(1);
		Yaw_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(MT6835_Angle_Read());
	}
	GIMBAL_MOTOR_StopOutput(&YawMotor);
	Yaw_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(MT6835_Angle_Read());
}


void GIMBAL_Axel_Pitch_GoToAngle(float PitchAngle,int time_us)
{
	float tgtPitchAngle = GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(Pitch_ZeroPoint_Angle+PitchAngle);
	float dPitchAngel = GIMBAL_Axel_Yaw_GetRevolveAngle(Pitch_Angle,tgtPitchAngle);//转的度数
	//确认方向
	GIMBAL_MOTOR_DIR PitchDir = GIMBAL_Axel_Yaw_GetRevolveDIR(Pitch_Angle,tgtPitchAngle);
	GIMBAL_MOTOR_SetDIR(&PitchMotor,PitchDir);
	GIMBAL_MOTOR_RevolveAngle(&PitchMotor, fabs(dPitchAngel), fabs(dPitchAngel/(time_us/1000)));
	GIMBAL_Delay(time_us);
	Pitch_Angle=tgtPitchAngle;
}


/*
 * Angle可正可负，正是顺时针负是逆时针
 */
void GIMBAL_Axel_Both_RevolveAngle(float YawAngle,float PitchAngle,int time_us)
{
	float tgtYawAngle = GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(Yaw_Angle-YawAngle);

	float dYawAngel = fabs(PitchAngle/(time_us/1000));//每秒转的度数
	float dPitchAngel = fabs(PitchAngle/(time_us/1000));//每秒转的度数
	//确认方向
	GIMBAL_MOTOR_DIR YawDir ;
	GIMBAL_MOTOR_DIR PitchDir ;
	if(YawAngle>=0)		YawDir = CLKWISE;
	if(YawAngle<0)		YawDir = COUNTERCLKWISE;
	if(PitchAngle>=0)	PitchDir = COUNTERCLKWISE;
	if(PitchAngle<0)	PitchDir = CLKWISE;
	//设置方向
	GIMBAL_MOTOR_SetDIR(&YawMotor,YawDir);
	GIMBAL_MOTOR_SetDIR(&PitchMotor,PitchDir);
	GIMBAL_MOTOR_RevolveAngle(&PitchMotor, fabs(PitchAngle), dPitchAngel);
	while(GIMBAL_Axel_Yaw_GetRevolveAngle(Yaw_Angle,tgtYawAngle)>0.1)
	{
		GIMBAL_MOTOR_RevolveAngle(&YawMotor,fabs(YawAngle),dYawAngel);
		GIMBAL_Delay(1);
		Yaw_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(MT6835_Angle_Read());
	}
	GIMBAL_MOTOR_StopOutput(&YawMotor);
	GIMBAL_MOTOR_StopOutput(&PitchMotor);
	Yaw_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(MT6835_Angle_Read());
	Pitch_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(Pitch_Angle+PitchAngle);
}
void GIMBAL_Axel_Yaw_RevolveAngle(float YawAngle,int time_us)
{
	float tgtYawAngle = GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(Yaw_Angle-YawAngle);
	float dYawAngel = fabs(YawAngle/(time_us/1000));//每秒转的度数
	//确认方向
	GIMBAL_MOTOR_DIR YawDir ;
	if(YawAngle>=0)		YawDir = CLKWISE;
	if(YawAngle<0)		YawDir = COUNTERCLKWISE;
	//设置方向
	GIMBAL_MOTOR_SetDIR(&YawMotor,YawDir);
	while(GIMBAL_Axel_Yaw_GetRevolveAngle(Yaw_Angle,tgtYawAngle)>0.1)
	{
		GIMBAL_MOTOR_RevolveAngle(&YawMotor,fabs(YawAngle),dYawAngel);
		GIMBAL_Delay(1);
		Yaw_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(MT6835_Angle_Read());
	}
	GIMBAL_MOTOR_StopOutput(&YawMotor);
	Yaw_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(MT6835_Angle_Read());
}
void GIMBAL_Axel_Pitch_RevolveAngle(float PitchAngle,int time_us)
{
	float dPitchAngel = fabs(PitchAngle/(time_us/1000));//每秒转的度数
	//确认方向
	GIMBAL_MOTOR_DIR PitchDir ;
	if(PitchAngle>=0)	PitchDir = COUNTERCLKWISE;
	if(PitchAngle<0)	PitchDir = CLKWISE;
	GIMBAL_MOTOR_SetDIR(&PitchMotor,PitchDir);
	GIMBAL_MOTOR_RevolveAngle(&PitchMotor, fabs(PitchAngle), dPitchAngel);
	GIMBAL_Delay(time_us);
	Pitch_Angle=GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(Pitch_Angle+PitchAngle);
}

/**
 * 单个轴归零
 */
void GIMBAL_Axel_Pitch_GoToZero(void)
{
	while(HAL_GPIO_ReadPin(Pitch_Zero_Point.GPIOx, Pitch_Zero_Point.GPIO_Pin) != GPIO_PIN_SET)
	{
		GIMBAL_MOTOR_RevolveAngle_SetDir(&PitchMotor , 1 , 50,CLKWISE);
	}
	Pitch_Angle = Pitch_ZeroPoint_Angle;
}

void GIMBAL_Axel_Pitch_GoToPreciseZero(void)
{
	GIMBAL_Axel_Pitch_GoToZero();

	GIMBAL_MOTOR_RevolveAngle_SetDir(&PitchMotor , 10 , 20 ,COUNTERCLKWISE);

	while(HAL_GPIO_ReadPin(Pitch_Zero_Point.GPIOx, Pitch_Zero_Point.GPIO_Pin) != GPIO_PIN_SET)
	{
		GIMBAL_MOTOR_RevolveAngle_SetDir(&PitchMotor , 1 , 10,CLKWISE);
	}
	Pitch_Angle = Pitch_ZeroPoint_Angle;
}

void GIMBAL_Axel_Pitch_CleanRoad(void)
{
	GIMBAL_Axel_Pitch_GoToZero();
	GIMBAL_MOTOR_RevolveAngle_SetDir(&PitchMotor , 10 , 50 ,COUNTERCLKWISE);
	GIMBAL_MOTOR_RevolveAngle_SetDir(&PitchMotor , 170 , 70 ,COUNTERCLKWISE);
	GIMBAL_MOTOR_RevolveAngle_SetDir(&PitchMotor , 170 , 70 ,CLKWISE);
	GIMBAL_Axel_Pitch_GoToZero();
}

void GIMBAL_Axel_Yaw_GoToZero(void)
{

	while(fabs(MT6835_Angle_Read()-Yaw_ZeroPoint_Angle)>0.5)
	{
		GIMBAL_MOTOR_RevolveAngle_SetDir(
				&YawMotor,
				GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(GIMBAL_Axel_Yaw_GetRevolveAngle(MT6835_Angle_Read(),Yaw_ZeroPoint_Angle)),
				25,
				GIMBAL_Axel_Yaw_GetRevolveDIR(MT6835_Angle_Read(),Yaw_ZeroPoint_Angle)
				);
	}
	Yaw_Angle = Yaw_ZeroPoint_Angle;

}
void GIMBAL_Axel_Yaw_GoToPreciseZero(void)
{
	GIMBAL_Axel_Yaw_GoToZero();
	GIMBAL_MOTOR_RevolveAngle_SetDir(&YawMotor , 10 , 20  ,CLKWISE);
	while(fabs(MT6835_Angle_Read()-Yaw_ZeroPoint_Angle)>0.1)
	{
		GIMBAL_MOTOR_RevolveAngle_SetDir(
				&YawMotor,
				GIMBAL_Axel_Yaw_LIMIT_RangeInCircle_0_360(GIMBAL_Axel_Yaw_GetRevolveAngle(MT6835_Angle_Read(),Yaw_ZeroPoint_Angle)),
				10,
				GIMBAL_Axel_Yaw_GetRevolveDIR(MT6835_Angle_Read(),Yaw_ZeroPoint_Angle)
				);
	}
	Yaw_Angle = Yaw_ZeroPoint_Angle;
}

void GIMBAL_Axel_Yaw_CleanRoad(void)
{
	GIMBAL_Axel_Yaw_GoToZero();
	GIMBAL_MOTOR_RevolveAngle_SetDir(&YawMotor , 90  , 150 ,CLKWISE);
	GIMBAL_MOTOR_RevolveAngle_SetDir(&YawMotor , 180 , 150 ,COUNTERCLKWISE);
	GIMBAL_MOTOR_RevolveAngle_SetDir(&YawMotor , 80  , 70  ,CLKWISE);
	GIMBAL_Axel_Yaw_GoToZero();
}









