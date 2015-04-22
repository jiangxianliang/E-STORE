/*
 * the implementation for the function in net_utility.h 
 * author: zhenhua
 * date: 2015.1
*/

#include "net_utility.h"
#include <time.h>

extern char *path_name;

//响应对应设备的处理函数
int reponseWithDevice(Nest *nest, int dimension, int sock_fd, int dev_tag)
{
	char *recvline;
	int task_tag;
	int ret;
	int n;

	//解析任务号(自定义的通信协议见net_headers.h)
	recvline = (char *)malloc(sizeof(int));
	memset(recvline, '\0', sizeof(int));
	if(sizeof(int) != (n = Readn(sock_fd, recvline, sizeof(int) ) ) )  
	{	
		fprintf(stderr, "responseWithDevice Readn failed\n");
		return -1;
	}
	task_tag = *(int *)recvline;
	
	switch(task_tag)
	{
		case TASK_UPLOAD:
			//响应客户端的图片上传请求(图片上传时，同客户端第一次通信)
			ret = responseDeviceForUpload(nest, dimension, sock_fd, dev_tag, task_tag);	
			if(ret == -1)
			{
				fprintf(stderr, "reponseDeviceForUpload failed\n");
				return -1;
			}
			else if(ret == -2)
			{
				fprintf(stderr, "reponseDeviceForUpload ask to close the socket\n");
				return -2;
			}					
	
			break;
		case TASK_QUERY:	
			//响应客户端的图片查询请求(图片查询时，同客户端的第一次通信)	
			ret = responseDeviceForQuery(nest, dimension, sock_fd, dev_tag, task_tag);
			if(ret == -1)
			{
				fprintf(stderr, "responseDeviceForQuery failed\n");
				return -1;
			}
#ifdef _DEBUG
			printf("reponseWithDevice | responseDeviceForQuery success\n");
#endif
			break;
		default:
			printf("response with Device other TASK\n");
	}

	free(recvline);
	return 0;	
}

/*
 *  根据查询结果，和客户端的当前电量，判断当前图片是否需要上传的准则
 *  TRUE 表示需要上传， FALSE 表示不需要上传
 */ 
bool judgeImageUploadOrNot(int battery, ImageGroups *iGroups, int query_keypoints_count)
{
	int i;
	int ret = -1;
	float simiRatio = 0.0;
	float simiThres = 50.00;
	float enlarge = 2.0;	

	simiThres += battery;

	simiRatio = 1.0 * enlarge * (iGroups->simi+ 0)->image_dis;

//	simiRatio = (iGroups->simi+ 0)->image_dis / (1.0 * query_keypoints_count) * 100;
#ifdef _DEBUG
	printf("judgeImageUploadOrNot: image_dis: %.2f\n", (iGroups->simi+ 0)->image_dis);
#endif

#ifdef _DEBUG
	printf("judgeImageUploadOrNot: battery: %d%%, simiThres: %.2f, simiRatio: %.2f  ", battery, simiThres, simiRatio);
#endif

	if(simiRatio > simiThres)
	{
#ifdef _DEBUG
	printf("     FALSE   \n");
#endif
		return FALSE;
	}
	else
	{
#ifdef _DEBUG
	printf("     TRUE   \n");
#endif
		return TRUE;
	}

//	srand((int)time(NULL));
/*
	ret = (int)(2.0 * rand() / (RAND_MAX+1.0));
	if(ret == 0)
		return TRUE;
	else if(ret == 1)
		return FALSE;
*/
//	return TRUE;
}

