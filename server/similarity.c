/*
 * the implementation for the function in similarity.h 
 * author: zhenhua
 * date: 2015.1
*/

#include <flann/flann.h>  
#include "similarity.h"
#include <pthread.h>

extern char *path_name;

/*
 * 在qsort中使用的比较函数，按距离排序
 */
int imageComp(const void *a, const void *b)
{
	ImageSimilarity *c, *d;

	c = (ImageSimilarity *)a;
	d = (ImageSimilarity *)b;

	return (d->image_dis) - (c->image_dis);
}


/*
 * 根据索引结构的查询结果，计算结果范围
 * calKeypointSimilarity 计算查询结果
 * ImageGroups **groups 查询结果集合
 * NNResult *res 单次查询结果
 * dimension 向量维度
 * *point 单个查询点
 */
int calResultScope(ImageGroups **groups, NNResult *res, float *cur_query_keypoint, int dimension)
{
	ImageGroups *temp;
	ImageSimilarity *pImage;
	float *p, *result_keypoint;
	char *name, new_name[NAME_LENGTH];
	int x, y, z, zz, n, m, i;
	float dis;
	int near_hamming_dis;

	temp = (ImageGroups *)malloc( sizeof(ImageGroups));
	if(temp == NULL)
	{
		fprintf(stderr, "calResultScope: temp malloc error \n");
		return -1;
	}
	temp->size = TOPK_SIMILARITY_IMAGE;
	temp->count = 0;
	temp->simi = (ImageSimilarity *)malloc(temp->size * sizeof(ImageSimilarity));
	if(temp->simi == NULL)
	{
		fprintf(stderr, "calResultScope: temp->simi malloc error\n");
		return -1;
	}
	memset(temp->simi, 0, temp->size * sizeof(ImageSimilarity));

	result_keypoint = (float *)malloc(dimension * sizeof(float));
	if(result_keypoint == NULL)
	{
		fprintf(stderr, "calResultScope: result_keypoint malloc error \n");
		return -1;
	}

	for(i = 0; i < res->num; i++)
	{
		name = (char *)(res->info[i]);
		sscanf(name, "%d_%d_%d_%d", &x, &y, &z, &zz);
		memset(new_name, '\0', NAME_LENGTH);
		sprintf(new_name, "%d_%d_%03d", x,y,z);
		
		//遍历临时表
		pImage = temp->simi;
		for(n = 0; n < temp->count; n++)
		{
			if( (strcmp((pImage + n)->image_name, new_name) == 0))
				break;
		}

		//若超过临时表，则重新分配
		if(temp->count >= temp->size)
		{
			temp->size += TOPK_SIMILARITY_IMAGE;
			temp->simi = (ImageSimilarity *)realloc(temp->simi, \
				sizeof(ImageSimilarity) * temp->size);
			if(temp->simi == NULL)
			{
				fprintf(stderr, "calResultScope: temp->simi realloc error\n");
				return -1;
			}
			memset(temp->simi + temp->count, 0, sizeof(ImageSimilarity) * (temp->size-temp->count) );
		}

		if(n == temp->count)
		{
			strcpy((temp->simi + temp->count)->image_name, new_name);
			temp->count++;
/*			near_hamming_dis = comuputeNearHammingDistance(cur_query_keypoint, ((float **)res->data)[i], dimension);
			if(fabs(near_hamming_dis - NEAR_HAMMING_MEDIAN) < NEAR_HAMMING_DISTANCE_THRES)
			{
#ifdef _DEBUG
				printf("name: %s\tnear hamming dis: %d\t", (char *)(res->info[i]), near_hamming_dis);
#endif
				strcpy((temp->simi + temp->count)->image_name, new_name);
				temp->count++;
			}
*/

/*			dis = computeDistance(cur_query_keypoint, ((float **)res->data)[i], dimension);
			if(dis < KEYPOINT_DISTANCE_THRES)
			{
				strcpy((temp->simi + temp->count)->image_name, new_name);
				temp->count++;
			}
*/
#ifdef _DEBUG
			//printf("E2 dis: %.0f\n", dis);
#endif

		}
	}

	//将单个keypoint的查询结果添加到image的结果中
	for(n = 0; n < temp->count; n++)
	{
		for(m = 0; m < (*groups)->count; m++)
		{
			if(strcmp((temp->simi + n)->image_name, ((*groups)->simi + m)->image_name) == 0)
				break;
		}

		if((*groups)->count >= (*groups)->size)
		{
			(*groups)->size += TOPK_SIMILARITY_IMAGE;
			(*groups)->simi = (ImageSimilarity *)realloc((*groups)->simi, sizeof(ImageSimilarity) * (*groups)->size);
			if((*groups)->simi == NULL)
			{
				fprintf(stderr, "calResultScope: *groups realloc error\n");
				return -1;
			}
			memset((*groups)->simi + (*groups)->count, 0, sizeof(ImageSimilarity) * ((*groups)->size - (*groups)->count));
		}
		if(m == (*groups)->count)
		{
			strcpy(((*groups)->simi + (*groups)->count)->image_name, (temp->simi + n)->image_name);
			(*groups)->count++;
		}
	}

	free(temp->simi);
	free(temp);
	free(result_keypoint);

	return 0;
}

