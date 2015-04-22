#ifndef NET_WRAP_H_
#define NET_WRAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>


#define LISTENQ 1024
#define PORT 31120


void perr_exit(const char *s);

int SocketListen(const int port);

int SocketConnect(const int port);

int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr);

ssize_t Readn(int fd, void *vptr, size_t n);

ssize_t Writen(int fd, const void *vptr, ssize_t n);

//int convertASCIItoStr(char *asc, char *str, int length, int dimension);

int splitStrToFloat(char *str, float *num, int dimension);


#endif