//响应客户端的图片上传请求(图片上传时，同客户端第一次通信)
int responseDeviceForUpload(Nest *nest, int dimension, int sock_fd, int dev_tag, int task_tag)
{
	char *recvline;
	int battery, task_upload_tag;
	int imageCount, realImageCountOfUpload = 0;
	int i, j, k, n, filenameLen, keypointCount;
	char filename[NET_NAME_LENGTH], *keypoint;
	float **upload_keypoints;
	NNResult *res;
	ImageGroups *iGroups;
	bool judgeResult;
	int *uploadImageName;
	int ret, ret2;

	//解析并判断当前上传的内容，TASK_UPLOAD_KEYPOINT表示当前上传的是局部特征点
	//			   TASK_UPLOAD_IMAGE表示当前上传的是图片
	recvline = (char *)malloc(NET_NAME_LENGTH * sizeof(char));
	memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
	n = Readn(sock_fd, recvline, sizeof(int));
	task_upload_tag = *(int *)recvline;

#ifdef _DEBUG
	printf("task_upload_tag: %d\n", task_upload_tag);
#endif

	switch(task_upload_tag)
	{
		case TASK_UPLOAD_KEYPOINT:
			
			//解析剩余的其他参数
			recvline = (char *)malloc(NET_NAME_LENGTH * sizeof(char));
			memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
			n = Readn(sock_fd, recvline, sizeof(int));
			battery = *(int *)recvline;

			memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
			n = Readn(sock_fd, recvline, sizeof(int));
			imageCount = *(int *)recvline;
#ifdef _DEBUG
			printf("responseDeviceForUpload | battery: %d, imageCount: %d\n", battery, imageCount);
#endif

			keypoint = (char *)malloc(KEYPOINT_LENGTH * sizeof(char));
			upload_keypoints = (float **)malloc(KEYPOINT_COUNT * sizeof(float *));
			for(i = 0; i < KEYPOINT_COUNT; ++i)
			{
				upload_keypoints[i] = (float *)malloc(dimension * sizeof(float));
				memset(upload_keypoints[i], '\0', dimension * sizeof(float));
			}

			uploadImageName = (int *)malloc(imageCount * sizeof(int));
			memset(uploadImageName, '\0', imageCount * sizeof(int));

			for(i = 0; i < imageCount; ++i)
			{
				memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
				n = Readn(sock_fd, recvline, sizeof(int));
				filenameLen = *(int *)recvline;

				memset(filename, '\0', NET_NAME_LENGTH);
				memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
				n = Readn(sock_fd, recvline, filenameLen);
				memcpy(filename, recvline, filenameLen);

				memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
				n = Readn(sock_fd, recvline, sizeof(int));
				keypointCount = *(int *)recvline;
		#ifdef _DEBUG
				printf("\n\n\nresponseDeviceForUpload | filename: %s, keypointCount: %d\n", filename, keypointCount);
		#endif
				iGroups = groupsCreate(NULL);

				for(j = 0; j < keypointCount; ++j)
				{
					memset(keypoint, '\0', KEYPOINT_LENGTH * sizeof(char));
					n = Readn(sock_fd, keypoint, KEYPOINT_LENGTH);
					memset(upload_keypoints[j], '\0', dimension * sizeof(float));
					splitStrToFloat(keypoint, upload_keypoints[j], dimension);

					//查询索引结构
					res = nestGetNN(nest, upload_keypoints[j]);
			
					//计算结果范围(similarity.h)
					ret = calResultScope(&iGroups, res, upload_keypoints[j], dimension);
					if(ret == -1)
					{
						fprintf(stderr, "responseDeviceForUpload: calResultScope error\n");
						return -1;
					}
			
					if(iGroups->simi != NULL)
						freeNNResult(res);
				}

				//计算相似性(similarity.h)
				ret = calKeypointSimilarity(&iGroups, dimension, upload_keypoints, keypointCount);
				if(ret == -1)
				{
					fprintf(stderr, "responseDeviceForUpload: calKeypointSimilarity error\n");
					return -1;
				}

				qsort(iGroups->simi, iGroups->count, sizeof(ImageSimilarity), imageComp);
		
				//判断当前图片是否需要上传的准则
				judgeResult = judgeImageUploadOrNot(battery, iGroups, keypointCount);
				if(judgeResult == TRUE)
				{			
					uploadImageName[realImageCountOfUpload] = i + 1;
					realImageCountOfUpload++;
				}


				if(iGroups->simi != NULL)
					free(iGroups->simi);	
				if(iGroups != NULL)
					free(iGroups);	
			}
			
			//在上传请求中，向客户端返回结果(图片上传时，同客户端第二次通信)
			//给客户端发送回送消息，返回需要上传的图片个数，及图片名称
			ret = writeMessageForUpload(sock_fd, dev_tag, task_tag, realImageCountOfUpload, uploadImageName);
			if(ret == -1)
			{
				fprintf(stderr, "writeMessageToPhoneForUpload error\n");
				return -1;
			}


			free(keypoint);
			free(uploadImageName);
			for(i = 0; i < KEYPOINT_COUNT; ++i)
				free(upload_keypoints[i]);
			free(upload_keypoints);

//************************************************************************
//dev_tag == DEV_PHONE 即连接的是手机端,需要创建两次连接
//返回值为-2，表示主动断开连接
			if(dev_tag == DEV_PHONE)
				return -2;

//*************************************************************************

			break;

		case TASK_UPLOAD_IMAGE:
			//在上传请求中，接收客户端上传的图片(图片上传时，同客户端第三次通信)
			ret2 = recvImageFromDeviceForUpload(sock_fd, dev_tag, task_tag);	
			if(ret2 == -1)
			{
				fprintf(stderr, "recvImageFromPhoneForUpload error\n");
				return -1;
			}

			break;

		default:
			printf("responseDeviceForUpload error, task_upload_tag: %d\n", task_upload_tag);
			return -1;
	}
	
	free(recvline);
	return 0;
}

