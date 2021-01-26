 ///
 /// @file    MyConf.h
 /// @author  gsw(1499186594@qq.com)
 /// @date    2020-06-23 19:50:57
 ///
 
#ifndef __MYCONF_H__
#define __MYCONF_H__

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
using std::map;
using std::string;
using std::ifstream;
using std::istringstream;

namespace wd
{

class MyConf
{
public:
	MyConf(ifstream& ifs)
	{
		std::cout << "MyConf" << std::endl;
		string s1, s2;
		string line;
		while(getline(ifs, line))
		{
			istringstream iss(line);
			iss >> s1 >> s2;
			std::cout << s1 << ' ' << s2 << std::endl;
			conf_[s1] = s2;	
		}
	}

	std::map<std::string, std::string>& show()
	{
		return conf_;
	}

	void print()
	{
		std::cout << conf_["myip"] << std::endl;
		std::cout << conf_["myport"] << std::endl;
	}
private:
	std::map<std::string, std::string> conf_;//地址、端口号、路径信息
};

}//


#endif
