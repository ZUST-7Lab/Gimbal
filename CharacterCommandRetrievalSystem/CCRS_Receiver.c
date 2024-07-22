/*
 * CCRS_Receiver.c
 *
 *  Created on: Jul 22, 2024
 *      Author: admin
 */

#include "CCRS_Receiver.h"
#include "gimbal_control.h"

extern UART_HandleTypeDef huart1;
extern const char CCRS_Dictionary[SignalNum][4];
extern const CCRS_DictionaryIndex_t CCRS_DictionaryIndex;
extern CCRS_PKG_un 			pkg_un;
extern CCRS_PKG_Parsed_t 	parsed_data;
extern bool					OperateFlag;
void CCRS_NonBlock_Begin(uint8_t pData[], uint16_t maxLength)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&CCRS_UART_HANDLE, pData , maxLength);
}

CCRS_OperateDictionary_t CCRS_SerchSignal(char operator[4])
{
	for(int i=0;i<=SignalNum;i++)
	{
		for(int j=0;j<CCRS_SIGNAL_LEN;j++)
		{
			if(CCRS_Dictionary[i][j]!=operator[j])break;
			if(j==CCRS_SIGNAL_LEN-1)return i;
		}
		if(i==CCRS_DictionaryIndex.Retrieval_NUM && operator[0]=='D'&& operator[1]=='T'&& operator[2]=='N')
			return i;
	}
	return NUL;
}


void CCRS_INIT(void)
{
	CCRS_NonBlock_Begin(pkg_un.streamBuffer,CCRS_PKG_LENGTH_MAX);
}
CCRS_PKG_Validation CCRS_CheckPkgValidation(CCRS_PKG_un pkg_un)
{
#if CCRS_HAS_START_SIGNAL
	if(CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_Head)!=STRT)
		return CCRS_PKG_HEAD_MISMATCH;
#endif
	CCRS_OperateDictionary_t serch_signal ;
	serch_signal = CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_System);
	if(CCRS_DictionaryIndex.Retrieval_System > serch_signal||serch_signal >= CCRS_DictionaryIndex.Retrieval_Object)
		return CCRS_PKG_SYSTEM_MISMATCH;
	serch_signal = CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_Object);
	if(CCRS_DictionaryIndex.Retrieval_Object > serch_signal||serch_signal >= CCRS_DictionaryIndex.Retrieval_Command)
		return CCRS_PKG_OBJECT_MISMATCH;
	serch_signal = CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_Command);
	if(CCRS_DictionaryIndex.Retrieval_Command > serch_signal||serch_signal >= CCRS_DictionaryIndex.Retrieval_NUM)
		return CCRS_PKG_COMMAND_MISMATCH;
	uint32_t dtn_adress = pkg_un.CCRS_PKG.PKG_DTN;
	if(CCRS_SerchSignal((char*)&dtn_adress)==DTN)
	{
		switch (pkg_un.CCRS_PKG.PKG_DataNumber)
		{
			case 1:
				if(CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_Data2)!=END)
					return CCRS_PKG_DATANUM_MISMATCH;
				break;
			case 2:
				if(CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_Data3)!=END)
					return CCRS_PKG_DATANUM_MISMATCH;
				break;
			case 3:
				if(CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_Data4)!=END)
					return CCRS_PKG_DATANUM_MISMATCH;
				break;
			case 4:
				if(CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_END)!=END)
					return CCRS_PKG_DATANUM_MISMATCH;
				break;
			default:
				break;
		}
	}
	return CCRS_PKG_CORRECT;
}

