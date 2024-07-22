/*
 * User_IT.c
 *
 *  Created on: Jul 13, 2024
 *      Author: admin
 */

#include "gimbal_control.h"

extern GIMBAL_MOTOR PitchMotor;
extern GIMBAL_MOTOR YawMotor;
/*
 * PWM输出中断，用于输出脉冲计数
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) //中断回调tim2可用
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //中断回调
{
	if(htim == PitchMotor.PFM_PIN.PWM_TIMER)
	{
		static uint16_t i1 = 0;
		i1++;
		if(i1 >= PitchMotor.GIMBAL_PFM_Counts)  //此处10为产生PWM的数量，可设置为变量实时改变
		{
			i1 =0;
			GIMBAL_MOTOR_StopOutput(&PitchMotor); //产生指定数量的PWM波后停止PWM的产生
			PitchMotor.GIMBAL_PFM_Counts = 0;
		}
	}
	if(htim == YawMotor.PFM_PIN.PWM_TIMER)
	{
		static uint16_t i2 = 0;
		i2++;
		if(i2 >= YawMotor.GIMBAL_PFM_Counts)  //此处10为产生PWM的数量，可设置为变量实时改变
		{
			i2 =0;
			GIMBAL_MOTOR_StopOutput(&YawMotor); //产生指定数量的PWM波后停止PWM的产生
			YawMotor.GIMBAL_PFM_Counts = 0;
		}

	}
}
