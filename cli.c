#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <error.h>

int main(int argc,char* argv[])
{
	if(argc < 2)
	{
		perror("input error\n");
		return 0;
	}

	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert( sockfd != -1);

	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6000);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert( res != -1);
	
	char buff[1024] = {0};
	for(int i = 1;i < argc;++i)
	{
		if(strlen(buff) + strlen(argv[i]) >= 1024)
		{
			perror("data too long\n");
			break;
		}
		strncat(buff,argv[i],strlen(argv[i]));
	}

	send(sockfd,buff,strlen(buff),0);
	memset(buff,0,sizeof(buff));
	recv(sockfd,buff,1024,0);
	printf("recv:%s\n",buff);
	close(sockfd);
	return 0;
}
