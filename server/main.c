/*
 * the main function of server
 * author: zhenhua
 * date: 2015.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nest.h"
#include "similarity.h"
#include "net_server.h"

char *path_name;

/*
 * 载入服务器端配置文件
 * pathname 是数据集的路径名。
 */
int loadMainConfigFile(const char *filename, char *path_name)
{
	FILE *fp;
	char buf[1024];
	char *str;
	int len, n;

	if(filename == NULL)
		return -1;

	if(NULL == (fp = fopen(filename, "r")))
	{
		fprintf(stderr, "fopen main config file error\n");
		return -1;
	}
	
	memset(buf, '\0', 1024);
	while(NULL != fgets(buf, 1024, fp))
	{
		if(buf[0] == '#' || buf[0] == '\n')
			continue;

		len = strlen(buf);
		buf[len-1] = '\0';
		
		str = strchr(buf, '=');
		*str++ = '\0';
		if(strcmp("pathname", buf) == 0)
		{
			n = len - strlen(buf);
			memcpy(path_name, str, n);
			continue;
		}
		memset(buf, '\0', 1024);
	}
	fclose(fp);

	return 0;
}


/*
 * 服务器端的入口函数
 * 1. 载入配置文件
 * 2. 构建索引结构
 * 3. 创建网络监听端口
 */
int main(int argc, char *argv[])
{
	char *trace_files, *search_file, *config_file, *main_config_file;
	int dimension;
        FILE *fp, *trace_fp;
	HashTable **hash_tables;
	Nest *nest;
	NNResult *res;
	ImageGroups *iGroups;
	char trace_filename[1024], fullpath_filename[PATH_NAME_LENGTH];
        int counter = 0;
	int ret;

	if(argc != 1) {
		printf("Usage: %s\n", argv[0]); 
		return -1;
	}

	//input arguments
	trace_files = "./trace/trace_namelist.txt";

	trace_fp = fopen(trace_files, "r");
	if(trace_fp == NULL) {
		fprintf(stderr, "can't open file : %s\n", trace_files);
		return -1;
	}

	//default
	config_file = "nest.conf";
	main_config_file = "main.conf";

	if(initNestParam(config_file) < 0) {
		fprintf(stderr, "init nest param failed.\n");
		return -1;
	}
	
	path_name = (char *)malloc(NET_NAME_LENGTH * sizeof(char));
	if(path_name == NULL)
	{
		fprintf(stderr, "main: path_name malloc error\n");
		return -1;
	}

	//载入配置文件
	memset(path_name, '\0', NET_NAME_LENGTH);
	ret = loadMainConfigFile(main_config_file, path_name);
	if(ret == -1)
	{
		fprintf(stderr, "loadMainConfigFile error\n");
		return -1;
	}
#ifdef  _DEBUG
	printf("path_name: %s\n", path_name);
#endif

	hash_tables = hashTablesCreate(free, free);		
	if(hash_tables == NULL) {
		fprintf(stderr, "create hash tables failed.\n");
		return -1;
	}
	
	//创建nest结构
	nest = nestCreate(hash_tables);
	if(nest == NULL) {
		fprintf(stderr, "create nest failed.\n");
		return -1;
	}

	dimension = NestGetDimension();	

	memset(trace_filename, '\0', 1024);
	
	while(fgets(trace_filename, 1024, trace_fp) != NULL) {
		
		if(trace_filename[strlen(trace_filename) - 1] == '\n')
			trace_filename[strlen(trace_filename) - 1] = '\0';	
		
		memset(fullpath_filename, '\0', PATH_NAME_LENGTH);
		sprintf(fullpath_filename, "%s/%s", path_name, trace_filename);
	
		//将文件中的数据插入Nest
		if(nestInsertFile(nest, fullpath_filename) < 0) break;	

#ifdef 	_DEBUG
	//	printf("Insert %s succeed!\n", trace_filename);
		fprintf(stderr, "Insert %s succeed!\n", trace_filename);
#endif
		counter++;
	}

	printf("Total %d files have been inserted.\n", counter);

	//打印哈希表状态	
	printf("\nNest status:\n");
	nestReport(nest);

#ifdef _DEBUG
	printf("main.c: createServer is starting \n");
	fprintf(stderr, "main.c: createServer is starting \n");
#endif
	
	//创建监听端口(net_server.h)
	ret = createServer(nest, dimension);
	if(ret == -1)
	{
		fprintf(stderr, "createServer failed\n");
		return -1;
	}
	

	return 0;
}
