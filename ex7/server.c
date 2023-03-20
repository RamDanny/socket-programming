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

struct packet
{
	char destip[20];
	char srcip[20];
	char srcmac[20];
};


void getIP(unsigned char ip_address[])
{

    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    memcpy(ifr.ifr_name, "enp3s0", IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    
    close(fd);

    
    strcpy(ip_address, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));  

 
}


void getMAC(unsigned char MACaddr[])
{
    struct ifaddrs *ifaddr=NULL;
    struct ifaddrs *ifa = NULL;
    int i = 0;

    if (getifaddrs(&ifaddr) == -1)
    {
         perror("getifaddrs");
    }
    else
    {
         for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
         {
             if ( (ifa->ifa_addr) && (ifa->ifa_addr->sa_family == AF_PACKET) )
             {
                  struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
	   sprintf(MACaddr,"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",s->sll_addr[0],s->sll_addr[1],s->sll_addr[2],s->sll_addr[3],s->sll_addr[4],s->sll_addr[5]);                  
             }
         }
         freeifaddrs(ifaddr);
    }
    
}


int main(int argc,char **argv)
{
	int len;
	int sockfd,newfd,n;
	struct packet *p = malloc(sizeof(struct packet));
	struct sockaddr_in servaddr,cliaddr;
	char destmac[20];
	char data[20];
	pid_t child;

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
		perror("cannot create socket");
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=INADDR_ANY;
	servaddr.sin_port=htons(atoi(argv[1]));
	if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		perror("Bind error");
	listen(sockfd,2);
	len=sizeof(cliaddr);
	printf("Enter the details of the ARP Request\n");
	printf("Destination IP : ");
	scanf("%[^\n]%*c",p->destip);
	printf("Source IP : ");
	getIP(p->srcip);
	printf("%s",p->srcip);
	printf("\nSource MAC : ");
	getMAC(p->srcmac);
	printf("%s",p->srcmac);
	
	printf("\n\nARP Request packet is broadcasted");
	printf("\nWaiting for reply...\n");
	while(1)
	{
		newfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
		if(newfd < 0)
			break;
		child = fork();
		if(child == 0)
		{
			n = send(newfd, p, sizeof(*p),0);
			if(recv(newfd,destmac, sizeof(destmac), 0) > 0)
			{
				printf("\nARP Reply Received from %s\n", destmac);
				
				printf("Data to send: ");
				scanf("%[^\n]%*c", data);
				
				printf("\nSending data...\n");
				
				n = send(newfd, p, sizeof(*p), 0);
				n = send(newfd, data, sizeof(data), 0);
				break;
			}
		}
	}
	close(sockfd);
	close(newfd);
	return 0;
}
