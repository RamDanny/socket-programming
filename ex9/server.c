# include <stdio.h>
# include <netdb.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
# include <math.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# define SA struct sockaddr

char *strrev(char s[],char r[])
{
   int begin, end, count = 0;
   while (s[count] != '\0')
      count++;
   end = count - 1;
   for (begin = 0; begin < count; begin++)
   {
      r[begin] = s[end];
      end--;
   }
   r[begin] = '\0';
}

void main(int argc, char *argv[])
{
	struct sockaddr_in ser,cli;
	int connfd;

	char buf[200];
	
	if(argc<2)
	{
		printf("\nUsage: ./client <port no>");
		exit(1);
	}

	int PORT = atoi(argv[1]);

	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		printf("\nSocket error\n");
		exit(0);
	}

	// assign IP, PORT
	ser.sin_family = AF_INET;
	ser.sin_addr.s_addr = htonl(INADDR_ANY);
	ser.sin_port = htons(PORT);

	if ((bind(sockfd, (SA*)&ser, sizeof(ser))) != 0)
	{
		printf("\nSocket bind error\n");
		exit(0);
	}

	// Now server is ready to listen and verify
	if ((listen(sockfd, 5)) != 0)
	{
		printf("\nListen error\n");
		exit(0);
	}
	else
		printf("\nListening..\n");
	int ln = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &ln);
	if (connfd < 0)
	{
		printf("\nAccept error\n");
		exit(0);
	}
    
	read(connfd, buf, sizeof(buf));
	printf("\nFrom client: %s\n",buf);

    char decode[200];
    strrev(buf,decode);
    int len = strlen(decode);
    int r = 0, m;
    while ((int)pow(2,r) < len+1)
        r++;
    m = len-r;
    int parity[30] = {0};
    for (int l = 0; l < r; l++) {
        int j = pow(2,l);
        for (int i = j - 1; i < len; i = i + 2*j) {
            for (int k = 0; k < j && i + k < len; k++) {
                if (decode[i+k] == '1') {
                    parity[l] = parity[l] ^ 1;
                }
            }
        }
    }
    int val=0;
    for(int i=0;i<r;i++){
        val+=(int)pow(2,i)*parity[i];
    }
    if(val!=0){
        printf("\nError is at %d\n",val);
        buf[len-val]=(buf[len-val]=='1'?'0':'1');
    }
	printf("\nCorrect encoded message from client:%s\n",buf);
	close(sockfd);
}
