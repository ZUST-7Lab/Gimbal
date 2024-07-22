/*
 * gimbal_motor_control.c
 *
 *	电机及驱动层，仅适用于STEP/DIR驱动方式，外加EN控制
 *  Created on: Jul 12, 2024
 *      Author: admin
 */
#include "gimbal_motor_control.h"

extern GIMBAL_MOTOR PitchMotor;
extern GIMBAL_MOTOR YawMotor;

/*
 * 判断两个是否为目标电机
 */
Boolean GIMBAL_MOTOR_CMP(GIMBAL_MOTOR *MOTOR1 , GIMBAL_MOTOR *MOTOR2)
{
	if (MOTOR1->PFM_PIN.PWM_CHANNEL 	== 	MOTOR2->PFM_PIN.PWM_CHANNEL
	&&  MOTOR1->PFM_PIN.PWM_TIMER 		== 	MOTOR2->PFM_PIN.PWM_TIMER
	&&	MOTOR1->EN_PIN.GPIO_Pin			==	MOTOR2->EN_PIN.GPIO_Pin
	&&	MOTOR1->EN_PIN.GPIOx			==	MOTOR2->EN_PIN.GPIOx
	&&	MOTOR1->DIR_PIN.GPIO_Pin		==	MOTOR2->DIR_PIN.GPIO_Pin
	&&	MOTOR1->DIR_PIN.GPIOx			==	MOTOR2->DIR_PIN.GPIOx
	) return TRUE;
	return FALSE;
}

/**
 * 使用PWM配置输出PFM频率,占空比50
 * output PFM with PWM mode，duty=50
 * frequency:输出频率
 */
static void GIMBAL_MOTOR_PFM_SetFrequency(GIMBAL_MOTOR *MOTOR , float frequency)
{
	const unsigned int duty =50;
	//计算PWM频率，所对应的自动重装载值   ---> ARR = 主频 / (预分频+1) / 预期PWM频率(Hz) - 1
    float pwm_freq_arr  = (TIM_CLK_FREQ * 1.0f) / (PRESCALER+1) / frequency * 1.0f - 1;
    //占空比:duty = Pulse / (ARR+1)
    float pwm_duty_pulse = ( duty / 100.0f)* (pwm_freq_arr + 1);

    //配置PSC预分频值
    __HAL_TIM_SET_PRESCALER((TIM_HandleTypeDef*)MOTOR->PFM_PIN.PWM_TIMER, PRESCALER);
    //配置PWM频率 ARR
    __HAL_TIM_SetAutoreload((TIM_HandleTypeDef*)MOTOR->PFM_PIN.PWM_TIMER, (uint16_t)pwm_freq_arr);
    //配置PWM占空比
    __HAL_TIM_SetCompare((TIM_HandleTypeDef*)MOTOR->PFM_PIN.PWM_TIMER, MOTOR->PFM_PIN.PWM_CHANNEL, (uint16_t)pwm_duty_pulse);
//    HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_1);
}

/*
 * 设置PFM输出脉冲个数
 */
static void GIMBAL_MOTOR_PFM_SetPFMNUM(GIMBAL_MOTOR *MOTOR , uint16_t NUM)
{
	MOTOR->GIMBAL_PFM_Counts	=	NUM;
}

/*
 * PFM输出,频率计数式
 */
void GIMBAL_MOTOR_PFM_OUTPUT(GIMBAL_MOTOR *MOTOR , float frequency , uint16_t NUM)
{
	GIMBAL_MOTOR_PFM_SetFrequency(MOTOR , frequency);
	GIMBAL_MOTOR_PFM_SetPFMNUM(MOTOR , NUM);
	//MOTOR.GIMBAL_PFM_Counts	=	NUM;
    //输出PWM
    HAL_TIM_PWM_Start_IT((TIM_HandleTypeDef*)MOTOR->PFM_PIN.PWM_TIMER,MOTOR->PFM_PIN.PWM_CHANNEL);
    //HAL_TIM
}

/*
 * PFM输出停止
 */
void GIMBAL_MOTOR_PFM_StopOutput(GIMBAL_MOTOR *MOTOR)
{
    HAL_TIM_PWM_Stop_IT((TIM_HandleTypeDef*)MOTOR->PFM_PIN.PWM_TIMER,MOTOR->PFM_PIN.PWM_CHANNEL);
}

/*
 * 角度转步数
 * 步数=角度/1步对应的角度
 * 1步对应的角度=步长/步长细分
 */
