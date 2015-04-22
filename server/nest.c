/*
 *author: hdz
 *date: 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include "lsh.h"
#include "cuckoo.h"
#include "nest.h"

static inline int checkSuffix(const char *framename)
{
        char suffix[3];
        int i = 0;
        int len = strlen(framename);
        for(i = 2; i >= 0; i--) {
                suffix[i] = framename[len-1];
                len--;
        }
        if(memcmp(suffix, "key", 3) == 0)
                return 1;
        else
                return -1;
}

/*
 * 初始化Nest的相关参数
 * @config_file: 配置文件nest.conf
 * 成功则返回正数，失败返回负数
 */
int initNestParam(const char *config_file)
{
	LshParam *param;
	param = initLshParam(config_file);
	if(param == NULL) 
		return -1;

	return 1;
}

/*
 * 创建哈希表
 * @free_data: 用于哈希表中释放数据内存的函数，如果不指定，需要用户手动释放内存
 * @free_info: 用于哈希表中释放附加信息内存的函数，如果不指定，需要手动释放
 * 成功返回初始化的哈希表，失败返回NULL
 */
HashTable **hashTablesCreate(void (*free_data)(void *ptr), void (*free_info)(void *ptr))
{
	HashTable **hash_tables;
	int i = 0, index;	
	int num = global_lsh_param->config->l;
	int size = global_lsh_param->config->size;

	hash_tables = malloc(sizeof(HashTable*)*num);	
	if(NULL == hash_tables) {
		goto error;
	}

	for( i = 0; i < num; i++) {
		index++;
		hash_tables[i] = tableCreate(size);		
		if(NULL == hash_tables[i]) {
			goto error;	
		}
		TableSetFreeDataMethod(hash_tables[i], free_data);
		TableSetFreeInfoMethod(hash_tables[i], free_info);
	}

	goto success;

	error:
		for(i = 0; i < index; i++) {
			tableRelease(hash_tables[i]);
		}		
		fprintf(stderr, "create hash tables failed.\n");
		return NULL;

	success:
		return hash_tables;	
}

/*
 * 创建nest
 * @hash_tables: 哈希表
 * 成功返回Nest，失败返回NULL
 */
Nest *nestCreate(HashTable **hash_tables)
{
	Nest *nest;
	hashfunc_t *hashfunc_list;
	int i = 0;
	int opt_pos;
	int table_num = global_lsh_param->config->l;
	int max_steps = global_lsh_param->config->max_steps;
	int func_num = table_num;
	int nest_offset = global_lsh_param->config->offset;
	

	hashfunc_list = malloc(sizeof(hashfunc_t) * func_num);
	if(NULL == hashfunc_list)  goto error;
	for(i = 0; i < func_num; i++) {
		hashfunc_list[i] = computeLsh;
	}

	opt_pos = func_num * (2 * nest_offset + 1);
	nest = cuckooInit(hash_tables, table_num, hashfunc_list, func_num, max_steps, opt_pos);	
	if(NULL == nest) goto error;	
	CuckooSetFindPosMethod(nest, nestFindOptPos);
	
	goto success;
		
	error:
		fprintf(stderr, "create nest failed.\n");	
		return NULL;

	success:
		return nest;
}


/*
 * 向nest中插入元素
 * @nest: 指向nest的指针
 * @data: 插入的数据
 * @info: 插入的辅助信息
 * 这里的实现直接调用了cuckoo hashing的插入函数
 * 成功返回插入元素后的Nest，失败返回NULL
 */
Nest *nestInsertItem(Nest *nest, void *data, void *info)
{
	return cuckooInsertItem(nest, data, info);
} 

/*
 * 在nest中查找可以插入元素的位置，这个函数将被cuckooInsertItem函数调用以确定可用的位置
 * @nest: 指向Nest的指针
 * @hash_value: 保存可用位置的数组
 * @data: 插入的数据，将传递给哈希函数计算哈希值
 * 成功返回正数，表示可用位置的数目; 失败返回负数，其绝对值为可kick out位置的数目
 * 可用位置的具体信息存储在hash_value数组中
 */
