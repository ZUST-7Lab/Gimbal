/*
 * User_IT.c
 *
 *  Created on: Jul 22, 2024
 *      Author: admin
 */

#include "CCRS_Receiver.h"

extern CCRS_PKG_un 			pkg_un;
extern CCRS_PKG_Parsed_t 	parsed_data;
extern bool					OperateFlag;
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == &CCRS_UART_HANDLE)
	{
		parsed_data=CCRS_DataParse(pkg_un);
		OperateFlag=true;
		CCRS_NonBlock_Begin(pkg_un.streamBuffer,CCRS_PKG_LENGTH_MAX);

	}
}
