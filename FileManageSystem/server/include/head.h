#ifndef __HEAD_H__
#define __HEAD_H__ //防止头文件的重复包含
#include <mysql/mysql.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <assert.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>

#define args_check(a, b) { if(a != b) { printf("error args\n");\
	return -1; } }
#define check_error(ret_val, ret, func_name) { if(ret_val == ret) {\
	perror(func_name); return -1; }}

#define check_thread_error(ret, func_name) { if(0 != ret) {\
	printf("%s is faild %d\n", func_name, ret); return -1; }}
#endif