int nestFindOptPos(Nest *nest, HashValue *hash_value, void *data)
{
	int num = 0, end = nest->func_num - 1;
	int start = 0, used_num = 0;
        unsigned long hv;
        HashTable *hash_table;
        Item *item;
        int func_index, table_index;
        int i = 0, j = 0;
	int offset;
	long new_hv;
	int nest_offset = global_lsh_param->config->offset;

        for(i = 0; i < nest->func_num; i++) {
                hash_table = nest->hash_tables[i];
                table_index = i;
		func_index = i;
                hv = nest->functions[i](data, (void*)table_index);
                hv = hv % hash_table->size;
//		printf("hv = %lu\n", hv);
                item = hash_table->items + hv;
                if(item->flag == USED) {
                        //使用过的桶的哈希值也记录下来，后面可以直接使用
                        hash_value[end].hv = hv;
                        hash_value[end].func_index = func_index;
                        hash_value[end].table_index = table_index;
			hash_value[end].access_count = item->access_count;
                        end--;
			used_num++;
                } else {
                        hash_value[num].hv = hv;
                        hash_value[num].func_index = func_index;
                        hash_value[num].table_index = table_index;
			hash_value[num].access_count = item->access_count;
                        num++;
                }
        }
	
	//如果num为0，表示在哈希的位置上没有空位可用的，这时检查左右扩展的位置是否可用
	start = nest->func_num;
	end = nest->opt_pos - 1; 

	if(num == 0) {
		for(i = 0; i < nest->func_num; i++) {
			hv = hash_value[i].hv;
			table_index = hash_value[i].table_index;
			func_index = hash_value[i].func_index;
			hash_table = nest->hash_tables[table_index];
			//向左右扩展
			for(j = 0; j <= nest_offset * 2; j++) {
				offset = nest_offset - j;		
				if(offset == 0) continue;
				//左右扩展后的hv值
				//假定hv不会在signed long中溢出
				new_hv = (signed long)hv + offset;
				if(new_hv < 0 || new_hv >= hash_table->size) continue;					
				
				item = hash_table->items + new_hv;	

				if(item->flag == USED) {
					hash_value[start].hv = new_hv;
					hash_value[start].func_index = func_index;
					hash_value[start].table_index = table_index;  
					hash_value[start].access_count = item->access_count;
					start++;
					used_num++;
				} else {
					hash_value[end].hv = new_hv;
					hash_value[end].func_index = func_index;
					hash_value[end].table_index = table_index;
					hash_value[end].access_count = item->access_count;
					end--;
					num++;
				}
			}
		}	

		//仍然没有找到空位
		if(num == 0) return -used_num;	

		//找到空位, 将空位转移到hash_value的前面
		if(num > 0) {
			start = nest->opt_pos - num;
			for(i = 0; i < num; i++) {
				hash_value[i].hv = hash_value[start].hv;
				hash_value[i].func_index = hash_value[start].func_index;
				hash_value[i].table_index = hash_value[start].table_index;	
				hash_value[i].access_count = hash_value[start].access_count; 
				start++;
			}	
		}
	}

        return num;
}

/*
 * 将整个文件中的数据插入到Nest中
 * 文件必须每行分别存储要插入的多维向量
 * 这个函数主要用于内部的测试，插入特定格式的数据
 * @nest: 指向Nest的指针
 * @filename: 存储数据的文件
 */
