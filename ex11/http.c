#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include <ifaddrs.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include<netdb.h>

#define SIZE 512

void main(int argc, char *argv[])
{
    char domain[20],path[100];
    
    sscanf(argv[1],"http://%[^/]%[^\n]", domain,path);

    struct hostent *server;
    server = (struct hostent*)malloc(sizeof(struct hostent));
    server = gethostbyname(domain);
    struct in_addr **ip = (struct in_addr **)server->h_addr_list;
    
    int len;
    int sockfd, n;
    
    struct sockaddr_in servaddr, cliaddr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
   	 perror("\n cannot create socket");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(inet_ntoa(*ip[0]));
    servaddr.sin_port = htons(80);
    
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
   	 printf("\nConnection error\n");
    
    printf("\n\nConnected\n\n");
    
    char buff[100];
    sprintf(buff,"GET %s HTTP/1.1\r\nHost: %s \r\n\r\n",path,domain);
    n = send(sockfd,buff,100,0);
    
    char recvbuff[SIZE];
    int count;

    FILE *fw;
    fw = fopen("download.html","w");
    while(1){
    	count = recv(sockfd, recvbuff, SIZE,0);

    	if(!strcmp(recvbuff,"\0"))
   		 break;

    	fwrite(recvbuff,SIZE, 1, fw);
    	memset(recvbuff,0,SIZE);
    }
    printf("File Downloaded!\n");
    fclose(fw);
    
    close(sockfd);
}