/*
 * 在上传请求中，向客户端返回结果(图片上传时，同客户端第二次通信)
 * 给客户端发送回送消息，返回需要上传的图片个数，及图片名称
 */ 
int writeMessageForUpload(int sock_fd, int dev_tag, int task_tag, int uploadCount, int *imageName)
{
	char *packet;
	int net_dev_tag, net_task_tag, net_uploadCount;
	int packet_size;
	int i, n;

	packet_size = 3 * sizeof(int) + uploadCount * sizeof(int);
	packet = (char *)malloc(packet_size * sizeof(char));
	memset(packet, '\0', packet_size * sizeof(char));

	//真实模式下，PC 和 PHONE 需要注意大端小端问题
	if(dev_tag == DEV_PHONE)
	{
		net_dev_tag = htonl(dev_tag);
		net_task_tag = htonl(task_tag);
		net_uploadCount = htonl(uploadCount);

		*(int *)packet = net_dev_tag;
		*(int *)(packet + sizeof(int)) = net_task_tag;
		*(int *)(packet + 2*sizeof(int)) = net_uploadCount;

		for(i = 0; i < uploadCount; ++i)
			*(int *)(packet+3*sizeof(int) + i * sizeof(int)) = htonl(imageName[i]);

	}
	else if(dev_tag == DEV_PC)
	{
		*(int *)packet = dev_tag;
		*(int *)(packet + sizeof(int)) = task_tag;
		*(int *)(packet + 2*sizeof(int)) = uploadCount;

		for(i = 0; i < uploadCount; ++i)
			*(int *)(packet+3*sizeof(int) + i * sizeof(int)) = imageName[i];
	}	

#ifdef _DEBUG
	printf("writeMessageForUpload | dev_tag: %d, task_tag: %d, uploadCount: %d\n", dev_tag, task_tag, uploadCount);
#endif
	
	n = Writen(sock_fd, packet, packet_size);
	if(n != packet_size)
	{
		fprintf(stderr, "writeMessageToPhoneForUpload writen failed\n");
		return -1;
	}

	free(packet);

#ifdef _DEBUG
	printf("writeMessageForUpload | write success\n");
#endif
	return 0;	
}

/*
 *  在上传请求中，接收客户端上传的图片(图片上传时，同客户端第三次通信)
 *  接收实际上传的图片
 */
