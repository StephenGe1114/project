#include "factory.h"

int query_users(int* userid, char* name, char* salt, char* passwd, int* path)
	//根据用户姓名查询用户信息
{
	MYSQL *conn;
	MYSQL_RES *res=0; //指向结果集
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* passwd1="123";
	const char* database="test";
	char query[512]="select ID,SALT,PASSWORD,PATH from users where NAME='";
	sprintf(query,"%s%s%s",query,name,"'");
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,passwd1,database,0,NULL,0)) //连接到mysql数据库
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res); //释放结果集使用的内存
		mysql_close(conn); //关闭服务器连接
		return -1;
	}
	t=mysql_query(conn,query); //执行sql语句
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn); //启动逐行结果集检索
		if(res)
		{
			if((row=mysql_fetch_row(res)) != NULL) //从结果集中获取下一行
			{
				*userid=atoi(row[0]);
				strcpy(salt,row[1]);
				strcpy(passwd,row[2]);
				*path=atoi(row[3]);
			}
			else{
				return -1;
			}
		}	
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_file(int path, int userid, char* buf) //查当前目录下的文件
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="select FILENAME,TYPE,SIZE from file where PROCODE=";
	sprintf(query,"%s%d%s%d",query,path," and ID=",userid);
	sprintf(buf,"%-14s%-14s%s","[FILENAME]","[TYPE]","[SIZE]\n");
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))==NULL){
				sprintf(buf,"%s%-14s%-14s%-14s",buf,"NULL","NULL","NULL");
				return 0;
			}
			do
			{
				for(int i=0;i<mysql_num_fields(res);i++) //返回结果集中的列数
				{
					sprintf(buf,"%s%-14s",buf,row[i]);
				}
				sprintf(buf,"%s%s",buf,"\n");
			}while((row=mysql_fetch_row(res))!=NULL);
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_pwd(int path, char* buf) //查询当前目录
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char tmp[1024] = {0};
	char tmp1[1024] = {0};
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	while(path!=0){
		char query[512]="select PROCODE,FILENAME from file where CODE=";
		sprintf(query,"%s%d",query,path);
		mysql_query(conn,query);
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{
				path=atoi(row[0]);
				sprintf(tmp1, "%s%s%s", "/", row[1], tmp);
				strcpy(tmp, tmp1);
			}

		}
	}
	sprintf(buf, "%s%s", "/netdisc", tmp1);
	mysql_free_result(res);
	mysql_close(conn);
	return 0;
}

int query_procode(int* procode, int code)
//查询上一级目录
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* passwd="123";
	const char* database="test";
	char query[512]="select PROCODE from file where CODE=";
	sprintf(query,"%s%d",query,code);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else
	{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))!=NULL)
			{
				*procode=atoi(row[0]);	
			}
			else
			{
				return -1;
			}
		}	
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_dir(int path, char* cmd2, int userid, int* ppath)
	//查询userid用户path目录下的cmd2目录是否存在, 若存在将其code存入path中
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[200]="select CODE from file where PROCODE=";
	sprintf(query,"%s%d%s%d%s%s%s",query,path," and ID= ",userid, " and FILENAME = '", cmd2, "' and TYPE = 'd'");
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res) //当查询没有出问题时，res值不为空，当查询结果为空时，res不为空，但row为空
		{
					
			if((row=mysql_fetch_row(res))!=NULL)
			{
				*ppath = atoi(row[0]);
			} else
				return -1;
		} 
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int  is_file(int path, int userid, char* cmd, int* code)
// 判断cmd是否是文件
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[200]="select	TYPE, CODE from file where PROCODE=";
	char buf[10] = {0};
	sprintf(query,"%s%d%s%d%s%s%s",query, path, " and ID = ", userid, " and FILENAME = '", cmd, "'");
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res) //当查询没有出问题时，res值不为空，当查询结果为空时，res不为空，但row为空
		{
					
			if((row=mysql_fetch_row(res))!=NULL)
			{
				strcpy(buf, row[0]);
				*code = atoi(row[1]);
			} 
		} 
		mysql_free_result(res);
	}
	mysql_close(conn);
	if(!strcmp(buf, "f"))
		return 1;
	else
		return 0;
}

