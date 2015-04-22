/*
 *author: hdz
 *date: 2013.5.8
 *
 */

#ifndef _H_HASH
#define _H_HASH

/*
 * 用于保存lsh和nest配置文件的参数
 * 具体含义参见nest.conf文件
 */

#define FACTOR 1.425

#define OPTIMIZE_ON 1
#define OPTIMIZE_OFF 0

typedef enum LSH_TYPE {E2LSH, HAMMING} LSH_TYPE;


typedef struct LshConfig 
{ 
	float r;
	float w;
	int k;
	int l;
	int size;
	int dimension;
	int offset;
	int max_steps;
	float p;
	LSH_TYPE lsh_type;
	int optimize_kickout;

} LshConfig ;

/*
 * 用于lsh计算的函数族
 * 计算公式: (av + b) / w
 * 结构体中的变量a,b与公式中的相对应, c用于最后计算出哈希值
 */
typedef struct HashFamily
{
	float **a;
	float *b;
	unsigned int *c;

} HashFamily;

/*
 * 用于LSH的参数，集成了配置文件中的设置与函数族
 */
typedef struct LshParam {
		
	LshConfig *config;	
	HashFamily *hash_family;
	double *buf; //保存中间结果的缓冲区
	
} LshParam;

/*
 * 均匀分配的多维向量，用于LshConfig中的c变量
 */
typedef struct UniformHashFunction
{
	unsigned int *u;	

} UniformHashFunction;

extern LshParam *global_lsh_param;

/*
 * 初始化LSH参数
 * @config_file: 配置文件名
 * 成功返回初始化的配置， 失败返回NULL
 */ 
LshParam *initLshParam(const char *config_file);

/*
 * 读取配置文件
 * @filename: 配置文件名
 * @conf: 保存配置的结构体指针
 * 失败返回-1, 成功返回0
 */
int loadConfigFile(const char* filename, LshConfig *conf);

/*
 * 初始化LSH哈希函数族
 * @conf: lsh配置信息
 * @hash_family: 已经分配好空间的哈希函数族 
 * @uhf: 均匀分配多维向量
 * @num: 哈希函数族的数目
 * 成功返回正数，失败返回负数
 */
int initHashFamily(LshConfig *conf, HashFamily *hash_family, UniformHashFunction *uhf,  int num);

/*
 * 初始化均匀分布多维向量
 * uhf: 均匀分布多维向量结构体
 * conf: LSH配置
 * 成功返回正数，失败返回负数
 */
int initUniformHashFunction(UniformHashFunction *uhf, LshConfig *conf);

void freeHashFamily(HashFamily *hash_family, int num);

/*
 * 得到满足均匀分布的浮点数
 * @range_start: 范围起始点
 * @range_end: 范围结束点
 */
float getUniformRandom(float range_start, float range_end);

/*
 * 得到满足高斯分布的浮点数
 */
float getGaussianRandom();

/*
 * 得到随机的32位无符号整数
 * @range_start: 范围起始点
 * @range_end: 范围结束点
 */
unsigned int getRandomUns32(unsigned int range_start, unsigned int range_end);

/*
 * 计算LSH的哈希值 
 * data: 指向用户插入哈希表的数据
 * other: 用于传入一些计算哈希值需要的附加信息
 */
unsigned long computeLsh(void *data, void *other);

/*
 * 释放哈希函数族的内存空间
 */
void freeHashFamily(HashFamily *hash_family, int num);

/*
 * 释放LSH参数的内存空间
 */
void freeLshParam(LshParam *lsh_param);


#endif


