#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<strings.h>

#define MAX 1024//max Byte input length
#define PORT 30000//server port number
#define HOSTADDR "192.168.124.1"

int main(int argc, char *argv[])
{
    int sd, send_bytes, n, recv_bytes;//socket descriptor
    struct sockaddr_in servaddr;
    char snddata[MAX], rcvdata[MAX];//send data and receive data is message buffer

    bzero((char*)&servaddr, sizeof(servaddr));//Prepare server address, port and bzero is same memset
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(HOSTADDR);//change 32bit ip address
    servaddr.sin_port=htons(PORT);//host to network short

    if((sd=socket(AF_INET, SOCK_STREAM, 0))<0)//TCP=SOCK_STREAM
    {
        fprintf(stderr, "can't open socket \n");
        exit(1);
    }

    if(connect(sd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0)
    //connect to server(client calls server), Auto binding client addr, port
    {
        fprintf(stderr, "can't connect to server. \n");
        exit(1);
    }

    while(fgets(snddata, MAX, stdin)!=NULL)//get a string max size is max
    {
        send_bytes=strlen(snddata);

        if(send(sd, snddata, send_bytes, 0)!=send_bytes)//to server
        {
            fprintf(stderr, "can't send data \n");
            exit(1);
        }

        recv_bytes=0;//clear
        while(recv_bytes<send_bytes)//this loop need to stream I/O
        {
            if((n=recv(sd, rcvdata+recv_bytes, MAX, 0))<0)//from server
            {
                fprintf(stderr, "can't receive data \n");
                exit(1);            
            }
            recv_bytes+=n;
        }

        rcvdata[recv_bytes]=0;//NULL char for string
        fputs(rcvdata, stdout);//display
    }

    close(sd);
    return 0;
} 