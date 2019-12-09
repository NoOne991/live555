#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
//#include <time.h>
#include <sys/time.h>

#include "RecvStream.hh"
#include "RingBuffer.hh"


#define LIVE555_SERVER_IP "127.0.0.1"
#define LIVE555_SERVER_PORT  6666

#define BUFFER_SIZE 128*1024
#define MAX_SIZE 128*1024

int g_sock_recv = -1;

static struct ring_buffer *g_ring_buf = NULL;


void *thread_recv(void* parm)
{
	int rc;
	fd_set rSet;
    struct timeval tm;
	
	char* recvBuf = (char*)malloc(MAX_SIZE);
	
    FD_ZERO(&rSet);
	while (1)
	{
		FD_SET(g_sock_recv, &rSet);
        tm.tv_sec = 1;
        tm.tv_usec = 0;
        rc = select(g_sock_recv + 1, &rSet, NULL, NULL, &tm);
        if (rc == -1)
        {
            perror("select()");
			usleep(1000 * 1000);
        }
		else if (rc == 0)
        {
            printf("Timeout, select again now ...\n");
        }
		else
		{
			int ret = recvfrom(g_sock_recv, recvBuf, MAX_SIZE, 0, NULL, NULL);
			if (ret > 0)
			{
				ring_buffer_put(g_ring_buf, recvBuf, ret);
			}
		}
	}
	free(recvBuf);
	return NULL;
}

int64_t util_gettime(void)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

static int64_t last_read_time = 0;

int64_t CRecoFace::read_from_bs(void *fFid, unsigned char* fTo, int64_t fMaxSize)
{
    int64_t t = util_gettime();
    if (t - last_read_time > 1000000)
    {
        ring_buffer_clean(g_ring_buf);
        printf("ring_buffer_clean\n");
    }
    last_read_time = t;
	int ringLen = ring_buffer_len(g_ring_buf);
	if (ringLen == 0)
	{
		usleep(10*1000);
		return 0;
	}
	int minLen = min(fMaxSize, ringLen);
	int len = ring_buffer_get(g_ring_buf, fTo, minLen);

	return len;
}

int bitstream_recv_sock(unsigned short port)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket()");
        return -1;
    }
    
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr(LIVE555_SERVER_IP);
    my_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)))
    {
        perror("bind()");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int CRecoFace::vedio_init(void)
{
	pthread_mutex_t *f_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(f_lock, NULL) != 0)
    {
		fprintf(stderr, "Failed init mutex,errno:%u,reason:%s\n",
        errno, strerror(errno));
		return -1;
    }

	void* buffer = (void *)malloc(BUFFER_SIZE);
	g_ring_buf = ring_buffer_init(buffer, BUFFER_SIZE, f_lock);
    if (!g_ring_buf)
    {
		fprintf(stderr, "Failed to init ring buffer.\n");
		return -1;
    }

	g_sock_recv = bitstream_recv_sock(LIVE555_SERVER_PORT);
	if (-1 == g_sock_recv)
	{
		printf("bitstream_recv_sock failed\n");
		goto EXIT;
	}

	/* create recv thread*/
	pthread_t thread_recv_id;
	if(pthread_create(&thread_recv_id, NULL, thread_recv, NULL) != 0)
	{
		printf("Create thread_recv fail !\n");
	}

    return 0;

EXIT:

	if(thread_recv_id != 0) 
	{                 
        pthread_join(thread_recv_id, NULL);
    }

    if (g_sock_recv != -1)
    {
		close(g_sock_recv);
    }

	return -1;
}

void CRecoFace::vedio_release(void)
{
	if (g_ring_buf != NULL)
	{
		ring_buffer_free(g_ring_buf);
		g_ring_buf = NULL;
	}
}