/*
 * CCRS_Dictionary.c
 *
 *  Created on: Jul 22, 2024
 *      Author: admin
 */

#include "CCRS_Dictionary.h"




//词典
const char 	CCRS_Dictionary[SignalNum][4]=
{
	{	"STRT"	},//0
	//system
	{	"GMBL"	},//1
	//object
	{	"AXBT"	},//2		AXel_Both
	{	"AXYW"	},//3		Axel_Yaw
	{	"AXPC"	},//4		Axel_Pitch
	//command
	{	"GTAG"	},//5		GoRoAngle
	{	"RVAG"	},//6		RevolveAngle
	{	"GTZR"	},//7		GotoZero
	{	"GTPZ"	},//8		GoToPreciseZero
	{	"CLRD"	},//9		CleanRoad

	{	"DTN "	},//10
	{	"END\n"	},//11

};
//词典检索设置
const CCRS_DictionaryIndex_t CCRS_DictionaryIndex = {
	.Retrieval_Head		=	0,
	.Retrieval_System	=	1,
	.Retrieval_Object	=	2,
	.Retrieval_Command	=	5,
	.Retrieval_NUM		=	10,
	.Retrieval_End		=	11,
};

