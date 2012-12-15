/*
 *      Author: Zoran Zivanovic
 *      Project: http communication
 *      Version: 0.1
 */
#ifndef __http_h__
#define __http_h__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "http_respones.h"
#include "common.h"

/*=========== define constant ===========*/
#define USER_AGENT "HTTP_GET 1.0"
#define PORT 80

typedef struct __http_context__
{
	uint8_t *host;
	uint8_t *path;
	uint8_t *name;

	int socket;
	char *ip_adr;
	struct sockaddr_in *remote;
	bool is_finished;
	http_respones *respones;

	double connection_time;
	double request_time;
	double response_time;

	double total_downlod_time;
	uint32_t total_readed_size;

	double downlod_speed;
} http_context;

function_result init_http_context(http_context **cont, uint8_t *file);
function_result delete_http_context(http_context **cont);
function_result connect_http_context(http_context *cont);
function_result read_packet_http_context(http_context *cont, uint8_t* ret_buffer, uint32_t ret_buffer_size, uint32_t *readed_size);
function_result read_all_http_context(http_context *cont, uint8_t** ret_buffer, uint32_t *readed_size);

#endif //__http_h__