//计算近似海明距离
inline int comuputeNearHammingDistance(float *point1, float *point2, int dimension)
{
	int dis = 0;
	int i;

	for(i = 0;i < dimension; ++i)
	{
		dis += (fabs(point1[i] - point2[i]) < NEAR_HAMMING_DIFFERT_THRES)?0:1;
	}
	return dis;
}

//计算欧拉距离
inline float computeDistance(float *point1, float *point2, int dimension)
{
	long sum = 0;
	int i;
	float diff;

	for(i = 0; i < dimension; ++i)
	{
		diff = point1[i] - point2[i];
		sum += diff * diff;
	}
	return sum;
}

//复制
void copyPoint(float *dst, float *src, int dimension)
{
	int i;

	for(i = 0; i < dimension; ++i)
	{
		dst[i] = src[i];
	}
}

int readKeypoints(FILE *fp, float *data, int rows, int cols)
{
    char buf[BUFSIZE];
    float *p;
    int ret = 0;
    int i, j;

    p = data;
    for(i = 0; i < rows && !feof(fp); i++) 
    {
	    fgets(buf, BUFSIZE, fp); // read keypoint name
	    
	    if(feof(fp))
		    break;

	    for(j = 0; j < cols; j++) 
	    {
		    fscanf(fp, "%f", p);
		    p++;
	    }
	    ret += 1;
	    fgets(buf, BUFSIZE, fp);
    }    

    return ret;
}

#define THREAD_NUM 4

typedef struct query_param_t {
	float *dataset;
	int rows;
	int cols;
	ImageSimilarity *simi;
	int count;
} query_param_t;

