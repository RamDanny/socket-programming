#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>

#define SERVER_PORT 5431


int main(int argc, char * argv[])
{
	struct sockaddr_in sin;
	int s,new_s;
	int len;
	char buff[100];
    
	bzero((char*)&sin,sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr= INADDR_ANY;
	sin.sin_port= htons(atoi(argv[1]));
    
	s=socket(PF_INET,SOCK_STREAM,0);
    
	if(s > 0)
	{
    	if(bind(s,(struct sockaddr *)&sin,sizeof(sin)) < 0)
    	{
        	printf("Simplex-talk:bind\n");
            	return 1;
    	}
   	 
    	listen(s,5);
   	 
    
   	 
        	if((new_s=accept(s,(struct sockaddr *)&sin,&len)) < 0)
        	{
            	printf("Simplex-talk:accept\n");
            	return 1;
        	}
        	while(len=recv(new_s,buff,sizeof(buff),0))
        	{
            	int i=0;
            	printf("\nClient's message:\n");
            	while(buff[i]!='\0')
                	printf("%c",buff[i++]);
            	printf("\n");
            	send(s,buff,sizeof(buff),0);
            	close(new_s);
            	return 0;
        	}
    	 
    
	}
    
	return 0;
}
