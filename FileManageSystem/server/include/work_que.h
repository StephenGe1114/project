#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"

typedef struct tag_node{ //任务节点
	int new_fd; //accept客户端请求生成的描述符
	struct tag_node* pNext;
}node_t, *pnode_t ;

typedef struct{ //任务队列
	pnode_t que_head, que_tail;
	int que_capacity; //队列最大容量
	int size; //队列当前大小
	pthread_mutex_t que_mutex; //任务队列锁,入队出队都要加解锁
}que_t, *pque_t;

void que_insert(pque_t , pnode_t);
void que_get(pque_t , pnode_t*);
void que_insert_exit(pque_t , pnode_t);
#endif
