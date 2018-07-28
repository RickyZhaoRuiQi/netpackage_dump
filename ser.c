#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

int create_sockfd();

int main()
{

	int sockfd = create_sockfd();
	assert( sockfd != -1 );
	
	char buff[1024] = {0};
	while(1)
	{
		struct sockaddr_in caddr;
		socklen_t len = sizeof(caddr);
		
		int c = accept(sockfd,(struct sockaddr *)&caddr,&len);
		if( c == -1 )
			continue;

		memset(buff,0,1024);
		int size = recv(c,buff,1024,0);
		send(c,"ok",2,0);
		printf("recv:%s\n",buff);
		close(c);
	}

	return 0;	
}

int create_sockfd()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if( sockfd == -1 )
		return sockfd;

	struct sockaddr_in saddr;
	memset( &saddr,0,sizeof(saddr) );
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6000); //ftp 什么端口？
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = bind( sockfd,(struct sockaddr*)&saddr,sizeof(saddr) );
	if( res == -1 )
		return res;

	listen( sockfd,5 );
	return sockfd;
}