int nestInsertFile(Nest *nest, const char *filename)
{
	FILE *fd;
        int counter = 0;
	int dimension;
	float *points = NULL, f;
	char *framename, *tmp, buf[1024];
	int i = 0;
	int zero_counter = 0;
	int len;	
	
	dimension = global_lsh_param->config->dimension;

        if((fd = fopen(filename, "r")) == NULL) {
                fprintf(stderr, "can't open file %s\n", filename);
                return -1;
        }

	points = malloc(sizeof(float)*dimension);
	if(points == NULL) return 0;
	framename = malloc(30); //magic number, just for test
	if(framename == NULL) {
		free(points);
		return 0;	
	}
	tmp = malloc(dimension + 1);
	if(tmp == NULL) {
		free(points);
		free(framename);
		return 0;
	}

        #ifdef _EVALUATION_
	struct timeval start, end;	
	gettimeofday(&start, NULL);
        insert_items_time = 0.0;
        #endif

	skip_counter = 0;

        while(fgets(framename, 30, fd) != NULL) {
		len = strlen(framename);	
		if(framename[len - 1] == '\n') 
			framename[len - 1] = '\0';

//		fgets(tmp, dimension + 2, fd);
//		if(tmp == NULL) break;

		if(NULL == points) {
			points = malloc(sizeof(float)*dimension);
			if(NULL == points) {
				fprintf(stderr, "nestInsertFile: allocate memory failed.\n");
				return counter;
			}
		}	

		for(i = 0; i < dimension; ++i)
			fscanf(fd, "%f", &points[i]);
		fgets(buf, 1024, fd);              //读取数据行剩余数据
/*
		zero_counter = 0;
		for(i = 0; i < dimension; i++) {
			points[i] = (float)(tmp[i] - '0');
			if((int)points[i] == 0) zero_counter++;
		}
		//跳过输入位向量全零的情况
		if(zero_counter == dimension) {
			skip_counter++;
		 	continue;
		}
*/

		if(NULL == nestInsertItem(nest, points, framename)) {
			break;
		}

		points = NULL;

		framename = malloc(30);
		if(framename == NULL) return counter;
		
		counter++;	
        }
//	if(framename != NULL) free(framename);

        #ifdef _EVALUATION_
        gettimeofday(&end, NULL);
        insert_file_time = 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
        #endif

	free(tmp);
        fclose(fd);
        return counter;

}

/*
 * 打印哈希表中的状态
 */
void nestReport(Nest *nest)
{
//	printf("total %ld data skiped.\n", skip_counter);
	cuckooReport(nest);
}

/*
 * 获得与data距离相近的元素
 * @nest: 指向Nest的指针
 * @data: 输入 
 * 返回值为存储结果的结构体, 失败则返回NULL
 */	
NNResult *nestGetNN(Nest *nest, void *data)
{
	NNResult *result;
	void **data_list, **info_list;
	HashTable **tables = nest->hash_tables;		
	HashTable *table;
	Item *items, *item;
	int table_num = nest->table_num;		
	int func_num = nest->func_num;
	int table_size;
	int i = 0, j = 0,  num = 0;
	int table_index;
	int offset;
	unsigned long hv;
	long new_hv;
	int nest_offset = global_lsh_param->config->offset;
	int opt_pos = func_num * (2 * nest_offset + 1);

	result = malloc(sizeof(NNResult));
	if(NULL == result) {
		return NULL;
	}	
	data_list = malloc(sizeof(void*)*opt_pos);	
	if(NULL == data_list) {
		free(result);
		return NULL;
	}
	info_list = malloc(sizeof(void*)*opt_pos);
	if(NULL == info_list) {
		free(result);
		free(data_list);
		return NULL;
	}
	
/*
	for(i = 0; i < table_num; i++) {
		table = tables[i];
		items = table->items;
		for(j = 0; j < table->size; j++) {
			if(items[j].data != NULL) {
				if(checkSuffix((char*)items[j].info) < 0)
				{
					printf("NestGetNN: framename = %s\n", (char*)items[j].info);
				}
			}	
		} 
	}
*/

	for(i = 0; i < table_num; i++) {
		table = tables[i];	
		table_index = i;
		table_size = table->size;
		items = table->items;
		hv = nest->functions[i](data, (void*)table_index); 
		hv = hv % table_size;

		for(j = 0; j <= nest_offset*2; j++) {
			offset = nest_offset - j; 
			new_hv = (signed long)hv + offset;	
			if(new_hv < 0 || new_hv >= table_size) continue;
			item = items + new_hv;
			if(item->flag == USED) {
				data_list[num] = item->data;	
				info_list[num] = item->info;
			//	printf("info = %s\n", (char*)item->info);
				num++;
			}		
		}	
	}
	result->num = num;
	result->data = data_list;	
	result->info = info_list;
			
	return result;	
}

