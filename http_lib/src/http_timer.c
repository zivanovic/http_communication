/*
 *      Author: Zoran Zivanovic
 *      Project: http communication
 *      Version: 0.1
 */

#include "http_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

function_result init_timer(http_time **time)
{
	http_time *ret_t = malloc(sizeof(http_time));
	if (ret_t)
	{
		memset(ret_t, 0, sizeof(http_time));
	}
	else
	{
		return ERROR_MEM;
	}
	*time = ret_t;
	return RET_OK;
}
function_result free_timer(http_time **time)
{
	if (*time)
	{
		free(*time);
	}
	*time = NULL;
	return RET_OK;
}

function_result start_timer(http_time *time)
{
	if (time)
		gettimeofday(&time->time_start, NULL);
	else
		return ERROR;

	return RET_OK;
}
function_result stop_timer(http_time *time)
{
	if (time)
		gettimeofday(&time->time_end, NULL);
	else
		return ERROR;
	return RET_OK;
}

struct timeval time_elapsed(http_time *time)
{
	struct timeval ret;
	ret.tv_sec = time->time_end.tv_sec - time->time_start.tv_sec;
	ret.tv_usec = time->time_end.tv_usec - time->time_start.tv_usec;

	return ret;
}

double time_elapsed_second(http_time *time)
{
	struct timeval ret;
	ret.tv_sec = time->time_end.tv_sec - time->time_start.tv_sec;
	ret.tv_usec = time->time_end.tv_usec - time->time_start.tv_usec;

	double retur = ret.tv_sec + ret.tv_usec / 1000000.0;
	return retur;
}
double time_elapsed_msecond(http_time *time)
{
	struct timeval ret;
	ret.tv_sec = time->time_end.tv_sec - time->time_start.tv_sec;
	ret.tv_usec = time->time_end.tv_usec - time->time_start.tv_usec;

	double retur = ret.tv_sec * 1000.0 + ret.tv_usec / 1000.0;
	return retur;
}
double time_elapsed_usecond(http_time *time)
{
	struct timeval ret;
	ret.tv_sec = time->time_end.tv_sec - time->time_start.tv_sec;
	ret.tv_usec = time->time_end.tv_usec - time->time_start.tv_usec;

	double retur = ret.tv_sec * 1000000.0 + ret.tv_usec;
	return retur;
}