int recvImageFromDeviceForUpload(int sock_fd, int dev_tag, int task_tag)
{
	char *recvline, *buf_name, *index;
	int recv_uploadCount;	
	int image_size;
	char image_name[NET_NAME_LENGTH];
	int i, n, ret;

	recvline = (char *)malloc(sizeof(int));
	buf_name = (char *)malloc(NET_NAME_LENGTH);

	memset(recvline, '\0', sizeof(int));
	n = Readn(sock_fd, recvline, sizeof(int));
	recv_uploadCount = *(int *)recvline;

#ifdef _DEBUG
	printf("recvImageFromDeviceForUpload | recv_uploadCount: %d\n", recv_uploadCount);
#endif

	for(i = 0; i < recv_uploadCount; ++i)
	{
		//接收图片
		memset(buf_name, '\0', NET_NAME_LENGTH);
		n = Readn(sock_fd, buf_name, NET_NAME_LENGTH);

		//手机端上传的图片名字后边用'#'填充
		if(dev_tag == DEV_PHONE)
		{
			memset(image_name, '\0', NET_NAME_LENGTH);
			index = strchr(buf_name, '#');
			strncpy(image_name, buf_name, index - buf_name);
		}	
		else if(dev_tag == DEV_PC)
		{
			memset(image_name, '\0', NET_NAME_LENGTH);
			memcpy(image_name, buf_name, NET_NAME_LENGTH);
		}	
#ifdef _DEBUG
		printf("recvImageFromDeviceForUpload | image_name: %s\n", image_name);
#endif

		memset(recvline, '\0', sizeof(int));
		n = Readn(sock_fd, recvline, sizeof(int));
		image_size = *(int *)recvline;
#ifdef _DEBUG
		printf("recvImageFromDeviceForUpload | image_size: %d\n", image_size);
#endif
		//将缓冲区中的图片写入到文件中
		ret = writeImageToFile(sock_fd, image_name, image_size, dev_tag, task_tag);
		if(ret == -1)
		{	
			fprintf(stderr, "writeImageToFile error\n");
		}
	}

	free(recvline);
	free(buf_name);
	return 0;
}


/*
 *  反转字符串
 */
char *revstr(char *str, size_t len)
{

    char    *start = str;
    char    *end = str + len - 1;
    char    ch;

    if (str != NULL)
    {
        while (start < end)
        {
            ch = *start;
            *start++ = *end;
            *end-- = ch;
        }
    }
    return str;
}

/*
 *  
 *  将缓冲区中待写入图片名称的路径去掉，只保留名称
 */
int getImagenameWithoutPath(char *old_name, char *new_name)
{
	char name1[PATH_NAME_LENGTH], name2[NET_NAME_LENGTH];
	char *str;
	int len, n;
	
	memset(name1, '\0', PATH_NAME_LENGTH);
	memcpy(name1, old_name, PATH_NAME_LENGTH);
	len = strlen(name1);
	revstr(name1, len);
	
	str = strchr(name1, '/');
	n = str - name1;
	
	memset(name2, '\0', NET_NAME_LENGTH);
	strncpy(name2, name1, n);
	revstr(name2, n);
	name2[n] = '\0';

	memcpy(new_name, name2, NET_NAME_LENGTH);

	return 0;
}


/*
 *  将缓冲区中的图片写到文件中
 *
 */
