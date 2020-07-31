#include<stdlib.h>
#include<stdio.h>
#include<unistd.h> // pid_t type, contains fork functions
#include<sys/types.h>
#include<sys/wait.h>


int main(void)
{
    int pid1, pid2;//child pid1, pid2, pid1 is parent of pid2
    int i, EOD=10;
    char char_string[10], chan_up[10], output[10];
    
    int fd1[2], fd2[2];
    
    pipe(fd1);
    pipe(fd2);
    if((pid1=fork())==0)//child1
    {
        close(fd1[1]);//Don't use pipe1, fd1[1] write channel
            
        
        read(fd1[0], chan_up, EOD);
            
        for(i=0; i<EOD; i++)//Change capital letter, if small letter
        {
            if(chan_up[i]>='a' && chan_up[i]<='z')
                chan_up[i]=chan_up[i]-32;
        }
        //printf("chan_up parent: %s \n", chan_up);
        write(fd2[1], chan_up, EOD);
        
        close(fd2[1]);
        close(fd1[0]);
                
        
        exit(0);//pid1 child die
    }

    
    if((pid2=fork())==0)//child2
    {
        close(fd2[1]);//Don't use fd2[1], write channel        
        
        read(fd2[0], output, EOD);
        printf("Output: %s \n", output);//Print output
    
        close(fd2[0]);
        exit(0);//pid2 child die
    }
    
    
    else//parent
    {
        close(fd1[0]);//Don't use pipe1, fd1[0] read channel
        printf("Input the char character: ");
        scanf(" %[^\n]s", char_string);
        write(fd1[1], char_string, EOD);//Write pipe1, fd1[1] write channel
        
        wait(&pid1);//Wait until child die, pid1
        wait(&pid2);//Wait until child die, pid2
    }
    return 0;
} 
