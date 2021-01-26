#include "factory.h"

void sigfunc(int signum)
{
	printf("%d is coming\n", signum);
}

int tran_file(int new_fd)
{
	train t;
	signal(SIGPIPE, sigfunc);
	//先发文件名
	t.len = strlen(FILENAME);
	strcpy(t.buf, FILENAME);
	int ret;
	ret = send_n(new_fd, (char*)&t, 4 + t.len); //这里最后的参数不能是sizeof(t),而一定是4+t.len,4是控制数据len的大小，t.len是buf中的内容大小
	if(-1 == ret)
	{
		goto end;
	}
	//发文件大小
	int fd = open(FILENAME, O_RDONLY);
	check_error(-1, fd, "open");
	struct stat buf;
	fstat(fd, &buf); 
	t.len = sizeof(buf.st_size);
	memcpy(t.buf, &buf.st_size, sizeof(off_t));
	ret = send_n(new_fd, (char*)&t, 4 + t.len);
	if(-1 == ret)
	{
		goto end;
	}
	//发文件内容
	while((t.len = read(fd, t.buf, sizeof(t.buf))) > 0)
	{
		ret = send_n(new_fd, (char*)&t, 4 + t.len);
		if(-1 == ret)
		{
			goto end;
		}
	}
	ret = send_n(new_fd, (char*)&t, 4 + t.len); //结束
	if(-1 == ret)
	{
		goto end;
	}
end:
	close(new_fd);
	return 0;
}