uint16_t GIMBAL_MOTOR_AngleToNum(float Angle)
{
	return (uint16_t)Angle/(MotorStepAngle/MotorSubDivision);
}
/*
 * 角速度转频率
 * 公式同上，单位不一样而已
 * 角速度单位:度每秒
 */
float GIMBAL_MOTOR_AngleSpeedToFrequency(float AngleSpeed)
{
	return AngleSpeed/(MotorStepAngle/MotorSubDivision);
}




/*
 * user function
 */
void GIMBAL_Delay(int ms)
{
	HAL_Delay(ms);
}

void GIMBAL_MOTOR_INIT(GIMBAL_MOTOR *MOTOR)
{
	GIMBAL_MOTOR_SetDIR(MOTOR , CLKWISE);
	GIMBAL_MOTOR_SetEN(MOTOR , MOTOR->EN);
}

/*
 * 改变方向
 */
void GIMBAL_MOTOR_SetDIR(GIMBAL_MOTOR *MOTOR , GIMBAL_MOTOR_DIR dir)
{
	MOTOR->DIR	=	dir;
	if ((MOTOR->REVERSE	==	REVERSE_TRUE	&&	dir ==	CLKWISE)
		||	(MOTOR->REVERSE	==	REVERSE_FALSE	&&	dir	==	COUNTERCLKWISE)) {
		HAL_GPIO_WritePin((GPIO_TypeDef*)MOTOR->DIR_PIN.GPIOx , MOTOR->DIR_PIN.GPIO_Pin , GPIO_PIN_SET);
	}
	if ((MOTOR->REVERSE	==	REVERSE_TRUE	&&	dir	==	COUNTERCLKWISE)
			||	(MOTOR->REVERSE	==	REVERSE_FALSE	&&	dir	==	CLKWISE)) {
		HAL_GPIO_WritePin((GPIO_TypeDef*)MOTOR->DIR_PIN.GPIOx , MOTOR->DIR_PIN.GPIO_Pin , GPIO_PIN_RESET);
	}

}

/*
 * 改变使能
 */
void GIMBAL_MOTOR_SetEN(GIMBAL_MOTOR *MOTOR , GIMBAL_MOTOR_EN en)
{
	MOTOR->EN = en;
	if (en	==	EN_ENABLE) {
		HAL_GPIO_WritePin((GPIO_TypeDef*)MOTOR->EN_PIN.GPIOx, MOTOR->EN_PIN.GPIO_Pin , GPIO_PIN_RESET);
	}
	if (en	==	EN_DISABLE) {
		HAL_GPIO_WritePin((GPIO_TypeDef*)MOTOR->EN_PIN.GPIOx , MOTOR->EN_PIN.GPIO_Pin , GPIO_PIN_SET);
	}

}

/*
 * 云台旋转输出设置，角度角速度式
 * 此函数执行后需要用户手动延时
 */
void GIMBAL_MOTOR_RevolveAngle(GIMBAL_MOTOR *MOTOR , float Angle , float AngleSpeed)
{

	uint16_t NUM = GIMBAL_MOTOR_AngleToNum(Angle);
	float frequency = GIMBAL_MOTOR_AngleSpeedToFrequency(AngleSpeed);
	GIMBAL_MOTOR_PFM_OUTPUT(MOTOR , frequency , NUM);
}
/*
 * 云台旋转输出，角度角速度式
 * 在电机转完前不会退出此函数，仅适用于单电机旋转
 */
void GIMBAL_MOTOR_RevolveAngle_SetDir(GIMBAL_MOTOR *MOTOR , float Angle , float AngleSpeed , GIMBAL_MOTOR_DIR dir)
{
	GIMBAL_MOTOR_SetDIR(MOTOR,dir);
	GIMBAL_MOTOR_RevolveAngle(MOTOR,Angle,AngleSpeed);
	while(MOTOR->GIMBAL_PFM_Counts!=0)
	{
		GIMBAL_Delay(1);
	}
}

/*
 * PFM输出,频率计数式
 */
void GIMBAL_MOTOR_RevolveCount(GIMBAL_MOTOR *MOTOR , float frequency , uint16_t NUM)
{
	GIMBAL_MOTOR_PFM_OUTPUT(MOTOR , frequency , NUM);
}

/*
 * 停止输出
 */
void GIMBAL_MOTOR_StopOutput(GIMBAL_MOTOR *MOTOR)
{
	GIMBAL_MOTOR_PFM_StopOutput(MOTOR);
}











