#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"

typedef void* (*thread_func_t)(void* p);
typedef struct{ //线程组数据结构
	pthread_t* pth_id; //线程id的起始地址
	int pthread_num; //创建的线程数目
	que_t que; //任务队列
	thread_func_t threadfunc; //线程函数
	pthread_cond_t cond; 
	short start_flag; //线程是否启动标志
}factory, *pfac;

int factory_init(pfac, thread_func_t, int, int);
int factory_start(pfac);
int tcp_start_listen(int*, char*, char*, int);
int tran_file(int);
int send_n(int, char*, int);
int recv_n(int, char*, int);
#define FILENAME "file"
typedef struct{
	int len; //控制数据,每个协议必带
	char buf[1000]; //读音频或者视频的时候，判断buf中的内容，不能用strlen,因为音频视频中本身就含有许多0；
}train;

int query_users(int*, char*, char*, char*, int*);
int query_file(int, int, char*);
int query_pwd(int, char*);
int query_file_size(char*, int*);
int query_file_size2(int, int, char*, int*);
int insert_users(char*, char*, char*, int);
int insert_dir(int, int, char*);
int insert_file(int, int,char*, char*, int );
int query_procode(int*, int);
int query_dir(int, char*, int, int*);
int query_md5(char*);
int change_path(int, int);
int delete_code(int);
int recus_dele(int);
int is_file(int, int, char*, int*);
int is_in_curdir(int, int, char*);

void get_salt(char*);

int tran_file(int);
int send_n(int, char*, int);
int recv_n(int, char*, int);

#endif
