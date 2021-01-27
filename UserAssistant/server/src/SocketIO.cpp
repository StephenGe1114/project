
#include "SocketIO.h"
#include "SocketUtil.h"
#include <iostream>


namespace wd
{

SocketIO::SocketIO(int sockfd)
	: sockfd_(sockfd)
{}


size_t SocketIO::readn(char * buf, size_t count)
{
	size_t nleft = count;
	char * pbuf = buf;
	while(nleft > 0)//为什么用循环？因为读取过程中可能出错要重读
	{
		int nread = ::read(sockfd_, pbuf, nleft);
		if(-1 == nread)
		{
			if(errno == EINTR)//忽略掉中断信号
				continue;
			return EXIT_FAILURE;
		}
		else if(0 == nread) //EOF,读完了
		{
			break;
		}
		pbuf += nread;
		nleft -= nread;
	}
	return (count - nleft);
}


size_t SocketIO::writen(const char * buf, size_t count)
{
	std::cout << "writen" << std::endl;
	std::cout << buf << std::endl;
	size_t nleft = count;
	const char * pbuf = buf;
	while(nleft > 0)
	{
		int nwrite = ::send(sockfd_, pbuf, nleft, 0);
		if(nwrite == -1)
		{
			if(errno == EINTR)
				continue;
			return EXIT_FAILURE;
		}
		nleft -= nwrite;
		pbuf += nwrite;
	}
	return (count - nleft);
}

size_t SocketIO::recv_peek(char * buf, size_t count)
{
	int nread;
	do
	{
		//当指定MSG_PEEK标志时， 可以查看下一个要读取的数据但不真正取走它
		nread = ::recv(sockfd_, buf, count, MSG_PEEK);
	}while(nread == -1 && errno == EINTR);
	return nread;
}

size_t SocketIO::readline(char * buf, size_t maxlen)
{
	size_t nleft = maxlen - 1;
	char * pbuf = buf;
	size_t total = 0;
	while(nleft > 0)
	{
		int nread = recv_peek(pbuf, nleft);
		if(nread <= 0)
			return nread;

		//检查'\n'
		for(size_t idx = 0; idx != nread; ++idx)
		{
			if(pbuf[idx] == '\n')
			{
				size_t nsize = idx + 1;
				if(readn(pbuf, nsize) != nsize)
					return EXIT_FAILURE;
				pbuf += nsize;
				total += nsize;
				*pbuf = 0;//这就是一行的末尾,用来分开另一行
				return total;
			}
		}
		if(readn(pbuf, nread) != nread)
			return EXIT_FAILURE;
		pbuf += nread;
		nleft -= nread;
		total += nread;
	}
	*pbuf = 0;
	return maxlen - 1;
}

}// end of namespace wd
