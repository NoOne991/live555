#ifndef _FACEPASS_KIFIFO_H__
#define _FACEPASS_KIFIFO_H__
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

//�ж�x�Ƿ���2�Ĵη�
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
//ȡa��b����Сֵ
#define min(a, b) (((a) < (b)) ? (a) : (b))

struct ring_buffer
{
    void         *buffer;     //������
    uint32_t     size;       //��С
    uint32_t     in;         //���λ��
    uint32_t       out;        //����λ��
    pthread_mutex_t *f_lock;    //������
};

//��ʼ��������
struct ring_buffer* ring_buffer_init(void *buffer, uint32_t size, pthread_mutex_t *f_lock);
//�ͷŻ�����
void ring_buffer_free(struct ring_buffer *ring_buf);

uint32_t ring_buffer_len(const struct ring_buffer *ring_buf);

uint32_t ring_buffer_get(struct ring_buffer *ring_buf, void *buffer, uint32_t size);

uint32_t ring_buffer_put(struct ring_buffer *ring_buf, void *buffer, uint32_t size);

void ring_buffer_clean(struct ring_buffer *ring_buf);

#endif /*_FACEPASS_KIFIFO_H__*/