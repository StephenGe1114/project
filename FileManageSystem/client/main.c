#include "func.h" 
typedef struct{
	char filename[128];
	int sfd;
}net, *pnet;

typedef struct{
	char ip[128];
	char port[128];
	char filename[128];
	int  offset;
	int down_size;
	//off_t total_size;
	int userid;
}down, *pdown;

void* pthread_func_gets(void* p)
{
	pdown pd = (pdown)p;
	int sfd = tcp_connect(pd->ip, pd->port);
	int port = atoi(pd->port);
	int ret;
	int len;
	char buf[128] = {0};
	int fd;
	if(!is_exist(pd->userid, pd->filename, port)) //文件不存在
	{
		printf("file is not exist\n");
		insert_progress(pd->userid, pd->filename, pd->offset, pd->down_size, port); //将文件信息插入process表中
		fd = open(pd->filename, O_RDWR | O_CREAT, 0666);
	} else{ //文件存在
		printf("file is exist\n");
		query_offset(port, pd->userid, pd->filename, &(pd->offset), &(pd->down_size)); //查询偏移值
		fd = open(pd->filename, O_RDWR);
	}
	send(sfd, (char*)pd, sizeof(down), 0); //发送偏移
	ret = lseek(fd, pd->offset, 0);
	if(-1 == ret)
		printf("lseek error!\n");
	while(1)
	{
		ret = recv_n(sfd, (char*)&len, 4);
		if(-1 != ret && len > 0)
		{
			ret = recv_n(sfd, buf, len);
			if(-1 == ret)
			{
				printf("服务器断开\n");
				break;
			}
			write(fd, buf, len);
			//printf("buf = %s\n", buf);
			pd->down_size -= len;
			pd->offset += len;
			update_offset(port, pd->userid, pd->filename, pd->offset, pd->down_size); //将新的偏移值和需要下载的文件大小插入process表中
		} else
		{
			printf("服务器断开或者发送完毕\n");
			break;
		}
	}
	close(fd);
	close(sfd);
	pthread_exit(NULL);
}

