#include "work_que.h"

//尾插法
void que_insert(pque_t pq, pnode_t pnew)
{
	if(!pq->que_head)
	{
		pq->que_head = pnew;
		pq->que_tail = pnew;
	} else{
		pq->que_tail->pNext = pnew;
		pq->que_tail = pnew;
	}
	++pq->size;
}

//删除链表头结点
void que_get(pque_t pq, pnode_t* pcur)
{
	*pcur = pq->que_head;
	if(NULL == *pcur)
	{
		return;
	}
	if(-1 == (*pcur)->new_fd) //表示accept失败, 或者服务器要退出时
	{
		//这里不取下结点，直接解锁退出子线程
		pthread_mutex_unlock(&pq->que_mutex);
		pthread_exit(NULL);
	}
		 
	pq->que_head = pq->que_head->pNext;
	if(!pq->que_head)
	{
		pq->que_tail = NULL;
	}
	--pq->size;
}

//头插法
void que_insert_exit(pque_t pq, pnode_t pnew)
//插入标志结束的任务结点
{
	if(NULL == pq->que_head)
	{
		pq->que_head = pnew;
		pq->que_tail = pnew;
	} else{
		pnew->pNext = pq->que_head;
		pq->que_head = pnew;
	}
	++pq->size;	
}