int writeImageToFile(int sock_fd, char *image_name, int image_size, int dev_tag, int task_tag)
{
	char *recv_image;
	int nwritten, n, ret;
	FILE *fp;
	char temp_name[NET_NAME_LENGTH], new_image_name[NET_NAME_LENGTH];
	
	if(dev_tag == DEV_PC)
	{
		memset(temp_name, '\0', NET_NAME_LENGTH);
		ret = getImagenameWithoutPath(image_name, temp_name);
		if(ret != 0)	
		{
			fprintf(stderr, "getImagenameWithoutPath error\n");
		}

		memset(new_image_name, '\0', NET_NAME_LENGTH);
		sprintf(new_image_name, "./recv_from_client/%s", temp_name);
	}
	else if(dev_tag == DEV_PHONE)
	{
		memset(new_image_name, '\0', NET_NAME_LENGTH);
		sprintf(new_image_name, "./recv_from_client/%s", image_name);
	}

	if(NULL == (fp = fopen(new_image_name, "w")))
	{
		fprintf(stderr, "new_image_name fopen error\n");
		return -1;
	}

	recv_image = (char *)malloc(IMAGE_SIZE * sizeof(char));
	if(recv_image == NULL)
	{
		fprintf(stderr, "writeImageToFile create buffer failed\n");
		return -1;
	}
	
	nwritten = 0;
	while(nwritten < image_size)
	{
		if(image_size - nwritten < IMAGE_SIZE)
			n = Readn(sock_fd, recv_image, image_size - nwritten);
		else
			n = Readn(sock_fd, recv_image, IMAGE_SIZE);

		nwritten += n;
		if(fwrite(recv_image, sizeof(char), n, fp) != n)
		{
			fprintf(stderr, "writeImageToFile: fwrite failed\n");
			return -1;
		}			
	}
	
	free(recv_image);
	fclose(fp);
	return 0;
}

