#ifndef _FACEPASS_KIFIFO_H__
#define _FACEPASS_KIFIFO_H__
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

//判断x是否是2的次方
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
//取a和b中最小值
#define min(a, b) (((a) < (b)) ? (a) : (b))

struct ring_buffer
{
    void         *buffer;     //缓冲区
    uint32_t     size;       //大小
    uint32_t     in;         //入口位置
    uint32_t       out;        //出口位置
    pthread_mutex_t *f_lock;    //互斥锁
};

//初始化缓冲区
struct ring_buffer* ring_buffer_init(void *buffer, uint32_t size, pthread_mutex_t *f_lock);
//释放缓冲区
void ring_buffer_free(struct ring_buffer *ring_buf);

uint32_t ring_buffer_len(const struct ring_buffer *ring_buf);

uint32_t ring_buffer_get(struct ring_buffer *ring_buf, void *buffer, uint32_t size);

uint32_t ring_buffer_put(struct ring_buffer *ring_buf, void *buffer, uint32_t size);

void ring_buffer_clean(struct ring_buffer *ring_buf);

#endif /*_FACEPASS_KIFIFO_H__*/