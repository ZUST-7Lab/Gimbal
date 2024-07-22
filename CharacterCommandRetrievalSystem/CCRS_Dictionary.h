/*
 * CCRS_Dictionary.h
 *
 *  Created on: Jul 22, 2024
 *      Author: admin
 */

#ifndef CHARACTERCOMMANDRETRIEVALSYSTEM_CCRS_DICTIONARY_H_
#define CHARACTERCOMMANDRETRIEVALSYSTEM_CCRS_DICTIONARY_H_

#include <String.h>
#include <stdint.h>

#define SignalNum				12			//共有多少操作符

//词典操作码，与词典对应
typedef enum
{
	STRT,
	GMBL,
	AXBT,
	AXYW,
	AXPC,
	GTAG,
	RVAG,
	GTZR,
	GTPZ,
	CLRD,
	DTN,
	END,
	NUL,//无操作
}CCRS_OperateDictionary_t;

//词典页码
typedef struct
{
	uint8_t	Retrieval_Head;
	uint8_t	Retrieval_System;
	uint8_t	Retrieval_Object;
	uint8_t	Retrieval_Command;
	uint8_t	Retrieval_NUM;
	uint8_t	Retrieval_End;
}CCRS_DictionaryIndex_t;




#endif /* CHARACTERCOMMANDRETRIEVALSYSTEM_CCRS_DICTIONARY_H_ */
