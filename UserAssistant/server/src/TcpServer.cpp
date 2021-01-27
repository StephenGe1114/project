 
#include "TcpServer.h"
#include "InetAddress.h"
#include "SocketUtil.h"
#include <iostream>
using std::cout;
using std::endl;


namespace wd
{

TcpServer::TcpServer(MyConf& mcf)
: mcf_(mcf)
, acceptor_(createSocketFd(), InetAddress(mcf_.show()["myip"].c_str(), atoi(mcf_.show()["myport"].c_str())))
, poller_(acceptor_)
{
}

void TcpServer::start()
{
	acceptor_.ready();
	poller_.setConnectionCallback(connectionCallback_);
	poller_.setMessageCallback(messageCallback_);
	poller_.setCloseCallback(closeCallback_);
	poller_.loop();
}

void TcpServer::stop()
{
	poller_.unloop();
}
	
void TcpServer::setConnectionCallback(TcpServerCallback cb)
{	connectionCallback_ = cb;	}
void TcpServer::setMessageCallback(TcpServerCallback cb)
{	messageCallback_ = cb;	}
void TcpServer::setCloseCallback(TcpServerCallback cb)
{	closeCallback_ = cb;	}

}//end of namespace wd
