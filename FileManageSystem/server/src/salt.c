#include "factory.h"

void get_rand_str(char* s, int num)
{
	const char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;\"'<>?";
	int i, len, rand_num;
	len = strlen(str);
	srand((unsigned int)time(NULL));
	for(i = 0; i < num; ++i)
	{
		rand_num = rand() % len;
		*s++ = str[rand_num];
	}
	*s = '\0';

}

void get_salt(char* s)
{
	get_rand_str(s, 8);
}
