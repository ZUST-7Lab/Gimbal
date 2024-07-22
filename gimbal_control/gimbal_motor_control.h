/*
 * gimbal_motor_control.h
 *
 *	电机及驱动层，仅适用于STEP/DIR驱动方式，外加EN控制
 *  Created on: Jul 12, 2024
 *      Author: admin
 */

#include "gimbal_control_typedef.h"

#ifndef GIMBAL_CONTROL_GIMBAL_MOTOR_CONTROL_H_
#define GIMBAL_CONTROL_GIMBAL_MOTOR_CONTROL_H_

#define PRESCALER 						80-1					//分频系数
#define TIM_CLK_FREQ 					80000000				//晶振频率
#define MotorStepAngle  				1.8						//电机步进角
#define MotorSubDivision  				16						//电机步长细分值
/*tmc2208步长细分对应表
 * MS2=		MS1=	subdivision = 1/
 * 		0		1					2
 * 		1		0					4
 * 		0		0					8
 * 		1		1					16
 */



/**
 * @author  schedule
 * @brief   电机init
 * @param   MOTOR 对应的电机
 */
void GIMBAL_MOTOR_INIT(GIMBAL_MOTOR *MOTOR);

/**
 * @author  schedule
 * @brief   设置电机旋转方向
 * @param   MOTOR       对应的电机
 * @param   dir         设置的方向
 * value:   CLKWISE/COUNTERCLKWISE   
 */
void GIMBAL_MOTOR_SetDIR(GIMBAL_MOTOR *MOTOR , GIMBAL_MOTOR_DIR dir);
/**
 * @author  schedule
 * @brief   设置电机使能
 * @param   MOTOR       对应的电机 
 * @param   en          
 * value:   EN_ENABLE/EN_DISABLE
 */
void GIMBAL_MOTOR_SetEN(GIMBAL_MOTOR *MOTOR , GIMBAL_MOTOR_EN en);

/**
 * @author  schedule
 * @brief   角度角速度输出
 * @param   MOTOR       对应的电机 
 * @param   Angle       旋转角度
 * @param   AngleSpeed  角速度
 * unit:	degree
 * unit:	degree/s
 */
void GIMBAL_MOTOR_RevolveAngle(GIMBAL_MOTOR *MOTOR , float Angle , float AngleSpeed);
void GIMBAL_MOTOR_RevolveAngle_SetDir(GIMBAL_MOTOR *MOTOR , float Angle , float AngleSpeed , GIMBAL_MOTOR_DIR dir);

/**
 * @author  schedule
 * @brief   频率计数输出
 * @param   MOTOR       对应的电机 
 * @param   frequency   频率
 * @param   NUM         计数
 */
void GIMBAL_MOTOR_RevolveCount(GIMBAL_MOTOR *MOTOR , float frequency , uint16_t NUM);

/**
 * @author  schedule
 * @brief   电机停止输出
 * @param   MOTOR       对应的电机 
 */
void GIMBAL_MOTOR_StopOutput(GIMBAL_MOTOR *MOTOR);



#endif /* GIMBAL_CONTROL_GIMBAL_MOTOR_CONTROL_H_ */
