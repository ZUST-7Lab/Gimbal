/*
 * gimbal_control.h
 *
 *  Created on: Jun 17, 2024
 *      Author: admin
 */

#ifndef GIMBAL_CONTROL_H_
#define GIMBAL_CONTROL_H_


#include "gimbal_motor_control.h"
#include "mt6835.h"

/*
 *	ground 	mode 	地面模式，将云台放在地面
 *	fly 	mode	空中模式，将云台搭载在无人机下部
 */
#define GIMBAL_MODE	GroundMode
#if GIMBAL_MODE	==	GroundMode

#endif

#if GIMBAL_MODE	==	FlyMode

#endif
//设置零点
#define	Yaw_ZeroPoint_Angle			236.0   //零点时磁编读数
#define	Pitch_ZeroPoint_Angle		0.0

/**
 * @author  schedule
 * @brief   整体的init函数
 * Duration of time     about 11 s
 * 持续时间大约11s，根据负载重量可一定改变
 */
void GIMBAL_INIT();

/*GIMBAL------------------------------表示此库
 *      Axel--------------------------表示对轴的控制
 *          Both----------------------控制两个轴
 *          Yaw-----------------------控制左右的轴
 *          Pitch---------------------控制上下的轴
 *              GoToAngle-------------命令云台转到给定角度
 *              RevolveAngle----------命令云台转到对应角度
 *              GoToZero--------------归零
 *              GoToPreciseZero-------高精度归零
 *              CleanRoad-------------检查路径
*/

/**
 * @author  schedule
 * @brief   两个轴转到对应角度，在time_us区间动作并完成
 * unit     degree
 * @param   YawAngle
 * @param   PitchAngle 
 * @param   time_us
 */
void GIMBAL_Axel_Both_GotoAngle(float YawAngle,float PitchAngle,int time_us);
/**
 * @author  schedule
 * @brief   转到对应角度，在time_us区间动作并完成
 * unit     degree
 * @param   YawAngle 
 * @param   time_us 
 */
void GIMBAL_Axel_Yaw_GoToAngle(float YawAngle,int time_us);
/**
 * @author  schedule
 * @brief   转到对应角度，在time_us区间动作并完成
 * unit     degree
 * @param   PitchAngle 
 * @param   time_us 
 */
void GIMBAL_Axel_Pitch_GoToAngle(float PitchAngle,int time_us);

/**
 * @author  schedule
 * @brief   两个轴旋转一定角度，在time_us区间动作并完成
 * unit     degree
 * signal   same in next 2 function
 * @param   YawAngle 
 * @param   PitchAngle 
 * @param   time_us 
 */
void GIMBAL_Axel_Both_RevolveAngle(float YawAngle,float PitchAngle,int time_us);
/**
 * @author  schedule
 * @brief   旋转一定角度，在time_us区间动作并完成
 * unit     degree
 * signal   +   clockwise顺时针
 *          -   conunterclockwise逆时针
 * @param   YawAngle 
 * @param   time_us 
 */
void GIMBAL_Axel_Yaw_RevolveAngle(float YawAngle,int time_us);
/**
 * @author  schedule
 * @brief   旋转一定角度，在time_us区间动作并完成
 * unit     degree
 * signal   +   up
 *          -   down
 * @param   PitchAngle 
 * @param   time_us 
 */
void GIMBAL_Axel_Pitch_RevolveAngle(float PitchAngle,int time_us);

/**return;
 * @author  schedule
 * @brief   归零
 */
void GIMBAL_Axel_Pitch_GoToZero(void);
/**
 * @author  schedule
 * @brief   高精度归零
 */
void GIMBAL_Axel_Pitch_GoToPreciseZero(void);
/**
 * @author  schedule
 * @brief   检查路径
 */
void GIMBAL_Axel_Pitch_CleanRoad(void);
/**
 * @author  schedule
 * @brief   归零
 * precision    0.5 degree
 */
void GIMBAL_Axel_Yaw_GoToZero(void);
/**
 * @author  schedule
 * @brief   高精度归零
 * precision    0.1 degree
 */
void GIMBAL_Axel_Yaw_GoToPreciseZero(void);
/**
 * @author  schedule
 * @brief   检查路径
 * 
 */
void GIMBAL_Axel_Yaw_CleanRoad(void);

#endif /* GIMBAL_CONTROL_H_ */
