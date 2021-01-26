#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

//解决服务器与客户端发送接收速度不一致引起的协议错乱问题
int send_n(int sfd, char* p, int len)
//循环发送
{
	int total = 0;
	int ret;
	while(total < len)
	{
		ret = send(sfd, p + total, len - total, 0); //客户端断开时先返回-1，再次send时发出SIGPIPE信号
		if(-1 == ret) 
		{
			printf("client is close\n");
			return -1;
		}
		total += ret;
	}
	return 0;
}

int recv_n(int sfd, char* p, int len)
//循环接收
{
	int total = 0;
	int ret;
	while(total < len)
	{
		ret = recv(sfd, p + total, len - total, 0);
		if(0 == ret)
		{
			return -1;
		}
		total += ret;
	}
	return 0;
}
