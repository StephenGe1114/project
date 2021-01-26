#include "json/json.h"
#include "tran_n.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <iostream>
#include <sstream>
using std::cout;
using std::endl;

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

int main(int argc, const char *argv[])
{
    int sfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
        ERR_EXIT("socket");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.153.129"); //localhost
    //addr.sin_addr.s_addr = INADDR_ANY; //localhost
    addr.sin_port = htons(8888);
    socklen_t len = sizeof addr;
    if(connect(sfd, (struct sockaddr*)&addr, len) == -1)
        ERR_EXIT("Connect");

	int ret;
	int epfd = epoll_create(1);
	struct epoll_event event, evs[2];
	event.events = EPOLLIN;
	event.data.fd = sfd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	if(-1 == ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	event.data.fd = STDIN_FILENO;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	if(-1 == ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	int i;
	char buf[1024] = {0};
	std::string word;
	while(1)
	{
		memset(evs, 0, sizeof(evs));
		ret = epoll_wait(epfd, evs, 2, -1);
		for(i = 0; i < ret; ++i)
		{
//				cout << "1" << endl;
			if(sfd == evs[i].data.fd)
			{
				bzero(buf, sizeof(buf));
				ret = recv(sfd, buf, sizeof(buf), 0);
//				cout << "3" << endl;
				if(-1 == ret)
				{
					printf("服务器断开\n");
					break;
				}
			//	std::string test = buf;
			//	Json::Reader reader;
			//	Json::Value root;
			//	if(reader.parse(test, root))
		//				std::cout << root[word].asString() << std::endl;
				cout << buf << endl;
			}
			if(EPOLLIN == evs[i].events && STDIN_FILENO == evs[i].data.fd)
			{
//				cout << "2" << endl;
				bzero(buf, sizeof(buf));
				read(STDIN_FILENO, buf, sizeof(buf));
				word = buf;
				send(sfd, buf, strlen(buf)-1, 0);
			}
		}
	}
	close(epfd);
	close(sfd);
    return 0;
}