void* pthread_func_puts(void* p)
{
	pnet pn = (pnet)p;
	char md5[128] = {0};
	char buf[128] = {0};
	send(pn->sfd, pn->filename, strlen(pn->filename), 0);
	recv(pn->sfd, buf, sizeof(buf), 0); //接收成功标志
	get_md5(pn->filename, md5);
	//printf("%s\n", md5);
	send(pn->sfd, md5, sizeof(md5), 0);
	bzero(buf, sizeof(buf));
	recv(pn->sfd, buf, sizeof(buf), 0); 
	//printf("%s\n", buf);
	if(!strcmp(buf, "exist")) //网盘中已存在该文件
	{
		printf("puts successfully!\n");
	} else{ //网盘中不存在该文件,上传文件
		train t;
		int ret;
		int fd = open(pn->filename, O_RDONLY);	
		if(-1 == fd)
		{
			perror("open");
			return NULL;
		}
		struct stat buf1;
		fstat(fd, &buf1); 
		off_t file_size;
		double puts_size = 0;
		file_size = buf1.st_size;
		t.len = sizeof(buf1.st_size);
		memcpy(t.buf, &buf1.st_size, sizeof(off_t));
		ret = send_n(pn->sfd, (char*)&t, 4 + t.len);
		if(-1 == ret)
			goto end;
		//按秒打印上传的百分比
		time_t start, end;
		start = time(NULL);
		end = time(NULL);
		while((t.len = read(fd, t.buf, sizeof(t.buf))) > 0)
		{
			ret = send_n(pn->sfd, (char*)&t, 4 + t.len);
			if(-1 == ret)
			{
				printf("puts percent %5.2f%s\n", puts_size / file_size * 100, "%");
				goto end;
			}
			puts_size += t.len;
			end = time(NULL);
			if(end - start >= 1)
			{
				printf("puts percent %5.2f%s\r", puts_size / file_size * 100, "%");
				fflush(stdout);
				start = end;
			}
		}
		ret = send_n(pn->sfd, (char*)&t, 4 + t.len);
		printf("puts percent %5.2f%s\n", puts_size / file_size * 100, "%");
end:	
		close(fd);
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	args_check(argc, 3);
	pnet pn = (pnet)calloc(1, sizeof(net));
	int sfd = tcp_connect(argv[1], argv[2]);
	pn->sfd = sfd;
	char buf[1024] = {0};
	char name[8] = {0};
	char* passwd = NULL;
	char salt[128] = {0};
	char cmd1[8] = {0};
	char cmd2[8] = {0};
relogin:
	bzero(name, sizeof(name));
	bzero(salt, sizeof(salt));
	printf("请输入用户名：");
	fflush(stdout);
	read(0, name, sizeof(name));
	send(sfd, name, strlen(name) - 1, 0); //发送名字
	bzero(buf, sizeof(buf));
	recv(sfd, salt, sizeof(salt), 0); //接收盐值
	passwd = getpass("请输入密码：");
	bzero(buf, sizeof(buf));
	const char* p = crypt(passwd, salt);
	strcpy(buf, p);
	send(sfd, buf, strlen(buf), 0); //发送密文
	bzero(buf, sizeof(buf));
	recv(sfd, buf, 1, 0); //是否匹配成功
	if(strcmp(buf, "0") == 0)
		goto relogin;
	else printf("登陆成功！\n");

	while(1)
	{
		bzero(buf, sizeof(buf));
		read(0, buf, sizeof(buf)); 
		sscanf(buf, "%s %s", cmd1,cmd2);
		send(sfd, cmd1, strlen(cmd1), 0); //发送命令
		if(!strcmp(cmd1, "quit"))
			break;
		else if(!strcmp(cmd1, "gets")) //下载文件
		{
			recv(sfd, buf, sizeof(buf), 0);  //成功标志
			send(sfd, cmd2, strlen(cmd2), 0); //发送文件的名字
			//int total_size;
			//recv(sfd, (char*)&total_size, 4, 0); //接收文件的大小
			down d[5];
			pthread_t pthread_id[5];
			for(int i = 0; i < 5; ++i)
			{
				recv(sfd, (char*)&d[i], sizeof(down), 0);
				pthread_create(pthread_id + i, NULL, pthread_func_gets, (void*)&d[i]);	
			}
			for(int i = 0; i < 5; ++i)
				pthread_join(pthread_id[i], NULL);
		} else if(!strcmp(cmd1, "puts")) //上传文件
		{
			recv(sfd, buf, sizeof(buf), 0);  //成功标志
			strcpy(pn->filename, cmd2);
			pthread_t pth_id;
			pthread_create(&pth_id, NULL, pthread_func_puts, (void*)pn);			
			pthread_join(pth_id, NULL);
		} else if(!strcmp(cmd1, "cd"))
		{
			recv(sfd, buf, sizeof(buf), 0);  //成功标志
			send(sfd, cmd2, strlen(cmd2), 0);
			bzero(buf, sizeof(buf));
			recv(sfd, buf, sizeof(buf), 0); 
			printf("%s\n", buf);
		} else if(!strcmp(cmd1, "rm")){
			recv(sfd, buf, sizeof(buf), 0); 
			send(sfd, cmd2, strlen(cmd2), 0);
			recv(sfd, buf, sizeof(buf), 0); 
		} else if(!strcmp(cmd1, "mkdir")){
			recv(sfd, buf, sizeof(buf), 0); 
			send(sfd, cmd2, strlen(cmd2), 0);
			recv(sfd, buf, sizeof(buf), 0); 
		} else if(!strcmp(cmd1, "ls") || !strcmp(cmd1, "pwd"))
		{
			bzero(buf, sizeof(buf));
			recv(sfd, buf, sizeof(buf), 0);
			printf("%s\n", buf);
		} else
			printf("cmd error\n");
	}
	
	return 0;
}
