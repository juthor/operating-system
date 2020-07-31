#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<pwd.h>

void execute(char **argv)
{
    pid_t pid;
    int status;
    int fd, i=0;
    int background=0, re_dir=0;
    char file_name[100];

    for(i=0; argv[i]!=0; i++)
    {
        if(strcmp(argv[i], ">")==0)//if argv[i] is same ">", redirection
        {
            argv[i]=NULL;
            re_dir=1;
        }

        else if(strcmp(argv[i], "&")==0)//if argv[i] is same "&", background
        {
            argv[i]=NULL;
            background=1;
        }
    }
    
    if((pid=fork())==0)//Child process
    {
        if(re_dir==1)//If re_dir==1, redirection
        {    
            fd=open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            //file name is argv[2]            
            dup2(fd, 1);//All output are directed to the fd, not 1(stdout)
            close(fd);
        }

        if(execvp(argv[0], argv)<0)
        {
            printf("ERROR exec failed\n");
            exit(1);
        }
    }

    if(background==0)//foreground
        while (wait(&status)!=pid) ;

    if(background==1)//background
        waitpid(WNOHANG, &status, 0);
}


int main(void)
{
    char input_line[100];//prompt input line 
    char *argv[100];
    int argc=0;
    char *ptr;
    int ndx=0;//index

    while(getlogin())
    {        
        printf("[%s]$", get_current_dir_name());
            gets(input_line);//input input_line
        fflush(stdout);

        ptr=NULL;
        ndx=0;
        ptr=strtok(input_line, " ");//input_line seperate using " "
        
        while(ptr!=NULL)
        {
            argv[ndx]=ptr;
            ptr=strtok(NULL, " ");
            ndx++;
        }
        argv[ndx]=NULL;
        argc=ndx;

        if(strcmp(argv[0], "exit")==0)
            exit(0);
    
        execute(argv);
    }

    return 0;
}
