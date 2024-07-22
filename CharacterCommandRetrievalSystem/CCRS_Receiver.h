/*
 * CCRS_Receiver.h
 *
 *  Created on: Jul 22, 2024
 *      Author: admin
 */

#ifndef CHARACTERCOMMANDRETRIEVALSYSTEM_CCRS_RECEIVER_H_
#define CHARACTERCOMMANDRETRIEVALSYSTEM_CCRS_RECEIVER_H_
#include <stdbool.h>
#include "usart.h"

#include "CCRS_Dictionary.h"

#define CCRS_UART_HANDLE 			huart1

#define CCRS_SPACE_BETWEEN_SIGNAL		0			//标志间加空格
#define	CCRS_CASE_INSENSITIVE			0			//不区分大小写
#define CCRS_HAS_START_SIGNAL			1			//包括开始符号

#define CCRS_SIGNAL_LEN					4			//单个符号字节数
#define CCRS_MAX_DATA_NUM				4			//最高传输数据个数

#if	CCRS_HAS_START_SIGNAL
#define CCRS_PKG_LENGTH_MAX		(6+CCRS_MAX_DATA_NUM)*SignalNum
#endif

#if	!CCRS_HAS_START_SIGNAL
#define CCRS_PKG_LENGTH_MAX		(5+CCRS_MAX_DATA_NUM)*SignalNum
#endif
/*
 	 (Package head)
 	 system
 	 operator
 	 command
 	 data number
 	 data1
 	 (data2)
 	 (data3)
 	 (data4)
 	 package end
 */
typedef enum
{

	CCRS_PKG_CORRECT,
#if	CCRS_HAS_START_SIGNAL
	CCRS_PKG_HEAD_MISMATCH,
#endif
	CCRS_PKG_DATANUM_MISMATCH,
	CCRS_PKG_SYSTEM_MISMATCH,
	CCRS_PKG_OBJECT_MISMATCH,
	CCRS_PKG_COMMAND_MISMATCH
}CCRS_PKG_Validation;

typedef struct __packed
{
#if CCRS_HAS_START_SIGNAL
	uint32_t					PKG_Head;
#endif
	uint32_t					PKG_System;
	uint32_t					PKG_Object;
	uint32_t					PKG_Command;
	uint32_t					PKG_DTN:24;
	uint32_t					PKG_DataNumber:4;
	uint32_t					PKG_Data1;
	uint32_t					PKG_Data2;
	uint32_t					PKG_Data3;
	uint32_t					PKG_Data4;
	uint32_t					PKG_END;
}CCRS_PKG_t;

typedef struct
{
	CCRS_PKG_Validation			PKG_Validation;
	CCRS_OperateDictionary_t	PKG_System;
	CCRS_OperateDictionary_t	PKG_Object;
	CCRS_OperateDictionary_t	PKG_Command;
	uint8_t						PKG_DataNumber;
	uint32_t					PKG_Data1;
	uint32_t					PKG_Data2;
	uint32_t					PKG_Data3;
	uint32_t					PKG_Data4;
}CCRS_PKG_Parsed_t;

typedef union
{
	uint8_t						streamBuffer[CCRS_PKG_LENGTH_MAX];
	CCRS_PKG_t					CCRS_PKG;
}CCRS_PKG_un;

void CCRS_INIT(void);
CCRS_PKG_Validation CCRS_CheckPkgValidation(CCRS_PKG_un pkg_un);
CCRS_PKG_Parsed_t CCRS_DataParse(CCRS_PKG_un pkg_un);
void CCRS_ExecuteCommand(CCRS_PKG_Parsed_t parsed_data);





#endif /* CHARACTERCOMMANDRETRIEVALSYSTEM_CCRS_RECEIVER_H_ */
