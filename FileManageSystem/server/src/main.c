#include "factory.h"

typedef struct{
	char ip[128];
	char port[128];
	char filename[128];
	int offset; //文件偏移值
	int down_size; //需要下载的大小
	int userid;
}down, *pdown;

int exit_fds[2];
char ip[6][128] = {0};
char port[6][128] = {0};

void sig_exit_func(int signum)
{
	char flag;
	write(exit_fds[1], &flag, 1);
}

void* pthread_func_gets(void* p)
//当所传的文件大小超过5字节时，多点下载
{
	pdown pd = (pdown)p;
	int sfd;
	tcp_start_listen(&sfd, pd->ip, pd->port, 1000);
	int new_fd = accept(sfd, NULL, NULL);	
	printf("phtread_func_gets\n");
	recv(new_fd, (char*)pd, sizeof(down), 0); //接收偏移
	char path[128] = {0};
	char buf[128] = {0};
	sprintf(path, "%s%s", "/home/jack/learned/test/netdisc/", pd->filename);
	int fd = open(path, O_RDWR);
//	printf("offset = %d\n", pd->offset);
	int ret = lseek(fd, pd->offset, 0);	
	if(-1 == ret)
	{
		printf("lseek error!\n");
	}
	train t;
	while(pd->down_size > 0)
	{
		bzero(buf, sizeof(buf));
		if(pd->down_size > sizeof(buf))
			t.len = read(fd, t.buf, sizeof(buf)); 
		else
			t.len = read(fd, t.buf, pd->down_size); 
		pd->down_size -= t.len;
//		printf("port = %d, t.buf = %s\n", atoi(pd->port), t.buf);
		ret = send_n(new_fd, (char*)&t, t.len + 4);
		if( -1 == ret)
			goto end;
	}
	t.len = 0;
	ret = send_n(new_fd, (char*)&t, t.len + 4);
end:
	close(new_fd);	
	close(fd);
	close(sfd);
	if( -1 == ret)
		pthread_exit((void*)-1);
	pthread_exit(NULL);
}

