/*
 * the similarity function
 * author: zhenhua
 * date: 2015.1
*/


#ifndef _SIMI_H
#define _SIMI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nest.h"
#include "net_headers.h"

#define NAME_LENGTH 30
#define TOPK_SIMILARITY_IMAGE 100

#define SIMILARITY_MAX_NUM 99999999
/*threshold on squared ratio of distances between NN and 2NN */
#define SQ_DIST_RATIO_THR  0.64 

#define KEYPOINT_DISTANCE_THRES 30000

#define NEAR_HAMMING_DIFFERT_THRES 1
#define NEAR_HAMMING_DISTANCE_THRES 7
#define NEAR_HAMMING_MEDIAN 85

/*
 * ImageSimilarity： 查询结果的基本单位
 * image_name: 返回结果中的图片名
 * image_dis: 返回结果中该图片与查询图片的相似度
 */
typedef struct ImageSimilarity{
	char image_name[NAME_LENGTH];
	float image_dis;
}ImageSimilarity;

/*
 * ImageGroups: 查询结果集合
 * size: 表示集合大小
 * count：表示集合已经使用的大小
 * ImageSimilarity *simi: 查询结果集合
 */
typedef struct ImageGroups{
	int size;
	int count;
	ImageSimilarity *simi;
}ImageGroups;

//qsort的比较函数
int imageComp(const void *a, const void *b);

//查询完索引结构后，计算结果范围
int calResultScope(ImageGroups **groups, NNResult *res, float *cur_query_keypoint, int dimension);

//计算近似海明距离
inline int comuputeNearHammingDistance(float *point1, float *point2, int dimension);

//计算欧拉距离
inline float computeDistance(float *point1, float *point2, int dimension);

//复制
void copyPoint(float *dst, float *src, int dimension);

//根据局部特征点计算两张图片匹配的特征点个数
//使用kd-tree进行比较，使用flann库支持(需要提前安装flann库)
//使用多线程计算
int calKeypointSimilarity(ImageGroups **groups, int dimension, float **query_keypoints, int query_keypoints_count);

//计算结果范围前，为ImageGroups分配存储空间
ImageGroups *groupsCreate(void *para);

#endif






