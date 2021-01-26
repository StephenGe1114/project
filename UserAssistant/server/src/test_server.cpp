
#include "SpellCorrectServer.h"
using namespace wd;

int main(int argc, char const *argv[])
{
	ifstream ifs("/home/jack/project/uia/server/conf/my.conf");
	
	MyConf mcf(ifs);
	SpellCorrectServer s(mcf);
	s.start();

    return 0;
}
