/*
 * mt6835_typedef.h
 *
 *  Created on: Jul 19, 2024
 *      Author: admin
 */

#ifndef MT6835_MT6835_TYPEDEF_H_
#define MT6835_MT6835_TYPEDEF_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct __packed{
	uint32_t 				crc:8;
	uint32_t  				status_Overspeed:1;
	uint32_t  				status_magField_TooWeak:1;
	uint32_t  				status_pwrSupply_Low:1;
	uint32_t  				angleReg:21;
}MT6835_AngleReg_t;

typedef union{
	uint8_t 				buffer[4];
	MT6835_AngleReg_t 		pkg;
}un_MT6835_pkg_mt;

typedef struct{
	float  				angle;
	bool					OverSpeedAlarm;
	bool					MagFieldWeakAlarm;
	bool					LowPowerAlarm;
}MT6835_Angle_t;

#endif /* MT6835_MT6835_TYPEDEF_H_ */
