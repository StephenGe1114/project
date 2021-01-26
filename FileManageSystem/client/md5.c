#include "func.h"

int get_md5(char* filename,char* md5) 
{
	MD5_CTX ctx; //定义结构体
	unsigned char outmd[16]; //存储md5码用16字节的字符串
	char buffer[1024];
	int len=0;
	int i=0;
	FILE * fp=NULL;
	memset(outmd,0,sizeof(outmd));
	memset(buffer,0,sizeof(buffer));
	fp=fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("the file not exist in client!!!\n");
		return -1;
	}

	MD5_Init(&ctx); //初始化MD5_CTX结构体
	while((len=fread(buffer,1,1024,fp))>0)
	{
		MD5_Update(&ctx,buffer,len); //不断的hash数据
		memset(buffer,0,sizeof(buffer));
	}
	MD5_Final(outmd,&ctx); //将hash后的数据存入outmd中，并清除ctx中的数据

	for(i=0;i<16;i++)
	{
		sprintf(md5,"%s%02X",md5,outmd[i]); //outmd中是字节存储形式，输出时应以16进制输出
	}
	fclose(fp);
	return 0;
}  