//根据局部特征点计算两张图片匹配的特征点个数
//使用多线程计算具体的调用函数
void *calSimilarity(void *arg)
{
        pthread_t tid;
	int i, j;
	query_param_t param = *((query_param_t *)arg);
	
	float *dataset = param.dataset;
	int rows = param.rows;
	int cols = param.cols;
	ImageSimilarity *simi = param.simi;
	int count = param.count;

	float *testset;
	int *result;
	float *dists;
	
	int nn = 2;
	int trows = 1000;
	int ret;
	FILE *fp;

	flann_index_t index_id;
	struct FLANNParameters p;
	float speedup;
	int matchedCount;
	char keypoints_name[NET_NAME_LENGTH];

	int total_keypoint_count = 0;

	
        
	result   = (int *)  malloc(trows * nn * sizeof(int));
	dists    = (float *)malloc(trows * nn * sizeof(float));
	testset = (float *)malloc(trows * cols * sizeof(float)); 
	if(result == NULL || dists == NULL || testset == NULL)
	{	
		fprintf(stderr, "calSimilarity: result or dists or testset malloc error\n");
		pthread_exit(NULL);
	}

	p = DEFAULT_FLANN_PARAMETERS;
	p.algorithm = FLANN_INDEX_KDTREE;
	p.trees = 8;
	p.log_level = FLANN_LOG_INFO;
	p.checks = 64;

	index_id = flann_build_index(dataset, rows, cols, &speedup, &p);

	for(i = 0; i < count; i++) {
		memset(keypoints_name, '\0', NET_NAME_LENGTH);
		sprintf(keypoints_name, "%s/%s.txt", path_name, (simi + i)->image_name);
		
		if(NULL == (fp = fopen(keypoints_name, "r")))
		{
			fprintf(stderr, "calSimilarity: can't open the keypoint file\n");
			pthread_exit(NULL);
		}
	
		matchedCount = 0;	
		total_keypoint_count = 0;	

		while((ret = readKeypoints(fp, testset, trows, cols)) > 0)
		{
			flann_find_nearest_neighbors_index(index_id, testset, ret, result, dists, nn, &p);
			for(j = 0; j < ret; j++) 
			{
				if(dists[nn*j]/dists[nn*j+1] < SQ_DIST_RATIO_THR)
				{
					matchedCount++;
				}					
			}		

			total_keypoint_count += ret;	
		}

		fclose(fp);

		//匹配率
	
#ifdef _DEBUG	
		//printf("calKeypointSimilarity: matchedCount: %d, first_result's_keypoint_count: %d\n", matchedCount, total_keypoint_count);
#endif
	//	(simi + i)->image_dis =  matchedCount / (1.0 * total_keypoint_count) * 100.0 ;
		(simi + i)->image_dis = matchedCount * 1.0;

#ifdef	_DEBUG
		tid = pthread_self();
	//	printf("tid %lu calSimilarity | cal image: %s, matchedCount: %d\n", (unsigned long)tid, (simi + i)->image_name, matchedCount);
#endif
	}

	flann_free_index(index_id, &p);
	free(testset);
	free(result);
	free(dists);
	
#ifdef _DEBUG
	tid = pthread_self();
//	fprintf(stderr, "tid %lu running over\n", (unsigned long)tid);
#endif
	return NULL;
}

//根据局部特征点计算两张图片匹配的特征点个数
//使用kd-tree进行比较，使用flann库支持(需要提前安装flann库)
//使用多线程计算
int calKeypointSimilarity(ImageGroups **groups, int dimension, float **query_keypoints, int query_keypoints_count)
{
	int i;
	float *dataset;	
	pthread_t tid[THREAD_NUM];
	int increment;
	query_param_t param[THREAD_NUM];
	ImageSimilarity *psimi;
	int cols = dimension;

	float *temp;

	dataset = (float *)malloc(query_keypoints_count * cols * sizeof(float));
	temp = dataset;
  	for(i = 0; i < query_keypoints_count; i++) 
	{
		copyPoint(temp, query_keypoints[i], cols);
		temp += cols;
	}

#ifdef _DEBUG	
//	printf("calKeypointSimilarity | calResultScope returned image count: %d\n", (*groups)->count);
/*	for(i = 0; i < (*groups)->count; ++i)
	{
		printf("returned image: %s\n", ((*groups)->simi + i)->image_name);
	}
*/
//	printf("calKeypointSimilarity | query image's keypoints count: %d\n", query_keypoints_count);
#endif

	increment = (*groups)->count / THREAD_NUM;
	psimi = (*groups)->simi;
	for(i = 0; i < THREAD_NUM; i++) {
		param[i].dataset = dataset;
		param[i].rows = query_keypoints_count;
		param[i].cols = cols;
		param[i].simi = psimi;		
		param[i].count = increment;
		if(i == THREAD_NUM - 1) {
			param[i].count = ((*groups)->count) - increment * (THREAD_NUM - 1); 
		}
		pthread_create(&tid[i], NULL, calSimilarity, (void *)&param[i]);
		psimi += increment;
	}

	for(i=0; i < THREAD_NUM; i++) {
		pthread_join(tid[i], NULL);
	}
	
#ifdef _DEBUG
//	fprintf(stderr, "pthread running over\n");
#endif
	free(dataset);
	return 0;
}


/*
 *  创建ImageGroups，用于存放查询结果
 *
 */
