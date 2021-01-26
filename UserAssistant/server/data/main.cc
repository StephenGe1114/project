 ///
 /// @file    main.cc
 /// @author  gsw(1499186594@qq.com)
 /// @date    2020-06-23 16:05:48
 ///
 
#include "DictProducer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <set>
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::pair;
using std::map;
using std::set;
using namespace wd;
 
int main(int argc, char **argv)
{
	if(4 != argc)
	{
		cout << "argc error!" << endl;
		return -1;
	}
	DictProducer d(argv[1]);
	d.build_dict();
	d.store_dict(argv[2]);

	ifstream ifs(argv[2]);
	string line;
	vector<pair<string, int>> svec;
	int num;
	string word;
	while(getline(ifs, line))
	{
		std::istringstream iss(line);
		iss >> word >> num;
		svec.push_back(make_pair(word, num));
	}

	map<string, set<int>> m;
	for(int i = 0; i < 26; ++i)
	{
		string s(1, 'a' + i);
		m[s];
	}

	for(size_t i = 0; i != svec.size(); ++i)
	{
		for(auto mem: svec[i].first)
		{
			string s(1, mem);
			m[s].insert(i);	
		}
	}

	ofstream ofs(argv[3]);
	for(auto mem: m)
	{
		ofs << mem.first << ' ';
		for(auto num: mem.second)
			ofs << num << ' ';
		ofs << endl;
	}


	return 0;
}
