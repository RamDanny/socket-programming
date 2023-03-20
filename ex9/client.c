# include <netdb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <unistd.h>
# include <math.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# define SA struct sockaddr

void strrev(char s[],char r[])
{    
   int begin, end, count = 0;
   while (s[count] != '\0')
      count++;
   end = count - 1;
   for (begin = 0; begin < count; begin++) {
      r[begin] = s[end];
      end--;
   }
   r[begin] = '\0';
}

void main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	
	if(argc<2)
	{
		printf("\nUsage: ./cli ip port_no\n");
		exit(1);
	}

	int PORT = atoi(argv[2]);
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		printf("\nSocket error\n");
		exit(0);
	}
	
	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(PORT);

	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
	{
		printf("\nConnection error\n");
		exit(0);
	}

    char buf[200];
    printf("\nEnter message:");
    scanf("%s", buf);
    char code[200];
    int r = 0, m = strlen(buf);
    while ((int)pow(2,r) < m+r+1)
        r++;
    int len=m+r;
    for (int i=0, j=m-1; i<len; i++){
        if (ceil(log2(i+1)) == floor(log2(i+1)))
            code[i]='r';
        else{
            code[i]=buf[j];
            j--;
        }
    }
    code[len]='\0';
    printf("%s\n", code);
    
    int parity[30]={0};
    for (int l=0; l<r; l++){
        int j=pow(2,l);
        for (int i=j-1; i<len; i=i+2*j){
            for (int k=0; k<j && i+k<len; k++){
                if (code[i+k]!='r' && code[i+k]=='1'){
                    parity[l] = parity[l] ^ 1;
                }
            }
        }
    }

    for (int i=0, j=0; i<len; i++){
        if (code[i] == 'r'){
            code[i] = '0' + parity[j++];
        }
    }
    char encode[200];
    strrev(code,encode);
    printf("%s\n",encode);
    int ch;
    printf("\nDo you want to include error:1)yes 2)no:");
    scanf("%d",&ch);
    if(ch==1){
        int ind;
        printf("\nEnter index(from right,starting with 1):");
        scanf("%d",&ind);
        encode[len-ind]=(encode[len-ind]=='1'?'0':'1');
    }
	write(sockfd, encode, sizeof(encode));
	close(sockfd);
}
