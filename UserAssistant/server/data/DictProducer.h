 ///
 /// @file    DictProducer.h
 /// @author  gsw(1499186594@qq.com)
 /// @date    2020-06-23 14:39:18
 ///
 
#ifndef __DICTPRODUCER_H__
#define __DICTPRODUCER_H__

#include <string>
#include <map>
using std::string;
using std::map;

namespace wd
{

class DictProducer
{
public:
	DictProducer(const string&);
	
	void build_dict();

	void store_dict(const string&);
private:
	string pre_process(string&);
private:
	string dir_;
	map<string, int> dict_;
};

}//

#endif
