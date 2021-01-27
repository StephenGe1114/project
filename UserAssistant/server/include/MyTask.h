 
#ifndef __MYTASK_H__
#define __MYTASK_H__

#include "TcpConnection.h"
#include "Dictionary.h"
#include "MyConf.h"
#include "json/json.h"
#include <string.h>
#include <string>
#include <iostream>
#include <queue>
#define INF 99999999
using std::string;
using std::cout;
using std::endl;

namespace wd
{

struct MyResult
{
	string candidate_word;
	int frequency;
	int distance;
};

bool operator>(const MyResult& lhs, const MyResult& rhs)
{
	if(lhs.distance > rhs.distance)
		return true;
	else if(lhs.distance == rhs.distance)
	{
		if(lhs.frequency < rhs.frequency)
			return true;
		else return false;
	} else return false;
}

class MyTask
{
public:
	MyTask(const string & query, const wd::TcpConnectionPtr & conn, MyConf& mcf)
	: mcf_(mcf)
	, _queury(query)
	, _conn(conn)
	{}
	
	void process()
	{

		cout << "> task is processing" << endl;
		//_conn->send(_queury);//如果直接去使用send，在计算线程里面完成了数据发送,不可取
		cout << _queury << endl;
		
		Dictionary* pdict = Singleton<Dictionary>::getInstance(mcf_);
		set<int> s;
		auto at1 = pdict->showDict();
		auto at2 = pdict->showIndex();
		for(auto mem: _queury)
		{
//			cout << mem << endl;
			auto a = at2[string(1, mem)];	
			s.insert(a.begin(), a.end());
		}
	//	for(auto mem: s)
	//		cout << mem << ' ';
		cout << endl;
		for(auto it = s.begin(); it != s.end(); ++it)
		{
			MyResult mr;
			mr.candidate_word = at1[*it].first;
			mr.frequency = at1[*it].second;
			mr.distance = calcDistance(mr.candidate_word, _queury);
			//cout << mr.candidate_word << ' ' << mr.frequency << ' ' << mr.distance << endl;
			if(mr.distance <= 3)
				que_.push(mr);
		}

		vector<string> ves;
		int n = que_.size() > 5 ? 5 : que_.size();
		for(int i = 0; i < n; ++i)
		{
			ves.push_back(que_.top().candidate_word);
			que_.pop();
		}

		string line;
		for(size_t i = 0; i < ves.size(); ++i)
		{
			line.append(ves[i]);
			line.append(1, ' ');
		}

		//Json::Value root;
		//Json::FastWriter writer;
		//root[_queury] = line;
		//line  = writer.write(root);
		cout << line << endl;

		_conn->sendInLoop(line);//数据的接收和发送应该交给IO线程（即reactor线程中），这里将发送操作放入vector<Functor>容器中，通过eventfd使其得到执行

	}

private:
	int min(int a, int b)
	{
		return a < b ? a : b;
	}

	int calcDistance(std::string lhs, std::string rhs)
		//求最小编辑距离算法（距离小于等于3）
	{
		int dp[1024][1024] = {{0}};
		lhs.insert(0, 1, '*');
		rhs.insert(0, 1, '*');
		int len1 = lhs.size() - 1;
		int len2 = rhs.size() - 1;

		for(int i = 1; i <= len1; ++i)
			for(int j = 1; j <= len2; ++j)
				dp[i][j] = INF;
		for(int i = 1; i <= len1; ++i)
			dp[i][0] = i;
		for(int j = 1; j <= len2; ++j)
			dp[0][j] = j;
		for(int i = 1; i <= len1; ++i)
		{
			for(int j = 1; j <= len2; ++j)
			{
				int flag;
				flag = lhs[i] == rhs[j] ? 0 : 1;
				dp[i][j] = min(dp[i-1][j]+1, min(dp[i][j-1]+1, dp[i-1][j-1]+flag));
			}
		}
		return dp[len1][len2];
	}

private:
	MyConf& mcf_;
	string _queury;
	wd::TcpConnectionPtr _conn;
	std::priority_queue<MyResult, vector<MyResult>, std::greater<MyResult>> que_;
};

}//

#endif
