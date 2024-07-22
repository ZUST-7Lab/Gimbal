/*
 * MT6835.c
 *
 *  Created on: Jul 15, 2024
 *      Author: admin
 */


#include "mt6835.h"
extern MT6835_HardWare_Set MT6835;

void Enable_MT6835(void)
{
	HAL_GPIO_WritePin(MT6835.GPIOx,MT6835.GPIO_Pin,GPIO_PIN_RESET);
}

void Disable_MT6835(void)
{
	HAL_GPIO_WritePin(MT6835.GPIOx,MT6835.GPIO_Pin,GPIO_PIN_SET);
}

MT6835_Angle_t MT6835_Data_Read(void)
{
	uint16_t 				TxData;
	un_MT6835_pkg_mt		un_pkg_mt;
	MT6835_Angle_t 			angle_parsed;

	Enable_MT6835();
	TxData = ReadRegister;
	HAL_SPI_Transmit(MT6835.hspi,(unsigned char *)&TxData,2, 100);
	HAL_SPI_Receive(MT6835.hspi,(unsigned char *)&un_pkg_mt.buffer,ReadLength, 100);
	Disable_MT6835();

	//解析数据
	angle_parsed.angle 				= (un_pkg_mt.pkg.angleReg/pow(2,21))*360;
	angle_parsed.LowPowerAlarm 		= un_pkg_mt.pkg.status_pwrSupply_Low;
	angle_parsed.MagFieldWeakAlarm 	= un_pkg_mt.pkg.status_magField_TooWeak;
	angle_parsed.OverSpeedAlarm 	= un_pkg_mt.pkg.status_Overspeed;

	return angle_parsed;
}

float MT6835_Angle_Read(void)
{
	MT6835_Angle_t 	angle_parsed = MT6835_Data_Read();
	float angle = angle_parsed.angle;
	return angle;
}

/**
 * 调试用，通过串口输出
 */
void MT6835_Angle_Print(MT6835_Angle_t angle_parsed)
{
	//vofa+ type
	printf("%f,%d,%d,%d\n",
			angle_parsed.angle,
			angle_parsed.LowPowerAlarm,
			angle_parsed.MagFieldWeakAlarm,
			angle_parsed.OverSpeedAlarm);

//	//usart print type with alarm
//	printf("angle = %f,"
//			"low power  = %d,"
//			"mag field  = %d,"
//			"over speed = %d\n",
//			angle_parsed.angle,
//			angle_parsed.LowPowerAlarm,
//			angle_parsed.MagFieldWeakAlarm,
//			angle_parsed.OverSpeedAlarm);
//
//	//usart print type without  alarm
//	printf("angle = %f\n",angle_parsed.angle);

}

