#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<strings.h>
#include<pthread.h>

#define MAX 1024//max client data length
#define PORT 30000//server port number
#define BACKLOG 5//queue length
#define THREAD_MAX 5//possible making number of thread, max is 5

void *threadfunc(void *arg);

int result=0;
int count_cli=0;//client count

pthread_t thread[THREAD_MAX];


int main(int argc, char *argv[])
{
    int sd, bytes, cliaddsize;
    struct sockaddr_in cliaddr, servaddr;
    char data[MAX];
    int accp_sock[THREAD_MAX];
    int stat;
    
    
    if((sd=socket(AF_INET, SOCK_STREAM, 0))<0)//TCP=SOCK_STREAM
    {
        fprintf(stderr, "can't open socket \n");
        exit(1);
    }

    //bind the server itself to the socket
    bzero((char *)&servaddr, sizeof(servaddr));//Prepare server address, port and bzero is same memset
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(PORT);//host to network short port
    
    if(bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        fprintf(stderr, "can't bind to socket \n");
        exit(1);
    }//bind iftelf to the socket

    
    
    while(1)//server waiting loop
    {
        listen(sd, BACKLOG);//Declare the client-queue length
        cliaddsize=sizeof(cliaddr);
        accp_sock[count_cli]=accept(sd, (struct sockaddr *)&cliaddr, &cliaddsize);
    
        if(accp_sock[count_cli]<0)        
        {
            fprintf(stderr, "can't accept connection \n");
            exit(1);
        }
        
        if((stat=pthread_create(&thread[count_cli], NULL, &threadfunc, (void *) &accp_sock[count_cli]))!=0)
        {
            fprintf(stderr, "can't fork process \n");
            exit(1);
        }
        count_cli++;
        if(count_cli==6)
            break;
    }

    return 0;
}


void *threadfunc(void *arg)
{

    int bytes;
    char data[MAX];
    int nsd=(int) *((int *)arg);
    while(1)
    {
        bytes=recv(nsd, data, MAX, 0);
        if(bytes==0)
            break;
                
        else if(bytes<0)
        {
            fprintf(stderr, "can't receive data \n");
            exit(1);
        }
                
        if(send(nsd, data, bytes, 0)!=bytes)
        {
            fprintf(stderr, "can't send data \n");
            exit(1);
        }
    }
    close(nsd);
    
}