void freeNNResult(NNResult *res)
{
	if(res != NULL) {
		if(res->data != NULL) free(res->data);
		if(res->info != NULL) free(res->info);
		free(res);
	}
}


/*
 * 释放Nest的内存空间
 */
void nestDestroy(Nest *nest)
{
	assert(nest != NULL);
	cuckooDestroy(nest);
}

/*
 * 释放Nest参数的内存空间
 */
void freeNestParam()
{
	freeLshParam(global_lsh_param);			
}

/*
 * 删除Nest中的元素
 * match函数用于进行匹配，由用户提供
 * 成功返回Nest，失败返回NULL
 */
Nest *nestRemoveItem(Nest *nest, void *data, int (*match)(void *data, void *ptr)) 
{
	HashTable **tables = nest->hash_tables;		
	HashTable *table;
	Item *items, *item;
	int table_num = nest->table_num;		
	int func_num = nest->func_num;
	int table_size;
	int nest_offset = global_lsh_param->config->offset;
	int opt_pos = func_num * (2 * nest_offset + 1);
	int i = 0, j = 0,  num = 0;
	int table_index;
	int offset;
	unsigned long hv;
	long new_hv;

	for(i = 0; i < table_num; i++) {
		table = tables[i];	
		table_index = i;
		table_size = table->size;
		items = table->items;
		hv = nest->functions[i](data, (void*)table_index); 
		hv = hv % table_size;

		for(j = 0; j <= nest_offset*2; j++) {
			offset = nest_offset - j; 
			new_hv = (signed long)hv + offset;	
			if(new_hv < 0 || new_hv >= table_size) continue;
			item = items + new_hv;
			if(item->flag == USED) {
				if(NULL == match) return NULL;
				if(match(data, item->data) > 0) 
				{
					tableRemove(table, new_hv);
				}
				return nest;
			}		
		}	
	}
				
	return NULL;
}


/*
 * 从Nest中获取单个元素，成功返回元素中的数据，失败返回NULL
 */
void *nestGetItem(Nest *nest, void *data, int (*match)(void *data, void *ptr))
{
	HashTable **tables = nest->hash_tables;		
	HashTable *table;
	Item *items, *item;
	int table_num = nest->table_num;		
	int func_num = nest->func_num;
	int table_size;
	int nest_offset = global_lsh_param->config->offset;
	int opt_pos = func_num * (2 * nest_offset + 1);
	int i = 0, j = 0,  num = 0;
	int table_index;
	int offset;
	unsigned long hv;
	long new_hv;

	for(i = 0; i < table_num; i++) {
		table = tables[i];	
		table_index = i;
		table_size = table->size;
		items = table->items;
		hv = nest->functions[i](data, (void*)table_index); 
		hv = hv % table_size;

		for(j = 0; j <= nest_offset*2; j++) {
			offset = nest_offset - j; 
			new_hv = (signed long)hv + offset;	
			if(new_hv < 0 || new_hv >= table_size) continue;
			item = items + new_hv;
			if(item->flag == USED) {
				if(NULL == match) return NULL;
				if(match(data, item->data) > 0) 
				{
					return item->data;
				}
			}		
		}	
	}
	return NULL;	
}