int insert_users(char* name, char* salt, char* password, int path)
{
	MYSQL *conn;
	const char* server="localhost";
	const char* user="root";
	const char* passwd="123";
	const char* database="test";
	char query[200]="insert into users(NAME,SALT,PASSWORD,PATH) values('";
	sprintf(query,"%s%s%s%s%s%s%s%d%s",query,name,"','",salt,"','",password,"',",path,")");
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,passwd,database,0,NULL,0);
	mysql_query(conn,query);
	mysql_close(conn);
	return 0;
}

int change_path(int userid, int path)
{
	MYSQL *conn;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="update users set PATH=";
	sprintf(query,"%s%d%s%d",query,path," where ID=",userid);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	mysql_close(conn);
	return 0;
}

int insert_dir(int path, int userid, char* name)
{
	MYSQL *conn;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[200]="insert into file(PROCODE,ID,FILENAME,TYPE,SIZE) values(";
	sprintf(query,"%s%d%s%d%s%s%s",query,path,",",userid,",'",name,"','d',8)");
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	mysql_close(conn);
	return 0;
}

int delete_code(int path)
{
	MYSQL *conn;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[200]="delete from file where CODE=";
	sprintf(query,"%s%d",query,path);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	mysql_close(conn);
	return 0;
}

int recus_dele(int code)
//递归删除code目录子文件
{
	MYSQL *conn;
	MYSQL_RES *res=0; //指向结果集
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[200]="select CODE TYPE from file where PROCODE=";
	sprintf(query,"%s%d",query, code);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)) //连接到mysql数据库
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res); //释放结果集使用的内存
		mysql_close(conn); //关闭服务器连接
		return -1;
	}
	t=mysql_query(conn,query); //执行sql语句
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn); //启动逐行结果集检索
		if(res)
		{
			while((row=mysql_fetch_row(res)) != NULL) //从结果集中获取下一行
			{
				if(!strcmp(row[1], "f"))
					delete_code(atoi(row[0]));
				else 
				{
					recus_dele(atoi(row[0]));
					delete_code(atoi(row[0]));
				}
			}
		}	
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_md5(char* md5)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	int num=0;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="select * from file where MD5 = '";
	sprintf(query,"%s%s%s",query,md5,"'");
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	res=mysql_use_result(conn);
	if(res)
	{
		if((row=mysql_fetch_row(res))==NULL)
		{
			return -1;
		}
		num++;
	}	
	mysql_free_result(res);
	mysql_close(conn);
	return num;
}

int insert_file(int path, int userid, char* name, char* md5, int size)
{
	MYSQL *conn;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[256]="insert into file(PROCODE,ID,FILENAME,TYPE,MD5,SIZE) values(";
	sprintf(query,"%s%d%s%d%s%s%s%s%s%d%s",query,path,",",userid,",'",name,"','f','",md5,"',",size,")");
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	mysql_close(conn);
	return 0;
}

int query_file_size(char* md5, int* size)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="select SIZE from file where MD5='";
	sprintf(query,"%s%s%s",query,md5,"'");
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))!=NULL){
				*size = atoi(row[0]);
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int is_in_curdir(int userid, int path, char* md5)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="select * from file where MD5 = '";
	sprintf(query,"%s%s%s%s%d%s%d",query,md5,"'", "and ID = ", userid, " and PROCODE = ", path);
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	res=mysql_use_result(conn);
	if(res)
	{
		if((row=mysql_fetch_row(res))==NULL)
		{
			return 0;
		}
	}	
	mysql_free_result(res);
	mysql_close(conn);
	return 1;
}

int query_file_size2(int path, int userid, char* name, int* size) //查询给定目录和用户和文件名的文件的大小
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="select SIZE from file where FILENAME ='";
	sprintf(query,"%s%s%s%d%s%d",query,name,"' and PROCODE = ", path, " and ID = ", userid);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))!=NULL){
				*size = atoi(row[0]);
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}
