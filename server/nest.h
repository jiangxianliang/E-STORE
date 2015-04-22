/*
 *author: hdz
 *date: 
 *
 */

#ifndef _LSH_CUCKOO_H_
#define _LSH_CUCKOO_H_

#include "cuckoo.h"
#include "lsh.h"

/*
 * 在每个哈希表中左右扩展的数目， 在Nest中为1
 */

typedef struct NNResult {
	int num;
	void **data;
	void **info;
} NNResult;

typedef CuckooHash Nest; 

//跳过的全零位向量计数
long skip_counter;

//虽然没有用到Nest，不过这可以保证不会在没有初始化配置时调用这个宏
#define NestGetDimension(nest) (global_lsh_param->config->dimension)

#define NestSetFindPosMethod(nest, m) ((nest)->find_opt_pos = (m))
#define NestSetHashMethods(nest, m) ((nest)->functions = (m))

/*
 * 初始化Nest的相关参数
 * @config_file: 配置文件nest.conf
 * 成功则返回正数，失败返回负数
 */
int initNestParam(const char *config_file);

/*
 * 创建哈希表
 * @free_data: 用于哈希表中释放数据内存的函数，如果不指定，需要用户手动释放内存
 * @free_info: 用于哈希表中释放附加信息内存的函数，如果不指定，需要手动释放
 * 成功返回初始化的哈希表，失败返回NULL
 */
HashTable **hashTablesCreate(void (*free_data)(void *ptr), void (*free_info)(void *ptr));

/*
 * 创建nest
 * @hash_tables: 哈希表
 * 成功返回Nest，失败返回NULL
 */
Nest *nestCreate(HashTable **hash_tables);

/*
 * 向nest中插入元素
 * @nest: 指向nest的指针
 * @data: 插入的数据
 * @info: 插入的辅助信息
 * 这里的实现直接调用了cuckoo hashing的插入函数
 * 成功返回插入元素后的Nest，失败返回NULL
 */
Nest *nestInsertItem(Nest *nest, void *data, void *info);

/*
 * 删除Nest中的元素
 * match函数用于进行匹配，由用户提供
 * 成功返回Nest，失败返回NULL
 */
Nest *nestRemoveItem(Nest *nest, void *data, int (*match)(void *data, void *ptr));

/*
 * 将整个文件中的数据插入到Nest中
 * 文件必须每行分别存储要插入的多维向量
 * 这个函数主要用于内部的测试，插入特定格式的数据
 * @nest: 指向Nest的指针
 * @filename: 存储数据的文件
 */
int nestInsertFile(Nest *nest, const char *file_name);

/*
 * 在nest中查找可以插入元素的位置，这个函数将被cuckooInsertItem函数调用以确定可用的位置
 * @nest: 指向Nest的指针
 * @hash_value: 保存可用位置的数组
 * @data: 插入的数据，将传递给哈希函数计算哈希值
 * 成功返回正数，表示可用位置的数目; 失败返回负数，其绝对值为可kick out位置的数目
 * 可用位置的具体信息存储在hash_value数组中
 */
int nestFindOptPos(Nest *nest, HashValue *hash_value, void *data);

/*
 * 打印哈希表中的状态
 */
void nestReport(Nest *nest);

/*
 * 获得与data距离相近的元素
 * @nest: 指向Nest的指针
 * @data: 输入 
 * 返回值为存储结果的结构体, 失败则返回NULL
 */
NNResult *nestGetNN(Nest *nest, void *data);

/*
 * 从Nest中获取单个元素，成功返回元素中的数据，失败返回NULL
 */
void *nestGetItem(Nest *nest, void *data, int (*match)(void *data, void *ptr));

/*
 * 释放Nest参数的内存空间
 */
void freeNestParam();

/*
 * 释放Nest的内存空间
 */
void nestDestroy(Nest *nest);

void freeNNResult(NNResult *res);

#endif
