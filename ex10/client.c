#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>

#define MAX 1024


#define MAX_ADDR 10
#define MAX_DOMAIN 20
typedef char string[30];

typedef struct Entry
{
	string domain;
	string address[MAX_ADDR];
} Entry;

void printTable(Entry table[MAX_DOMAIN])
{
	//printf("---------------------------------------\n");
	printf("   Domain Name   	 Address   	\n");
	//printf("---------------------------------------\n");
	for (int i = 0; i < MAX_DOMAIN; i++)
	{
    	if (table[i].domain[0])
    	{
        	printf("| %-15s | %-20s |\n", table[i].domain, table[i].address[0]);

        	for (int j = 1; j < MAX_ADDR && table[i].address[j][0]; j++)
            	printf("| %-15s | %-20s |\n", "", table[i].address[j]);
        	//printf("+-----------------+----------------------+\n");
    	}
	}
	printf("\n");
}

int checkAddress(Entry table[MAX_DOMAIN], char *const address)
{
	string addr_copy;
	strcpy(addr_copy, address);
	char *split;
	int val;
	split = strtok(addr_copy, ".");
	while (split)
	{
    	val = atoi(split);
    	if (val < 0 || val > 255)
        	return -1;
    	split = strtok(NULL, ".");
	}

	for (int i = 0; i < MAX_DOMAIN; i++)
	{
    	if (!table[i].domain[0])
        	continue;

    	for (int j = 0; j < MAX_ADDR && table[i].address[j][0]; j++)
        	if (strcmp(address, table[i].address[j]) == 0)
            	return -2;
	}

	return 0;
}

int createEntry(Entry table[MAX_DOMAIN], char *domain, char *address)
{
	// Search if entry exists already
	int index = -1;
	int flag = 0;

	int addr_invalid = checkAddress(table, address);

	if (addr_invalid)
    	return addr_invalid;

	for (int i = 0; i < MAX_DOMAIN; i++)
	{
    	if (strcmp(table[i].domain, domain) == 0)
    	{
        	for (int j = 0; j < MAX_ADDR; j++)
            	if (!table[i].address[j][0])
            	{
                	strcpy(table[i].address[j], address);
                	flag = 1;
                	break;
            	}
        	break;
    	}
    	if (!table[i].domain[0] && index == -1)
        	index = i;
	}

	// IF entry has to be created
	if (!flag)
	{
    	strcpy(table[index].domain, domain);
    	strcpy(table[index].address[0], address);
    	flag = 1;
	}

	return flag;
}

Entry getAddress(Entry *table, char *const domain)
{
	Entry result;
	bzero(&result, sizeof(Entry));
	strcpy(result.domain, domain);

	for (int i = 0; i < MAX_DOMAIN; i++)
	{

    	if (strcmp(table[i].domain, domain) == 0)
    	{
        	for (int j = 0; j < MAX_ADDR; j++)
        	{
            	strcpy(result.address[j], table[i].address[j]);
        	}
        	break;
    	}
	}
	return result;
}

//#include "dns.h"

#define SA struct sockaddr
int main(int argc, char **argv)
{
    if (argc < 2)
    {
   	 fprintf(stderr, "Please pass port number of server as second argument!\n");
   	 exit(EXIT_FAILURE);
    }
    int PORT = atoi(argv[1]);

    Entry query;
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    char buff[30] = {0};

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
   	 fprintf(stderr, "Error: Socket creation failed!\n");
   	 exit(EXIT_FAILURE);
    }
    else
   	 printf("Socket creation successfull!\n");

    bzero(&servaddr, sizeof(servaddr));
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[2]);
    servaddr.sin_port = htons(PORT);
    int len = sizeof(Entry);
    while(1)
    {

   	 bzero(&query, sizeof(Entry));
   	 printf("Enter domain name: ");
   	 scanf(" %[^\n]", query.domain);

   	 if (strcmp(query.domain, "END") == 0)
   		 break;

   	 sendto(sockfd, query.domain, sizeof(query.domain), MSG_CONFIRM, (struct sockaddr *)&servaddr, sizeof(servaddr));
   	 recvfrom(sockfd, &query, sizeof(Entry), MSG_WAITALL, (struct sockaddr *)&servaddr, &len);

   	 if (!query.address[0][0])
   		 printf("No entry found in DNS!\n");
   	 else
   	 {
   		 printf("The requested IP Address is: \n");
   		 for (int i = 0; i < MAX_ADDR; i++)
   		 {
   			 if (!query.address[i][0])
   			 {
   				 printf("%s\n", query.address[i-1]);
   				 //printf("\n");
   			 }
   		 }
   		 
   	 }
    }

    close(sockfd);
}
