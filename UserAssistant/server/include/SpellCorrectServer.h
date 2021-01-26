 ///
 /// @file    SpellCorrectServer.h
 /// @author  gsw(1499186594@qq.com)
 /// @date    2020-06-26 09:29:19
 ///
 
#ifndef __SPELLCORRECTSERVER_H__
#define __SPELLCORRECTSERVER_H__

#include "TcpServer.h"
#include "Threadpool.h"
#include "MyTask.h"
#include "MyConf.h"
#include <stdlib.h>

namespace wd
{


class SpellCorrectServer
{
public:
	SpellCorrectServer(MyConf& mcf)
	: conf_(mcf) 
	, threadpool_(4, 10)
	, tcpserver_(mcf)
	{}

	void onConnection(const TcpConnectionPtr& conn)
	{
		cout << conn->toString() << endl;
    	//conn->send("hello, welcome to Chat Server.\r\n");
	}
	void onMessage(const TcpConnectionPtr& conn)
	{
		std::string s(conn->receive());
		cout << s << endl;

		wd::MyTask task(s, conn, conf_);
		//这里bind里面传递的是task对象，不能传递指针，因为这里是在栈中进行的，onMessage结束后，task就会被销毁
		threadpool_.addTask(std::bind(&MyTask::process, task));
		cout << "> add task to threadpool" << endl;
	}
	void onClose(const TcpConnectionPtr& conn)
	{
		printf("%s close\n", conn->toString().c_str());
	}

	void start()
	{
		threadpool_.start();
		tcpserver_.setConnectionCallback(std::bind(&SpellCorrectServer::onConnection, this, std::placeholders::_1));
		tcpserver_.setMessageCallback(std::bind(&SpellCorrectServer::onMessage, this, std::placeholders::_1));
		tcpserver_.setCloseCallback(std::bind(&SpellCorrectServer::onClose, this, std::placeholders::_1));
		tcpserver_.start();
	}
private:
	MyConf& conf_;
	Threadpool threadpool_;
	TcpServer tcpserver_;
};

}//

#endif
