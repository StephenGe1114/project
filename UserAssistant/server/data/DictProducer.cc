 ///
 /// @file    DictProducer.cc
 /// @author  gsw(1499186594@qq.com)
 /// @date    2020-06-23 14:51:14
 ///
 
#include "DictProducer.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
using std::ifstream;
using std::ofstream;
using std::vector;
using std::endl;

namespace wd
{

DictProducer::DictProducer(const string& dir)
: dir_(dir)
, dict_()
{}

void DictProducer::build_dict()
{
	ifstream ifs(dir_);	
	string line;
	while(std::getline(ifs, line))
	{
		replace_if(line.begin(), line.end(), [](char c){
					if(!isalpha(c))
						return true;
					else return false;
				}, ' ');
		std::istringstream iss(line);
		string word;
		while(iss >> word)
		{
			word = pre_process(word);
			++dict_[word];
		}
	}
}

void DictProducer::store_dict(const string& filename)
{
	ofstream ofs(filename);
	for(auto mem: dict_)
		ofs << mem.first << ' ' << mem.second << endl;
}

string DictProducer::pre_process(string& word)
{
	string tmp;
	for(auto mem: word)
		tmp += tolower(mem);
	return tmp;
}

}//
