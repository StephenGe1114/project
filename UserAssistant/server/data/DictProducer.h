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
