/*
 *      Author: Zoran Zivanovic
 *      Project: http communication
 *      Version: 0.1
 */

#include <stdlib.h>
#include "http.h"

#include <sys/stat.h> /* struct stat, fchmod (), stat (), S_ISREG, S_ISDIR */


// Process has done i out of n rounds,
// and we want a bar of width w and resolution r.
// w - bar with
// r - resolution
// n - number of times
// x - current time
static void loadBar(int x, int n, int r, int w)
{
	int p;
    // Only update r times.
   // if ( x % (n/r) != 0 ) return;

    // Calculuate the ratio of complete-to-incomplete.
    float ratio = x/(float)n;
    int   c     = ratio * w;

    // Show the percentage complete.
    printf("%3d%% [", (int)(ratio*100) );

    // Show the load bar.
    for (p=0; p<c; p++)
       printf("=");

    printf(">");
    for (p=c+1; p<w; p++)
       printf(" ");

    // ANSI Control codes to go back to the
    // previous line and clear it.
    printf("]\n\033[F\033[J");
	//printf("]\n33[1A33[2K");
}


int main(int argc, char* argvc[])
{
	http_context *context = NULL;
	//uint8_t *url = "http://katastar.rgz.gov.rs/KnWebPublic/files/Katastar%20nepokretnosti-knweb20120905.zip";
	//uint8_t *url ="www.google.rs";
	if (argc < 2)
	{
		printf("\n 2 parameters are required  \n");
		return -1;
	}
	uint8_t *url = (uint8_t *) argvc[1];
	function_result result = init_http_context(&context, url);
	if (result != RET_OK)
	{
		return -1;
	}

	result = connect_http_context(context);
	if (result != RET_OK)
	{
		return -1;
	}

	uint8_t *buffer;

	int new_old = 0;
#if everiting
	uint32_t readed_size = 0;
	result = read_all_http_context(context, &buffer, &readed_size);

	if (result != RET_OK)
	{
		return -1;
	}

	struct stat sts;
	if (stat((char*)context->name, &sts)==0)
	{
		printf ("The file %s already exist...\n",(char*)context->name);
		printf("delete existing file y/n ?");
		char val=getchar();
		if(val=='y' || val == 'Y')
		{
			new_old=1;
		}
		else
		{
			printf("appending ...\n");
		}
	}

	FILE *fh;
	if(new_old)
	fh=fopen((char*)context->name,"w");
	else
	fh=fopen((char*)context->name,"a");

	fwrite(buffer,1,readed_size, fh);
	fflush(fh);
	fclose(fh);
#else
	buffer = malloc(1024*1024);
	uint32_t read_buff_size;
	struct stat sts;
	if (stat((char*) context->name, &sts) == 0)
	{
		printf("The file %s already exist...\n", (char*) context->name);
		printf("delete existing file y/n ?");
		char val = getchar();
		if (val == 'y' || val == 'Y')
		{
			new_old = 1;
		}
		else
		{
			printf("appending ...\n");
		}
	}

	FILE *fh;
	if (new_old)
		fh = fopen((char*) context->name, "w");
	else
		fh = fopen((char*) context->name, "a");
int i=0;
	do
	{
		result = read_packet_http_context(context, buffer, 1024*1024, &read_buff_size);
//		printf("%4.2f\t%4.2f\n",context->total_readed_size/context->total_downlod_time,context->downlod_speed);
		//fwrite(buffer, 1, read_buff_size, fh);
		//fflush(fh);
		i=(context->total_readed_size/(double)context->respones->recv_data_len)*100;
		//printf("i =%d\n",i);
		loadBar(i,100,1,100);

	} while (context->is_finished == false);
	fclose(fh);
#endif

	free(buffer);

	result = delete_http_context(&context);
	if (result != RET_OK)
	{
		return -1;
	}

	return 0;
}
