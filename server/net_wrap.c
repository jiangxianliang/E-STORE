

#include "net_wrap.h"

void perr_exit(const char *s)
{
	perror(s);
	exit(1);
}

int SocketListen(const int port)
{
	int listen_fd;
	struct sockaddr_in server_addr;
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
	
	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	
	int one = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));	

	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		return -1;
	}
	
	if(listen(listen_fd, LISTENQ) < 0) {
		perror("listen");	
		return -1;
	}
		
	return listen_fd ;	
}

int SocketConnect(const int port)
{
	int conn_fd;
	struct sockaddr_in cli_addr;

	memset(&cli_addr, '\0', sizeof(cli_addr));
   	cli_addr.sin_family = AF_INET;	
	inet_pton(AF_INET, "127.0.0.1", &cli_addr.sin_addr);
	cli_addr.sin_port = htons(port);

	if((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket connect error\n");
		return -1;	
	}

  	if(connect(conn_fd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0)
	{
		perror("connect error\n");
		return -1;
	}
	
	return conn_fd;
}


int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr)
{
	int n;
again:
	if((n = accept(fd,sa,salenptr))<0)
	{
		if((errno == ECONNABORTED)||(errno == EINTR))
			goto again;
		else 
			perr_exit("accept error\n");
	}
	
	return n;
}

ssize_t Readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nread = read(fd, ptr, nleft)) < 0)
		{
			if(errno == EINTR)
				nread = 0;
			else return -1;
		}
		else if(nread == 0)
			break;
		nleft -= nread;
		ptr += nread;
	}
	return n - nleft;
}

ssize_t Writen(int fd, const void *vptr, ssize_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}



int splitStrToFloat(char *str, float *num, int dimension)
{
	int i;
	char temp[5];
	char *first, *index;
	
	first = str;
	index = strchr(first, ' ');
	for(i = 0; i < dimension; ++i)
	{
		memset(temp, '\0', 5);
		strncpy(temp, first, index-first);
		num[i] = atoi(temp);
		first = index + 1;
		index = strchr(first, ' ');
	}
	return 0;
}

/*
int convertASCIItoStr(char *asc, char *str, int length, int dimension)
{
	int i;
	int temp;

	memset(str, '\0', length);
	for(i = 0; i < length; i++)
	{
		temp = *(int *)(asc + i);
		if(temp != 32)  //空格的ASCII值为32
			str[i] = temp - '0';
		else if(temp == 32)
			str[i] = ' ';
	}

	return 0;
}
*/


