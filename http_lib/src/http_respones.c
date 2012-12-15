/*
 *      Author: Zoran Zivanovic
 *      Project: http communication
 *      Version: 0.1
 */

#include "http_respones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

function_result init_http_respones(http_respones **resp)
{
	http_respones *ret;

	*resp = NULL;

	ret = (http_respones *) malloc(sizeof(http_respones));
	if (ret)
		memset(ret, 0, sizeof(http_respones));
	else
		return ERROR_MEM;

	*resp = ret;

	return RET_OK;
}
function_result free_http_respones(http_respones **resp)
{
	if (*resp)
		free(*resp);
	*resp = NULL;
	return RET_OK;
}
function_result print_http_respons(http_respones *resp)
{
	printf("---------------------------------------\n");
	printf("Response:\n");
	printf("status %d\n",resp->http_status);
	printf("data length %d\n",resp->recv_data_len);
	printf("---------------------------------------\n");
	return RET_OK;
}
