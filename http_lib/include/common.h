/*
 *      Author: Zoran Zivanovic
 *      Project: http communication
 *      Version: 0.1
 */

#ifndef COMMON_H_
#define COMMON_H_

typedef enum __function_result__
{
	RET_OK = 0xFF0,
	ERROR = 0xFF1,
	ERROR_CONNECTION = 0XFF2,
	ERROR_MEM = 0XFF3,
	ERROR_BAD_DATA = 0XFF4
} function_result;

typedef enum __bool__
{
	true= 0xFFFA,
	false=0xFFFB
}bool;

#endif /* COMMON_H_ */
