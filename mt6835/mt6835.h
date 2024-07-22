/*
 * MT6835.h
 *
 *  Created on: Jul 15, 2024
 *      Author: admin
 */

#ifndef MT6835_MT6835_H_
#define MT6835_MT6835_H_

#include <math.h>

#include "spi.h"
#include "gpio.h"
#include "usart.h"

#include "mt6835_typedef.h"

#define ReadRegister 	0xA003; //A=continous read  003 = ANGLE 要读取的地址
#define ReadLength		4		//读取的字节数

/**
 * SPI映射
 */
typedef struct{
	SPI_HandleTypeDef 	*hspi;		//spi Pin
	GPIO_TypeDef 		*GPIOx; 	//cs  Pin
	uint16_t 			GPIO_Pin;	//cs  Pin
}MT6835_HardWare_Set;

/**
 * @author  schedule
 * @brief 	读取角度
 * @return MT6835_Angle_t
 * 	double  				angle;
	bool					OverSpeedAlarm;
	bool					MagFieldWeakAlarm;
	bool					LowPowerAlarm;
 */
MT6835_Angle_t MT6835_Data_Read(void);
float MT6835_Angle_Read(void);
#endif /* MT6835_MT6835_H_ */
