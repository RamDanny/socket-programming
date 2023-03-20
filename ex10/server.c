#include <stdio.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

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
	printf("+-----------------+----------------------+\n");
	printf("|   Domain Name   |    	Address   	|\n");
	printf("+-----------------+----------------------+\n");
	for (int i = 0; i < MAX_DOMAIN; i++)
	{
    	if (table[i].domain[0])
    	{
        	printf("| %-15s | %-20s |\n", table[i].domain, table[i].address[0]);

        	for (int j = 1; j < MAX_ADDR && table[i].address[j][0]; j++)
            	printf("| %-15s | %-20s |\n", "", table[i].address[j]);
        	printf("+-----------------+----------------------+\n");
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

int main(int argc, char **argv)
{

    Entry table[MAX_DOMAIN], result;
    bzero(table, MAX_DOMAIN * sizeof(Entry));

    if (argc < 2)
    {
   	 fprintf(stderr, "Error: Enter port number for server as second argument!\n");
   	 exit(EXIT_FAILURE);
    }

    int PORT = atoi(argv[1]);
    int sockfd, len;
    struct sockaddr_in servaddr, cliadrr;
    char buff[30];
    int n;

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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
   	 fprintf(stderr, "Error: Socket bind failed!\n");
   	 exit(EXIT_FAILURE);
    }
    else
   	 printf("Socket bind successfull\n");

    len = sizeof(cliadrr);

    createEntry(table, "google.com", "192.168.1.1");
    createEntry(table, "yahoo.com", "194.12.34.12");
    createEntry(table, "nesoacademy.com", "17.10.23.123");

    printTable(table);

    string domain, address, opt;

    while (1)
    {
   	 recvfrom(sockfd, buff, sizeof(buff), MSG_WAITALL, (struct sockaddr *)&cliadrr, &len);
   	 result = getAddress(table, buff);
   	 sendto(sockfd, &result, sizeof(Entry), MSG_CONFIRM, (struct sockaddr *)&cliadrr, len);

   	 int flag = 0;

   	 printf("Do you want to alter table (y/n): ");
   	 scanf("%s", opt);
   	 if (strcmp(opt, "y") == 0)
   	 {
   		 printf("Enter domain: ");
   		 scanf("%s", domain);
   		 do
   		 {
   			 printf("Enter IP address: ");
   			 scanf("%s", address);
   			 flag = createEntry(table, domain, address);
   			 switch (flag)
   			 {
   			 case 1:
   				 break; // Correct IP
   			 case -1:
   				 printf("Invalid IP address!\n");
   				 break;
   			 case -2:
   				 printf("Duplicate IP address!\n");
   				 break;
   			 default:
   				 printf("Error!\n");
   			 }
   		 } while (flag != 1);

   		 printf("Updated table:\n");
   		 printTable(table);
   	 }
    }

    close(sockfd);
}
