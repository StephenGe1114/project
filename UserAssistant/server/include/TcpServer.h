 ///
 /// @file    TcpServer.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2017-05-12 16:21:37
 ///
 
#ifndef __WD_TCPSERVER_H__
#define __WD_TCPSERVER_H__

#include "Acceptor.h"
#include "EpollPoller.h"
#include "MyConf.h"
#include <string>

using std::string;

namespace wd
{


class TcpServer
{
public:
	typedef EpollPoller::EpollCallback TcpServerCallback;
	TcpServer(MyConf& mcf);

	void start();
	void stop();

	void setConnectionCallback(TcpServerCallback cb);
	void setMessageCallback(TcpServerCallback cb);
	void setCloseCallback(TcpServerCallback cb);
private:
	MyConf& mcf_;
	Acceptor acceptor_;
	EpollPoller poller_;

	TcpServerCallback connectionCallback_;
	TcpServerCallback messageCallback_;
	TcpServerCallback closeCallback_;
};


}//end of namespace wd

#endif