ImageGroups *groupsCreate(void *para)
{
	ImageGroups *iGroups;

	iGroups = (ImageGroups *)malloc( sizeof(ImageGroups));
	if(iGroups == NULL)
	{
		fprintf(stderr, "iGroups malloc error \n");
		return NULL;
	}
	iGroups->size = TOPK_SIMILARITY_IMAGE;
	iGroups->count = 0;
	iGroups->simi = (ImageSimilarity *)malloc(iGroups->size * sizeof(ImageSimilarity));
	if(iGroups->simi == NULL)
	{
		fprintf(stderr, "iGroups->simi malloc error\n");
		return NULL;
	}
	memset(iGroups->simi, 0, iGroups->size * sizeof(ImageSimilarity));

	return iGroups;
}



/*
 * 返回查询结果
 *
 */
/*
ImageGroups *getImageResult(Nest *nest, FILE *search_fp, int dimension)
{
	ImageGroups *iGroups;
	NNResult *res;
	float *point;
//	char *char_data;
	char filename[1024], buf[1024];
	int i;

	point = malloc(sizeof(float) * dimension);
	if(point == NULL) {
		fprintf(stderr, "allocate memory failed.\n");
		return NULL;
	}


//	char_data = malloc(dimension + 1);
//	if(char_data == NULL) {
//		free(point);
//		return NULL;
//	}


	iGroups = (ImageGroups *)malloc( sizeof(ImageGroups));
	if(iGroups == NULL)
	{
		printf("iGroups malloc error \n");
		free(point);
//		free(char_data);
		return NULL;
	}
	iGroups->size = TOPK_SIMILARITY_IMAGE;
	iGroups->count = 0;
	iGroups->simi = (ImageSimilarity *)malloc(iGroups->size * sizeof(ImageSimilarity));
	if(iGroups->simi == NULL)
	{
		printf("iGroups->simi malloc error\n");
		free(point);
//		free(char_data);
		return NULL;
	}
	memset(iGroups->simi, 0, iGroups->size * sizeof(ImageSimilarity));

	while(fgets(filename, 1024, search_fp) != NULL) 
	{
	
//		fgets(char_data, dimension + 2, search_fp);	
//		if(char_data == NULL) break;

//		for(i = 0; i < dimension; i++) {
//			point[i] = (float)(char_data[i] - '0');
//		}	
	
		for(i = 0; i < dimension; ++i)
			fscanf(search_fp, "%f", &point[i]);
		fgets(buf, 1024, search_fp);              //读取数据行剩余数据

		res = nestGetNN(nest, point);


		//解析查询视频名称
//		if(flag == 0) 
//		{
//			sscanf(filename, "%d_%d_%d.key", &x, &y, &z);
//			memset(new_name, '\0', NAME_LENGTH);
//			sprintf(new_name, "%d_%d", x,y);	
//			flag = 1;	
//		}


		calKeypointSimilarity(&iGroups, res, dimension, point);

		freeNNResult(res);
	}

	qsort(iGroups->simi, iGroups->count, sizeof(ImageSimilarity), imageComp);

	return iGroups;
}
*/


/*
 * 打印查询结果
 */
/*
void similarityPrint(ImageGroups *iGroups, FILE *search_fp)
{
	char filename[1024], new_name[NAME_LENGTH];
	int n, x, y, z, zz;

	rewind(search_fp);

	memset(filename, '\0', 1024);
	memset(new_name, '\0', NAME_LENGTH);

	fgets(filename, 1024, search_fp);
	sscanf(filename, "%d_%d_%d_%d", &x, &y, &z, &zz);
	memset(new_name, '\0', NAME_LENGTH);
	sprintf(new_name, "%d_%d_%d", x,y,z);

	printf("The query image is: %s\n", new_name);
	printf("The query result:\n");
	printf("\tnum\tname\t\tsimi\n");
	for(n = 0; n < iGroups->count; n++)
	{
		printf("\t%d\t%s\t\t\t%.0f\n", n+1, (iGroups->simi + n)->image_name, (iGroups->simi + n)->image_dis);
	}
}
*/

