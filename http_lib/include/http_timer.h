/*
 *      Author: Zoran Zivanovic
 *      Project: http communication
 *      Version: 0.1
 */

#ifndef HTTP_TIMER_H_
#define HTTP_TIMER_H_

#include "common.h"
#include <stdint.h>
#include <sys/time.h>

typedef struct
{
	struct timeval time_start;
	struct timeval time_end;

}http_time;


function_result init_timer(http_time **time);
function_result free_timer(http_time **time);

function_result start_timer(http_time *time);
function_result stop_timer(http_time *time);

struct timeval time_elapsed(http_time *time);

double time_elapsed_second(http_time *time);
double time_elapsed_msecond(http_time *time);
double time_elapsed_usecond(http_time *time);

#endif /* HTTP_TIMER_H_ */
