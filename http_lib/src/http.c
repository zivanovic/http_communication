/*
 *      Author: Zoran Zivanovic
 *      Project: http communication
 *      Version: 0.1
 */

#include "http.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "http_timer.h"

/*=========== define constant ===========*/
#define HTTP_SIZE 7
#define HTTPs_SIZE 8
#define BUFFER_SIZE 1024

/*===========static function list declaration===========*/
static function_result get_host_from(uint8_t *file, uint8_t **host);
static function_result get_path_from(uint8_t *file, uint8_t **path);
static function_result get_file_name_from(uint8_t *file, uint8_t **file_name);
static char* make_request(http_context *cont);
static int create_tcp_socket();
static char *get_ip(char *host);
static function_result pars_http_respones(http_context *cont, char *buff);
static function_result recive_respons(http_context *cont);

/*===========Implementation static functions=============*/
static function_result get_host_from(uint8_t *file, uint8_t **host)
{
	uint8_t *start, *end, *ret_host;
	uint32_t file_size, copy_size;
	file_size = strlen((const char *) file);
	//skip http or https ://
	if (file_size < HTTP_SIZE)
	{
		return ERROR_BAD_DATA;
	}

	if (strstr((const char *) file, "http://") != NULL)
	{
		start = file + HTTP_SIZE;
	}
	else if (strstr((const char *) file, "https://"))
	{
		start = file + HTTPs_SIZE;
	}
	else
	{
		start = file;
	}
	end = (uint8_t *) strstr((const char *) start, "/");
	if (end == NULL)
	{
		end = file + file_size;
	}

	copy_size = end - start;
	ret_host = (uint8_t*) malloc(copy_size + 1);
	memset(ret_host, 0, copy_size + 1);
	*host = memmove((void*) ret_host, (void*) start, copy_size);

	return RET_OK;
}
static function_result get_path_from(uint8_t *file, uint8_t **path)
{

	uint8_t *start;
	uint32_t file_size, copy_size;
	file_size = strlen((const char *) file);

	if (file_size < HTTP_SIZE)
	{
		return ERROR_BAD_DATA;
	}

	if (strstr((const char *) file, "http://") != NULL)
	{
		start = file + HTTP_SIZE;
	}
	else if (strstr((const char *) file, "https://"))
	{
		start = file + HTTPs_SIZE;
	}
	else
	{
		start = file;
	}
	start = (uint8_t *) strstr((const char *) start, "/");
	if (start == NULL)
	{
		*path = (void*) malloc(2);
		memset(*path, 0, 2);
		*path[0] = '/';
		return RET_OK;
	}
	copy_size = (file_size - (start - file)) + 1;
	*path = (void*) malloc(copy_size * sizeof(uint8_t));
	if (*path == NULL)
	{
		return ERROR_MEM;
	}
	*path = memmove((void*) *path, (void*) start, copy_size);
	return RET_OK;
}
static function_result get_file_name_from(uint8_t *file, uint8_t **file_name)
{
	function_result fun_res;
	uint8_t *path = NULL, *start = NULL;
	uint32_t copy_size;
	*file_name = NULL;
	fun_res = get_path_from(file, &path);
	if (fun_res != RET_OK)
	{
		return fun_res;
	}

	start = (uint8_t *) strrchr((const char *) path, '/') + 1;
	if (!start)
	{
		return RET_OK;
	}
	copy_size = strlen((char*) path) - (start - path) + 1;

	*file_name = (void*) malloc(copy_size * sizeof(uint8_t) + 1);
	memset(*file_name, 0, copy_size * sizeof(uint8_t) + 1);
	if (*file_name == NULL)
	{
		return ERROR_MEM;
	}
	*file_name = memmove((void*) *file_name, (void*) start, copy_size);
	free(path);
	path = NULL;
	return RET_OK;
}
static char* make_request(http_context *cont)
{
	char *req;
	char *tpl = "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
	int size;
	int host_size, path_size, name_size;
	host_size = cont->host ? strlen((char*) cont->host) : 0;
	path_size = cont->path ? strlen((char*) cont->path) : 0;
	name_size = cont->name ? strlen((char*) cont->name) : 0;
	size = host_size + path_size + name_size + strlen(tpl) + 128; //128 is padding
	req = malloc(size);
	memset(req, 0, size);
	sprintf(req, tpl, cont->path, cont->host, USER_AGENT);

	return req;
}
static int create_tcp_socket()
{
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		return -1;
	}
	return sock;
}
static char *get_ip(char *host)
{

	struct hostent *hent;
	int iplen = 15;
	char *ip = (char *) malloc(iplen + 1);
	memset(ip, 0, iplen + 1);

	if ((hent = gethostbyname(host)) == NULL)
	{
		return NULL;
	}

	if (inet_ntop(AF_INET, (void *) hent->h_addr_list[0], ip, iplen) == NULL)
	{
		return NULL;
	}

	return ip;

}
static function_result pars_http_respones(http_context *cont, char *buff)
{
	char *tmp_start;
	int recv_data_len;
	int state;

	http_respones *respons;
	function_result ret;
	ret = init_http_respones(&respons);
	if (ret != RET_OK)
	{
		return ret;
	}

	tmp_start = strstr(buff, " ");
	if (tmp_start)
	{
		state = atoi(tmp_start);
		respons->http_status = state;
	}

	tmp_start = strstr(buff, "Content-Length:");
	if (tmp_start)
	{
		tmp_start += strlen("Content-Length:");
		recv_data_len = atoi(tmp_start);
		respons->recv_data_len = recv_data_len;
	}

	cont->respones = respons;

	return RET_OK;
}
static function_result recive_respons(http_context *cont)
{
	int tmpres;
	char *buff = malloc(BUFFER_SIZE);
	memset(buff, 0, BUFFER_SIZE);
	int cnt = 0;
	char recv_buf[2];
	int state = 0;
	while ((tmpres = recv(cont->socket, recv_buf, 1, 0)) > 0)
	{
		if (recv_buf[0] == '\r' || recv_buf[0] == '\n')
		{
			state += 1;
		}
		else
		{
			state = 0;
		}
		buff[cnt++] = recv_buf[0];
		if (state == 4)
		{
			buff[cnt++] = '\0';
			break;
		}
	}

	function_result res = pars_http_respones(cont, buff);
	free(buff);
	return res;
}

