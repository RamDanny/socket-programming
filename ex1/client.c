#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <unistd.h>
#include<netinet/in.h>
#include<stdlib.h>


#define SERVER_PORT 5431

int main(int argc,char *argv[])
{
    
	struct sockaddr_in sin;
	char *host;
	char buff[100];
	int s;
	int len;
    
	bzero((char*)&sin,sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr= INADDR_ANY;
	sin.sin_port= htons(atoi(argv[1]));
    
	if((s=socket(PF_INET,SOCK_STREAM,0)) < 0 )
	{
    	printf("simplex-talk:socket");
    	return 1;
	}
    
	if(connect(s,(struct sockaddr *)&sin,sizeof(sin)) < 0)
	{
    
    	printf("simplex-talk: connect");
    	close(s);
    	return 1;
	}
    
	printf("Enter message:\n");
	scanf("%[^\n]%*c",buff);
    
    
	send(s,buff,sizeof(buff),0);
    

	return 0;
}