//响应客户端的图片查询请求(图片查询时，同客户端的第一次通信)
int responseDeviceForQuery(Nest *nest, int dimension, int sock_fd, int dev_tag, int task_tag)
{
	char *recvline, *keypoint;
	int i, j, n, k, l, ret, filenameLen;
	int battery, imageCount, keypointCount;
	float **query_keypoints;
	char **queryImageName, filename[NET_NAME_LENGTH];
	NNResult *res;
	ImageGroups *iGroups;
#ifdef 	_DEBUG
	struct timeval start, end;
	float time_use = 0.0, time_temp = 0.0;
#endif
#ifdef _DEBUG
	printf("responseDeviceForQuery | starting\n");
#endif

	//解析参数
	recvline = (char *)malloc(NET_NAME_LENGTH * sizeof(char));
	memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
	n = Readn(sock_fd, recvline, sizeof(int));
	battery = *(int *)recvline;

	memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
	n = Readn(sock_fd, recvline, sizeof(int));
	imageCount = *(int *)recvline;

	//目前图片进行查询时，只支持一张图片进行查询，暂时不支持多张图片同时查询(但留有相应借口，方便扩展)
	if(imageCount != 1)
	{
		fprintf(stderr, "responseDeviceForQuery: imageCount != 1 error\n");
		return -1;
	}

	keypoint = (char *)malloc(KEYPOINT_LENGTH * sizeof(char));
	query_keypoints = (float **)malloc(KEYPOINT_COUNT * sizeof(float *));
	for(i = 0; i < KEYPOINT_COUNT; ++i)
	{
		query_keypoints[i] = (float *)malloc(dimension * sizeof(float));
		memset(query_keypoints[i], '\0', dimension * sizeof(float));
	}

	queryImageName = (char **)malloc(imageCount * sizeof(char *));
	for(k = 0; k < imageCount; ++k)
	{
		queryImageName[k] = (char *)malloc(NET_NAME_LENGTH * sizeof(char));	
		memset(queryImageName[k], '\0', NET_NAME_LENGTH);
	}

#ifdef  _DEBUG
	time_use = 0.0;
#endif
	for(i = 0; i < imageCount; ++i)
	{
		memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
		n = Readn(sock_fd, recvline, sizeof(int));
		filenameLen = *(int *)recvline;

		memset(filename, '\0', NET_NAME_LENGTH);
		memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
		n = Readn(sock_fd, recvline, filenameLen);
		memcpy(filename, recvline, filenameLen);

		memset(recvline, '\0', NET_NAME_LENGTH * sizeof(char));
		n = Readn(sock_fd, recvline, sizeof(int));
		keypointCount = *(int *)recvline;

		iGroups = groupsCreate(NULL);

		for(j = 0; j < keypointCount; ++j)
		{
			memset(keypoint, '\0', KEYPOINT_LENGTH * sizeof(char));
			n = Readn(sock_fd, keypoint, KEYPOINT_LENGTH);
			memset(query_keypoints[j], '\0', dimension * sizeof(float));
			splitStrToFloat(keypoint, query_keypoints[j], dimension);		

#ifdef _DEBUG
			gettimeofday(&start, NULL);
#endif
			//查询索引结构
			res = nestGetNN(nest, query_keypoints[j]);

			//计算结果范围
			ret = calResultScope(&iGroups, res, query_keypoints[j], dimension);
			if(ret == -1)
			{
				fprintf(stderr, "responseDeviceForQuery: calResultScope error\n");
				return -1;
			}
#ifdef _DEBUG
			gettimeofday(&end, NULL);
			time_temp = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / (1.0 * 1000);
			time_use += time_temp;
#endif			
			if(iGroups->simi != NULL)
				freeNNResult(res);
		}
	
#ifdef	_DEBUG
		printf("*********nestGetNN and calResultScope | the use of time: %.2f ms\n", time_use);
		time_use = 0.0;
		gettimeofday(&start, NULL);
#endif		
		//计算相似性
		ret = calKeypointSimilarity(&iGroups, dimension, query_keypoints, keypointCount);
		if(ret == -1)
		{
			fprintf(stderr, "responseDeviceForQuery: calKeypointSimilarity error\n");
			return -1;
		}
#ifdef _DEBUG
		gettimeofday(&end, NULL);
		time_use = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / (1.0 * 1000);
		printf("*********calKeypointSimilarity | the use of time: %.2f ms\n", time_use);
		time_use = 0.0;
		gettimeofday(&start, NULL);
#endif
		qsort(iGroups->simi, iGroups->count, sizeof(ImageSimilarity), imageComp);
#ifdef _DEBUG
		gettimeofday(&end, NULL);
		time_use = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / (1.0 * 1000);
		printf("*********qsort | the use of time: %.2f ms\n", time_use);
#endif

#ifdef _DEBUG
/*		printf("responseDeviceForQuery | ImageGroups->count: %d\n", iGroups->count);
		for(j = 0; j < iGroups->count; ++j)
		{
			printf("image: %s , ratio: %.0f\n", (iGroups->simi + j)->image_name, (iGroups->simi + j)->image_dis);
		}
*/
#endif

	}
#ifdef _DEBUG
	printf("responseDeviceForQuery | calKeypointSimilarity success\n");
#endif

	//在查询请求中，向客户端返回结果(图片查询时，同客户端的第二次通信)
	ret = writeMessageForQuery(sock_fd, dev_tag, task_tag, battery, iGroups);
	if(ret == -1)
	{
		fprintf(stderr, "writeMessageForQuery error\n");
		return -1;
	}

#ifdef _DEBUG
	printf("responseDeviceForQuery | writeMessageForQuery success\n");
#endif

	if(iGroups->simi != NULL)
		free(iGroups->simi);	
	if(iGroups != NULL)
		free(iGroups);	
	free(recvline);
	free(keypoint);
	for(k = 0; k < imageCount; ++k)
		free(queryImageName[k]);
	free(queryImageName);	
	for(i = 0; i < KEYPOINT_COUNT; ++i)
		free(query_keypoints[i]);
	free(query_keypoints);
	
	return 0;
}

