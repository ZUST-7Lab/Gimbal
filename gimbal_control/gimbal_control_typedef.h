/*
 * gimbal_control_typedef.h
 *
 *  Created on: Jul 12, 2024
 *      Author: admin
 */
#include <stdint.h>
#include "gpio.h"
#include "tim.h"

#ifndef GIMBAL_CONTROL_GIMBAL_CONTROL_TYPEDEF_H_
#define GIMBAL_CONTROL_GIMBAL_CONTROL_TYPEDEF_H_


typedef enum
{
	FALSE,
	TRUE,
}Boolean;

/*
 * 云台电机使能
 */
typedef enum
{
	EN_ENABLE,
	EN_DISABLE,
}GIMBAL_MOTOR_EN;
/*
 * 云台电机反转标志
 */
typedef enum
{
	REVERSE_TRUE,
	REVERSE_FALSE,
}GIMBAL_MOTOR_REVERSE;
/*
 * 云台电机旋转方向
 */
typedef enum
{
	CLKWISE,
	COUNTERCLKWISE,
}GIMBAL_MOTOR_DIR;



/*
 * PWM产生PFM
 */
typedef struct
{
	void* 				PWM_TIMER;
//	TIM_HandleTypeDef 	*PWM_TIMER;
	uint32_t 			PWM_CHANNEL;
}GIMBAL_PFM;
/*
 * IO口
 */
typedef struct
{
//	GPIO_TypeDef 		*GPIOx;
	void*		 		*GPIOx;
	uint16_t 			GPIO_Pin;
}GIMBAL_GPIO;
/*
 * 电机
 */
typedef struct
{
	GIMBAL_PFM					PFM_PIN;		//电机PFM脚，对应驱动的STEP
	GIMBAL_GPIO					EN_PIN;			//电机使能脚，对应驱动的EN
	GIMBAL_GPIO					DIR_PIN;		//电机方向脚，对应驱动的DIR
	GIMBAL_MOTOR_EN				EN;				//电机使能判断
	GIMBAL_MOTOR_REVERSE		REVERSE;		//电机反转判断
	GIMBAL_MOTOR_DIR			DIR;			//电机旋转方向
	uint16_t					GIMBAL_PFM_Counts;
}GIMBAL_MOTOR;

#endif /* GIMBAL_CONTROL_GIMBAL_CONTROL_TYPEDEF_H_ */
