/*
 * reponse functions for the server 
 * author: zhenhua
 * date: 2015.1
*/


#ifndef NET_UTILITY_PHONE_H_
#define NET_UTILITY_PHONE_H_

#include "nest.h"
#include "similarity.h"
#include "net_wrap.h"
#include "net_headers.h"

/*
 *  反转字符串
 */
char *revstr(char *str, size_t len);

/*
 *  
 *  将缓冲区中待写入图片名称的路径去掉，只保留名称
 */
int getImagenameWithoutPath(char *old_name, char *new_name);


/*
 * 获取图片大小
 */
int getFileSize(char *fileName);


/*
 *  将图片发送到对应端口号
 */
int sendImageToNet(int conn_fd, char *image_name, int image_size);

//响应对应设备的处理函数
int reponseWithDevice(Nest *nest, int dimension, int sock_fd, int dev_tag);

//判断图片是否上传的准则
bool judgeImageUploadOrNot(int battery, ImageGroups *iGroups, int query_keypoints_count);

//响应客户端的图片上传请求(图片上传时，同客户端第一次通信)
int responseDeviceForUpload(Nest *nest, int dimension, int sock_fd, int dev_tag, int task_tag);

//在上传请求中，向客户端返回结果(图片上传时，同客户端第二次通信)
int writeMessageForUpload(int sock_fd, int dev_tag, int task_tag, int uploadCount, int *imageName);

//在上传请求中，接收客户端上传的图片(图片上传时，同客户端第三次通信)
int recvImageFromDeviceForUpload(int sock_fd, int dev_tag, int task_tag);

//将缓冲区中的图片写入到文件中
int writeImageToFile(int sock_fd, char *image_name, int image_size, int dev_tag, int task_tag);

//响应客户端的图片查询请求(图片查询时，同客户端的第一次通信)
int responseDeviceForQuery(Nest *nest, int dimension, int sock_fd, int dev_tag, int task_tag);

//在查询请求中，向客户端返回结果(图片查询时，同客户端的第二次通信)
int writeMessageForQuery(int sock_fd, int dev_tag, int task_tag, int battery, ImageGroups *iGroups);

#endif