/*=========== Global functions definition ===========*/
function_result init_http_context(http_context **cont, uint8_t *file)
{
	http_context *http_con;
	uint8_t *host;
	uint8_t *path;
	uint8_t *file_name;

	if (get_host_from(file, &host) != RET_OK)
	{
		return ERROR_BAD_DATA;
	}

	if (get_path_from(file, &path) != RET_OK)
	{
		return ERROR_BAD_DATA;
	}

	if (get_file_name_from(file, &file_name) != RET_OK)
	{
		return ERROR_BAD_DATA;
	}

	http_con = (http_context*) malloc(sizeof(http_context));
	memset(http_con, 0, sizeof(http_context));
	http_con->host = host;
	http_con->path = path;
	http_con->name = file_name;

	*cont = http_con;

	return RET_OK;
}
function_result delete_http_context(http_context **cont)
{
	if (*cont == NULL)
	{
		return RET_OK;
	}

	if ((*cont)->host)
	{
		free((*cont)->host);
		(*cont)->host = NULL;
	}

	if ((*cont)->path)
	{
		free((*cont)->path);
		(*cont)->path = NULL;
	}

	if ((*cont)->name)
	{
		free((*cont)->name);
		(*cont)->name = NULL;
	}

	if ((*cont)->ip_adr)
	{
		free((*cont)->ip_adr);
		(*cont)->ip_adr = NULL;
	}

	if ((*cont)->remote)
	{
		free((*cont)->remote);
		(*cont)->remote = NULL;
	}
	if ((*cont)->respones)
	{
		free_http_respones(&(*cont)->respones);
	}

	free((*cont));
	*cont = NULL;
	return RET_OK;
}
function_result connect_http_context(http_context *cont)
{
	char *request, *ip_adr;
	int socket, tmpres;
	struct sockaddr_in *remote;
	http_time *time;
	init_timer(&time);
	start_timer(time);

	request = make_request(cont);
	if (request == NULL)
	{
		return ERROR_BAD_DATA;
	}
	socket = create_tcp_socket();
	if (socket < 0)
	{
		free_timer(&time);
		return ERROR_CONNECTION;
	}

	ip_adr = get_ip((char*) cont->host);

	remote = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
	memset(remote, 0, sizeof(struct sockaddr_in));
	remote->sin_family = AF_INET;
	tmpres = inet_pton(AF_INET, ip_adr, (void *) (&(remote->sin_addr.s_addr)));
	if (tmpres < 0)
	{
		free_timer(&time);
		return ERROR_BAD_DATA;
	}
	else if (tmpres == 0)
	{
		free_timer(&time);
		return ERROR_BAD_DATA;
	}

	remote->sin_port = htons(PORT);

	if (connect(socket, (struct sockaddr *) remote, sizeof(struct sockaddr)) < 0)
	{
		free_timer(&time);
		return ERROR_CONNECTION;
	}
	stop_timer(time);
	cont->connection_time = time_elapsed_msecond(time);

	start_timer(time);
	int sent = 0;
	while (sent < strlen(request))
	{
		tmpres = send(socket, request + sent, strlen(request) - sent, 0);

		if (tmpres == -1)
		{
			free_timer(&time);
			return ERROR_BAD_DATA;
		}

		sent += tmpres;
	}
	stop_timer(time);
	cont->request_time = time_elapsed_msecond(time);

	cont->socket = socket;
	cont->remote = remote;
	cont->ip_adr = ip_adr;
	cont->is_finished = false;
	cont->total_readed_size = 0;
	start_timer(time);
	function_result result = recive_respons(cont);
	stop_timer(time);

	cont->request_time = time_elapsed_msecond(time);

	free(request);
	free_timer(&time);

	return result;
}
function_result read_all_http_context(http_context *cont, uint8_t** ret_buffer, uint32_t *readed_size)
{
	char *buffer = malloc(cont->respones->recv_data_len);
	int rec_data_len = cont->respones->recv_data_len;
	int tmpres;
	uint32_t recv_l = 0;

	char *top_buf = buffer;
	while ((tmpres = recv(cont->socket, top_buf, rec_data_len, 0)) > 0)
	{
		top_buf += tmpres;
		rec_data_len -= tmpres;
		recv_l += (uint32_t) tmpres;
	}
	*ret_buffer = (uint8_t*) buffer;
	*readed_size = recv_l;
	cont->total_readed_size = recv_l;
	cont->is_finished = true;

	return RET_OK;
}
function_result read_packet_http_context(http_context *cont, uint8_t* ret_buffer, uint32_t ret_buffer_size, uint32_t *readed_size)
{
	int tmpres;
	uint32_t recv_l = 0;
	char *top_buf = (char*) ret_buffer;
	http_time *time;
	init_timer(&time);
	start_timer(time);

	while ((tmpres = recv(cont->socket, top_buf, ret_buffer_size, 0)) > 0)
	{
		top_buf += tmpres;
		ret_buffer_size -= tmpres;
		recv_l += (uint32_t) tmpres;
		if (ret_buffer_size <= 0)
		{
			break;
		}
	}
	stop_timer(time);

	cont->total_downlod_time += time_elapsed_msecond(time);

	if (cont->downlod_speed)
	{
		cont->downlod_speed = cont->downlod_speed * (1 - recv_l / (double) (recv_l + cont->respones->recv_data_len))
				+ (recv_l / time_elapsed_msecond(time)) * (recv_l / (double) (recv_l + cont->respones->recv_data_len));
	}
	else
	{
		cont->downlod_speed = recv_l / time_elapsed_msecond(time);
	}
	*readed_size = recv_l;
	cont->total_readed_size += recv_l;
	if (cont->total_readed_size >= cont->respones->recv_data_len) // == if this never happened
	{
		cont->is_finished = true;
	}
	free_timer(&time);
	return RET_OK;
}
