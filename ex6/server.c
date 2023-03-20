#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define IP_PROTOCOL 0
#define NET_BUF_SIZE 32
#define sendrecvflag 0
#define nofile "File Not Found!"


void clearBuf(char* b)
{
	int i;
	for (i = 0; i < NET_BUF_SIZE; i++)
		b[i] = '\0';
}


int main(int argc,char* argv[])
{
	int sockfd, nBytes;
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(atoi(argv[1]));
	addr_con.sin_addr.s_addr = INADDR_ANY;
	char net_buf[NET_BUF_SIZE];
	FILE* fp;

	
	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);

	if (sockfd < 0)
		printf("\nfile descriptor not received!!\n");
	
	
	if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) != 0)
		printf("\nBinding Failed!\n");

	{
		
		clearBuf(net_buf);

		nBytes = recvfrom(sockfd, net_buf,NET_BUF_SIZE, sendrecvflag,(struct sockaddr*)&addr_con, &addrlen);

		fp = fopen(net_buf, "r");
		printf("\nFile Name obtained: %s\n", net_buf);
		if (fp == NULL)
			printf("\nFile open failed!\n");
		else
		{
			printf("\nFile Successfully opened!\n");
			char ch;
			for (int i = 0; i < NET_BUF_SIZE; i++) 
			{
				ch = fgetc(fp);
				net_buf[i] = ch;
					if (ch == EOF)
						break;
			}
			
			
			sendto(sockfd, net_buf, NET_BUF_SIZE,sendrecvflag,(struct sockaddr*)&addr_con, addrlen);
			printf("\nFile Successfully sent!\n");
			clearBuf(net_buf);
		
			if (fp != NULL)
				fclose(fp);
		}

			
		
	}
	return 0;
}
