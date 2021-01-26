#include "func.h"

int tcp_connect(char* ip, char* port)
{
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	int ret;
	struct sockaddr_in ser;
	bzero(&ser, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(port));//端口号转换成网络字节序
	ser.sin_addr.s_addr = inet_addr(ip);//点分10进制的ip地址转换为网络字节序
	ret = connect(sfd, (struct sockaddr*)&ser, sizeof(ser));
	check_error(-1, ret, "connect");
	return sfd;
	return 0; 
}
