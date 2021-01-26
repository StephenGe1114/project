#include "func.h"

int insert_progress(int userid, char* filename, int offset, int down_size, int port)
{
	printf("insert_progress\n");
	MYSQL *conn;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[256]="insert into progress(PORT, ID, FILENAME, OFFSET, DOWN_SIZE) values(";
	sprintf(query, "%s%d%s%d%s%s%s%d%s%d%s", query, port, ", ", userid, ", '", filename, "' ,", offset, ", ", down_size, ")");  
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	mysql_close(conn);
	return 0;
}

int query_offset(int port, int userid, char* filename, int* offset, int* down_size)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="select OFFSET, DOWN_SIZE from progress where FILENAME='";
	sprintf(query,"%s%s%s%d%s%d",query,filename,"' and ID=",userid, " and PORT = ", port);
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
			while((row=mysql_fetch_row(res))!=NULL)
			{
				*offset=atoi(row[0]);
				*down_size=atoi(row[1]);
			}
		}
		mysql_free_result(res);
		mysql_close(conn);
		return 0;
	}
	return 0;
}

int update_offset(int port, int userid, char* filename, int offset, int down_size)
{
	MYSQL *conn;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="update progress set OFFSET = ";
	sprintf(query,"%s%d%s%d%s%d%s%d%s%s%s",query, offset, ", DOWN_SIZE = ", down_size, " where ID=",userid, " and PORT = ", port, " and FILENAME = '", filename, "'");
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

int is_exist(int userid, char* filename, int port)
{

	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="test";
	char query[512]="select * from progress where FILENAME='";
	sprintf(query,"%s%s%s%d%s%d",query,filename,"' and ID=",userid, " and PORT = ", port);
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
			if((row=mysql_fetch_row(res))!=NULL)
				return 1;
		}
		mysql_free_result(res);
		mysql_close(conn);
	}
	return 0;
}
