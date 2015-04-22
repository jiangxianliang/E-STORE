/*
 * headers for server
 * author: zhenhua
 * date: 2015.1
*/


#ifndef NET_HEADERS_H_
#define NET_HEADERS_H_


#define DEV_CLOSE 0
#define DEV_PC  1
#define DEV_PHONE  2

#define TASK_QUERY 1
#define TASK_UPLOAD 2

#define TASK_UPLOAD_KEYPOINT 1
#define TASK_UPLOAD_IMAGE 2

#define NET_NAME_LENGTH 256
#define KEYPOINT_LENGTH 1024

#define IMAGE_SIZE 10000

#define bool int
#define TRUE 0
#define FALSE 1

#define IMAGECOUNT 200
#define KEYPOINT_COUNT 1000

#define RETURN_IMAGECOUNT 100
#define RETURN_IMAGECOUNT_FOR_PHONE 5

#define PATH_NAME_LENGTH 1024

#define BUFSIZE 1024

#define _DEBUG

/*
 *  客户端上传图片时，数据包格式
 *  
 *  1. 第一次同客户端交互信息(从客户端接收查询请求)
 *  数据包格式： |dev_tag(int)|task_tag(int)| task_upload_tag(int) |battery(int)|imagecount(int)|
 *  数据包格式： |filenamelen(int)|filename(char *)|keypointcount(int)|keypoint1(char *)|keypoint2(char *)| ... |keypointn(char*)|
 *  数据包格式： |filenamelen(int)|filename(char *)|keypointcount(int)|keypoint1(char *)|keypoint2(char *)| ... |keypointn(char*)|
 *  数据包格式： |... ...|
 *
 *  2. 第二次同客户端交互消息(向客户端发送消息，返回需要上传的图片个数及编号（编号从1开始计起）)
 *  数据包格式： |dev_tag(int)|task_tag(int)|uploadimagecount(int)|number1(int)|number2(int)| ... |
 *
 *  3. 第三次同客户端交互信息(从客户端接收实际上传的图片)
 *  数据包格式： |dev_tag(int)|task_tag(int)| task_upload_tag(int) |uploadimagecount(int)|imagename1(char *)|image1size(int)|image1(char *)|
 *  数据包格式： |imagename2(char *)|image2size(int)|image2(char *)| ... |
 */

/*
 *  客户端进行查询时，数据包格式  
 *   
 *  1. 第一次同客户端交互信息（从客户端接收查询请求）
 *  数据包格式： |dev_tag(int)|task_tag(int)|battery(int)|imagecount(int)(默认为1)|
 *  数据包格式： |filenamelen(int)|filename(char *)|keypointcount(int)|keypoint1(char *)|keypoint2(char *)| ... |keypointn(char*)|
 *  数据包格式： |... ...|
 *
 *  2. 第二次同客户端交互信息（向客户端发送查询结果，返回图片）
 *  数据包格式： |dev_tag(int)|task_tag(int)|queryimagecount(int)|imagename1(char *)|image1size(int)|image1similarity(int)|image1(char *)|
 *  数据包格式： |imagename2(char *)|image2size(int)|image2similarity(int)|image2(char *)| ... |
 */



#endif