void* pthread_func(void* p)
{
	pfac pf = (pfac)p;
	pque_t pq = &pf->que;
	pnode_t pcur;
	char buf[1024] = {0};
	char name[128] = {0};
	char passwd[1024] = {0};
	char salt[128] = {0};
	char cmd1[8] = {0};
	char cmd2[128] = {0};
	int path;
	int ret;
	int userid;
	while(1)
	{
		pthread_mutex_lock(&pq->que_mutex);
		if(0 == pq->size)
		{
			pthread_cond_wait(&pf->cond, &pq->que_mutex);
		}
		que_get(pq, &pcur);
		pthread_mutex_unlock(&pq->que_mutex);
		if(pcur != NULL) //完成该任务
		{
relogin:
			bzero(name, sizeof(name));
			bzero(buf, sizeof(buf));
			bzero(passwd, sizeof(passwd));
			bzero(salt, sizeof(salt));
			path = 0;
			recv(pcur->new_fd, name, sizeof(name), 0); //接收名字
			ret = query_users(&userid, name, salt, passwd, &path);
			if(0 == ret) //用户存在
			{
				//send(pcur->new_fd, "1", 1, 0); //注意这里不要连续send，可能被客户端的recv一次性接收; 如果要连续send的话，那要控制客户端第一次recv的长度刚好为第一次send的长度
				send(pcur->new_fd, salt, sizeof(salt), 0); //发送盐值, 注意用sizeof，盐值中可能包含0
				bzero(buf, sizeof(buf));
				recv(pcur->new_fd, buf, sizeof(buf), 0); //接收密文
				if(strcmp(passwd, buf) == 0) //密码正确
				{
					send(pcur->new_fd, "1", 1, 0);
				} else{
					send(pcur->new_fd, "0", 1, 0);
					goto relogin;
				}
			} else{ //用户不存在时, 注册
				get_salt(salt);
				sprintf(buf, "%s%s%s", "$6$", salt, "$");
				strcpy(salt, buf);
				send(pcur->new_fd, salt, sizeof(salt), 0); //发送盐值, 注意用sizeof，盐值中可能包含0
				recv(pcur->new_fd, passwd, sizeof(passwd), 0); //接受密文
				path = 0;
				ret = insert_users(name, salt, passwd, path);
				send(pcur->new_fd, "0", 1, 0);
				goto relogin;						
			}
			
			while(1)
			{
				bzero(cmd1, sizeof(cmd1));
				bzero(cmd2, sizeof(cmd2));
				recv(pcur->new_fd, cmd1, sizeof(cmd1), 0); //接收命令 
				if(!strcmp(cmd1, "quit"))
					break;
				else if(!strcmp(cmd1, "gets")){
					send(pcur->new_fd, "1", 1, 0);
					recv(pcur->new_fd, cmd2, sizeof(cmd2), 0); //接收文件的名字
					int size;
					query_file_size2(path, userid, cmd2, &size);
					//send(pcur->new_fd, (char*)&size, 4, 0);//发送文件的大小
					down d[5];
					pthread_t pthread_id[5];
					for(int i = 0; i < 5; ++i) //发送ip地址
					{
						bzero(&d[i], sizeof(down));
						strcpy(d[i].filename, cmd2);
						if(4 == i)
						{
							d[i].offset = size / 5 * 4;
							d[i].down_size = size - d[i].offset;
						} else{
							d[i].offset = size / 5 * i;
							d[i].down_size = size / 5;
						}
						strcpy(d[i].ip, ip[i+1]);
						strcpy(d[i].port, port[i+1]);
						d[i].userid = userid;
						send(pcur->new_fd, (char*)&d[i], sizeof(down), 0);
						pthread_create(pthread_id + i, NULL, pthread_func_gets, (void*)&d[i]);
					}
					long l;
					for(int i = 0; i < 5; ++i)
					{
						pthread_join(pthread_id[i], (void**)&l);
					}
					printf("l = %ld\n", l);
					if(-1 == l)
						goto end;				

				} else if(!strcmp(cmd1, "puts")){
					send(pcur->new_fd, "1", 1, 0);
					recv(pcur->new_fd, cmd2, sizeof(cmd2), 0); //接收文件的名字
					bzero(name, sizeof(name));
					strcpy(name, cmd2);
					bzero(cmd2, sizeof(cmd2));
					send(pcur->new_fd, "1", 1, 0); //文件名字接收成功
					recv(pcur->new_fd, cmd2, sizeof(cmd2), 0); //接受文件的md5码
					//printf("%s\n", cmd2);
					ret = query_md5(cmd2);
					//printf("ret = %d\n", ret);
					if(ret > 0) //网盘中已存在该文件
					{
						send(pcur->new_fd, "exist", 5, 0);
						if(!is_in_curdir(userid, path, cmd2))
						//如果不在当前用户目录下, 则插入
						{
							int size = 0;
							query_file_size(cmd2, &size);
							insert_file(path, userid, name, cmd2, size);
						} 
					} else{ //文件不存在
						send(pcur->new_fd, "not exist", 10, 0);
						int len;
						off_t file_size;
						//接收文件大小
						recv_n(pcur->new_fd, (char*)&len, 4);
						recv_n(pcur->new_fd, (char*)&file_size, len);
						bzero(buf, sizeof(buf));
						sprintf(buf, "%s%s", "/home/jack/learned/test/netdisc/", name);
						//printf("buf = %s\n", buf);
						int fd = open(buf, O_RDWR | O_CREAT, 0666); 
						if(-1 == fd)
						{
							perror("open");
							return NULL;
						}
						while(1)
						{
							ret = recv_n(pcur->new_fd, (char*)&len, 4);
							if(-1 != ret && len > 0)
							{
								bzero(buf, sizeof(buf));
								ret = recv_n(pcur->new_fd, buf, len);
								if(-1 == ret)
									break;
								write(fd, buf, len);
							} else{
								break;
							}
						}
						close(fd);
						insert_file(path, userid, name, cmd2, file_size);
					}
					
				} else if(!strcmp(cmd1, "cd")){
					send(pcur->new_fd, "1", 1, 0);
					recv(pcur->new_fd, cmd2, sizeof(cmd2), 0); //接受目录名
					if(!strcmp(cmd2, ".."))
					{
						bzero(buf, sizeof(buf));
						if(0 == path)
						{
							query_pwd(path, buf);
							send(pcur->new_fd, buf, strlen(buf), 0);
						} else{
							query_procode(&path, path);
							query_pwd(path, buf);
							change_path(userid, path);
							send(pcur->new_fd, buf, strlen(buf), 0);
						}
					} else{ //cd 目录
						bzero(buf, sizeof(buf));
						ret = query_dir(path, cmd2, userid, &path);
						if(-1 == ret)
							send(pcur->new_fd, "the dir don't exist!", 32, 0);
						else{
							query_pwd(path, buf);
							change_path(userid, path);
							send(pcur->new_fd, buf, strlen(buf), 0);
						}
					}
				} else if(!strcmp(cmd1, "rm")){
					send(pcur->new_fd, "1", 1, 0);
					recv(pcur->new_fd, cmd2, sizeof(cmd2), 0); //接收第二个命令符
					int code;
					ret = query_dir(path, cmd2, userid, &code);
					if(-1 == ret) //不是目录或目录不存在
					{
						if(is_file(path, userid, cmd2, &code)) //是文件
							delete_code(code);

						send(pcur->new_fd, "the dir don't exist!", 32, 0);
					}
					else{
						recus_dele(code);
						delete_code(code);
						send(pcur->new_fd, "1", 1, 0);
					}

				} else if(!strcmp(cmd1, "mkdir")){
					send(pcur->new_fd, "1", 1, 0);
					recv(pcur->new_fd, cmd2, sizeof(cmd2), 0); //接收第二个命令符
					insert_dir(path, userid, cmd2);
					send(pcur->new_fd, "1", 1, 0);
				} else if(!strcmp(cmd1, "ls")){
					bzero(buf, sizeof(buf));
					query_file(path, userid, buf);
					send(pcur->new_fd, buf, strlen(buf) - 1, 0);
				} else if(!strcmp(cmd1, "pwd")){
					bzero(buf, sizeof(buf));
					//printf("path = %d\n", path);
					query_pwd(path, buf);
					send(pcur->new_fd, buf, strlen(buf), 0);
				}
			}
end:
			printf("bye!\n");
			free(pcur); //该客户请求完成
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	args_check(argc, 2);
	pipe(exit_fds);
	FILE* fp = fopen("../conf/server.conf", "rb");
	char config_buf[128] = {0};
	for(int i = 0; i < 6; ++i)
	{
		fgets(config_buf, 128, fp);
		sscanf(config_buf, "%s %s", ip[i], port[i]);
	}
	fclose(fp);

	if(fork()) //父进程
	{
		close(exit_fds[0]);
		signal(SIGUSR1, sig_exit_func);
		pid_t pid;
		pid = wait(NULL);
		printf("pid = %d\n", pid);
		return 0;
	}

	//子进程
	close(exit_fds[1]);
	factory f; //主要数据结构
	int thread_num = 16;
	int capacity = 1000;
	factory_init(&f, pthread_func, thread_num, capacity);
	factory_start(&f);
	int sfd;
	tcp_start_listen(&sfd, ip[0], port[0], capacity);
	int new_fd;
	pque_t pq = &f.que;
	pnode_t pnew;
	int ret;
	int epfd = epoll_create(1);
	struct epoll_event event, evs[2];
	event.events = EPOLLIN;
	event.data.fd = sfd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	check_error(-1, ret, "epoll_ctl");
	event.data.fd = exit_fds[0];
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, exit_fds[0], &event);
	check_error(-1, ret, "epoll_ctl");
	int i;
	while(1)
	{
		ret = epoll_wait(epfd, evs, 2, -1);
		for(i = 0; i < ret; ++i)
		{
			if(sfd == evs[i].data.fd)
			{
				new_fd = accept(sfd, NULL, NULL);	
				pnew = (pnode_t)calloc(1, sizeof(node_t));
				pnew->new_fd = new_fd;
				pthread_mutex_lock(&pq->que_mutex);
				que_insert(pq, pnew);
				pthread_mutex_unlock(&pq->que_mutex);
				pthread_cond_signal(&f.cond); //唤醒一个子线程
			}
			if(exit_fds[0] == evs[i].data.fd)
			{
				close(sfd); //不再接收新的网络请求
				pnew = (pnode_t)calloc(1, sizeof(node_t));
				pnew->new_fd = -1; //标志结束的任务结点，插在任务队列的队首，后面的任务全部放弃
				pthread_mutex_lock(&pq->que_mutex);
				que_insert_exit(pq, pnew);
				pthread_mutex_unlock(&pq->que_mutex);
				pthread_cond_broadcast(&f.cond); //所有的线程都运转，先后拿到标志结束的任务结点，相继退出
				for(i = 0; i < f.pthread_num; ++i)
				{
					pthread_join(f.pth_id[i], NULL);
				}
				exit(0);
			}
		}
	}
}