CCRS_PKG_Parsed_t CCRS_DataParse(CCRS_PKG_un pkg_un)
{
	CCRS_PKG_Parsed_t parsed_data = {
			.PKG_System 	= 	NUL,
			.PKG_Object		= 	NUL,
			.PKG_Command	=	NUL,
			.PKG_DataNumber =	0,
			.PKG_Data1		=	0,
			.PKG_Data2		=	0,
			.PKG_Data3		=	0,
			.PKG_Data4		=	0,
	};
	if((parsed_data.PKG_Validation=CCRS_CheckPkgValidation(pkg_un))!=CCRS_PKG_CORRECT)
		return parsed_data;
	parsed_data.PKG_System		=	CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_System);
	parsed_data.PKG_Object		=	CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_Object);
	parsed_data.PKG_Command		=	CCRS_SerchSignal((char*)&pkg_un.CCRS_PKG.PKG_Command);
	parsed_data.PKG_DataNumber	=	pkg_un.CCRS_PKG.PKG_DataNumber;
	switch (parsed_data.PKG_DataNumber) {
		case 4:
			parsed_data.PKG_Data4	=	pkg_un.CCRS_PKG.PKG_Data4;
		case 3:
			parsed_data.PKG_Data3	=	pkg_un.CCRS_PKG.PKG_Data3;
		case 2:
			parsed_data.PKG_Data2	=	pkg_un.CCRS_PKG.PKG_Data2;
		case 1:
			parsed_data.PKG_Data1	=	pkg_un.CCRS_PKG.PKG_Data1;
			break;
		default:
			break;
	}
	return parsed_data;
}

void CCRS_ExecuteCommand(CCRS_PKG_Parsed_t parsed_data)
{
	if(parsed_data.PKG_Validation != CCRS_PKG_CORRECT)
		return;
	if(parsed_data.PKG_System == NUL)
		return;
	if(parsed_data.PKG_Object == NUL)
		return;
	if(parsed_data.PKG_Command == NUL)
		return;
	switch (parsed_data.PKG_System)
	{
		case GMBL:
			switch (parsed_data.PKG_Object)
			{
				case AXBT:
					switch (parsed_data.PKG_Command)
					{
						case GTAG:
							GIMBAL_Axel_Both_GotoAngle(*(float*)&parsed_data.PKG_Data1,*(float*)&parsed_data.PKG_Data2,(int)parsed_data.PKG_Data3);
							break;
						case RVAG:
							GIMBAL_Axel_Both_RevolveAngle((float)parsed_data.PKG_Data1,(float)parsed_data.PKG_Data2,(int)parsed_data.PKG_Data3);
							break;
						case GTZR:
							GIMBAL_INIT();
							break;
						case GTPZ:
							GIMBAL_INIT();
							break;
						case CLRD:
							GIMBAL_INIT();
							break;
						default:
							break;
					}
					break;
				case AXYW:
					switch (parsed_data.PKG_Command)
					{
						case GTAG:
							GIMBAL_Axel_Yaw_GoToAngle((float)parsed_data.PKG_Data1,(int)parsed_data.PKG_Data2);
							break;
						case RVAG:
							GIMBAL_Axel_Yaw_RevolveAngle((float)parsed_data.PKG_Data1,(int)parsed_data.PKG_Data2);
							break;
						case GTZR:
							GIMBAL_Axel_Yaw_GoToZero();
							break;
						case GTPZ:
							GIMBAL_Axel_Yaw_GoToPreciseZero();
							break;
						case CLRD:
							GIMBAL_Axel_Yaw_CleanRoad();
							break;
						default:
							break;
					}
					break;
				case AXPC:
					switch (parsed_data.PKG_Command)
					{
						case GTAG:
							GIMBAL_Axel_Pitch_GoToAngle((float)parsed_data.PKG_Data1,(int)parsed_data.PKG_Data2);
							break;
						case RVAG:
							GIMBAL_Axel_Pitch_RevolveAngle((float)parsed_data.PKG_Data1,(int)parsed_data.PKG_Data2);
							break;
						case GTZR:
							GIMBAL_Axel_Pitch_GoToZero();
							break;
						case GTPZ:
							GIMBAL_Axel_Pitch_GoToPreciseZero();
							break;
						case CLRD:
							GIMBAL_Axel_Pitch_CleanRoad();
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	OperateFlag=false;
}
















