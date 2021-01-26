 ///
 /// @file    Dictionary.h
 /// @author  gsw(1499186594@qq.com)
 /// @date    2020-06-23 20:17:07
 ///
 
#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <fstream>
#include <sstream>
#include "MyConf.h"
using std::vector;
using std::map;
using std::string;
using std::set;
using std::pair;
using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;

namespace wd
{

class Dictionary
{
public:
	Dictionary(MyConf& mcf)
	: mcf_(mcf)
	{
		cout << "Dictionary" << endl;
		const char *path1 = mcf_.show()["mydict"].c_str();
		const char *path2 = mcf_.show()["myindex"].c_str();
		cout << path1 << endl;
		cout << path2 << endl;
		ifstream ifs1(path1);
		if(!ifs1)
		{
			cout << "ifs1 open error" << endl;
			return;
		}
		ifstream ifs2(path2);
		if(!ifs2)
		{
			cout << "ifs2 open error" << endl;
			return;
		}
		string line;
		while(getline(ifs1, line))
		{
			istringstream iss(line);
			string word;
			int num;
			iss >> word >> num;
			dict_.push_back(make_pair(word, num));
			//cout << word << ' ' << num << endl;
		}

			cout << "ifs1"  << endl;
		while(getline(ifs2, line))
		{
			istringstream iss(line);
			string word;
			int num;
			iss >> word;

			while(iss >> num)
			{
				indexTable_[word].insert(num);
				
			//cout << word << ' ' << num << endl;
			}
		}
			cout << "ifs2"  << endl;
	}

	map<string, set<int>>& showIndex() { return indexTable_; }
	vector<pair<string, int>>& showDict() { return dict_; }
private:
	MyConf& mcf_;
	vector<pair<string, int>> dict_;
	map<string, set<int>> indexTable_;
};

template <typename T>
class Singleton
{
	class AutoRelease
	{
	public:
		AutoRelease() { cout << "AutoRelease()" << endl; }
		~AutoRelease() {
			if(_pInstance){
				delete _pInstance;
			}
			cout << "~AutoRelease()" << endl;
		}
	};

public:
	template <typename... Args>
	static T * getInstance(Args... args)
	{
		if(_pInstance == NULL)
		{
			_autoRelease;//露脸，否则无法调用其构造函数和析构函数
			_pInstance = new T(args...);
		}
		return _pInstance;
	}

private:
	Singleton() { cout << "Singleton()" << endl; }
	~Singleton() { cout << "~Singleton()" << endl; }
private:
	static T * _pInstance;	
	static AutoRelease _autoRelease;
};

template <typename T>
T * Singleton<T>::_pInstance = NULL;

template <typename T>
class Singleton<T>::AutoRelease Singleton<T>::_autoRelease;

}//

#endif