//在查询请求中，向客户端返回结果(图片查询时，同客户端的第二次通信)
int writeMessageForQuery(int sock_fd, int dev_tag, int task_tag, int battery, ImageGroups *iGroups)
{
	char *packet;
	int i, n, ret, image_size;
	int query_image_count = 0;
	char image_name[PATH_NAME_LENGTH], name[NET_NAME_LENGTH];

	packet = (char *)malloc(3 * sizeof(int));
	if(packet == NULL)
	{
		fprintf(stderr, "writeMessageForQuery: pakcet malloc failed\n");
		return -1;
	}

	query_image_count = RETURN_IMAGECOUNT;
	if(dev_tag == DEV_PHONE)
	{
		query_image_count = RETURN_IMAGECOUNT_FOR_PHONE;
	}
	if(query_image_count > iGroups->count)
		query_image_count = iGroups->count;

	//封装相应参数
	*(int *)packet = dev_tag;
	*(int *)(packet + sizeof(int)) = task_tag;
	*(int *)(packet + 2*sizeof(int)) = query_image_count;

	n = Writen(sock_fd, packet, 3 * sizeof(int));
	if(n != 3 * sizeof(int))
	{
		fprintf(stderr, "writeMessageForQuery: Writen packet error\n");
		return -1;
	}
	free(packet);	

	for(i = 0; i < query_image_count; ++i)
	{
		packet = (char *)malloc(NET_NAME_LENGTH + 2*sizeof(int));
		if(packet == NULL)
		{
			fprintf(stderr, "writeMessageForQuery: pakcet malloc failed\n");
			return -1;
		}
		memset(packet, '\0', NET_NAME_LENGTH + 2*sizeof(int));

		memset(image_name, '\0', PATH_NAME_LENGTH);
		sprintf(image_name, "%s/%s.jpeg", path_name, (iGroups->simi + i)->image_name);

		image_size = getFileSize(image_name);

		memset(name, '\0', NET_NAME_LENGTH);
		sprintf(name, "%s.jpeg", (iGroups->simi + i)->image_name);

		//注意网络的大小端问题
		memcpy(packet, name, NET_NAME_LENGTH);
		if(dev_tag == DEV_PC)
		{
			*(int *)(packet + NET_NAME_LENGTH * sizeof(char)) = image_size;
			*(int *)(packet + NET_NAME_LENGTH + sizeof(int)) = (iGroups->simi + i)->image_dis;
		}
		else if(dev_tag == DEV_PHONE)
		{
			*(int *)(packet + NET_NAME_LENGTH * sizeof(char)) = htonl(image_size);
			*(int *)(packet + NET_NAME_LENGTH + sizeof(int)) = htonl((iGroups->simi + i)->image_dis);
		}

		n = Writen(sock_fd, packet, NET_NAME_LENGTH + 2*sizeof(int));
		if(n != NET_NAME_LENGTH + 2*sizeof(int))
		{
			fprintf(stderr, "writeMessageForQuery: : Writen packet error\n");
			return -1;
		}
		free(packet);	
		
		//将图片发送到对应端口号
		ret = sendImageToNet(sock_fd, image_name, image_size);
		if(ret == -1)
		{
			fprintf(stderr, "writeMessageForQuery: sendImageToNet failed\n");	
			return -1;
		}
	}

}

/*
 * 获取图片大小
 */
int getFileSize(char *fileName)   
{
	FILE * fp;
	int size;

   	fp = fopen(fileName, "r");  
    	if(fp == NULL)	
    	{
		fprintf(stderr, "getFileSize fopen error\n");
		return 0;
    	}
	
	fseek(fp, 0L, SEEK_END);  
        size = ftell(fp);  
        fclose(fp);  
        
  	return size;  
}

/*
 *  将图片发送到对应端口号
 */
int sendImageToNet(int conn_fd, char *image_name, int image_size)
{
	FILE *fp;
	char *buf;
	int nwritten = 0, size;
	int ret;

	buf = (char *)malloc(IMAGE_SIZE * sizeof(char));
	if(buf == NULL)
	{
		fprintf(stderr, "sendImageToNet malloc error\n");
		return -1;
	}

	if(NULL == (fp = fopen(image_name, "r")))
	{
		fprintf(stderr, "sendImageToNet fopen error\n");
		return -1;
	}

	while(nwritten < image_size)
	{
		memset(buf, '\0', IMAGE_SIZE);
		size = fread(buf, sizeof(char), IMAGE_SIZE, fp);
		if(size < 0)
			return -1;
		nwritten += size;
		ret = Writen(conn_fd, buf, size);
		if(ret < 0)
		{
			fprintf(stderr, "sendImageToNet Writen error\n");
			return -1;
		}
	}

	free(buf);
	fclose(fp);

	return 0;
}




