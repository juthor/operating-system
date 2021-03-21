#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include<signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>



struct shmbuf
{
    sem_t num_space;//semaphore, num_space
    sem_t num_data;//semaphore, num_data
    int buffer[100];//save data at buffer and bring data from buffer
    int in, out;
};


int main(void)
{
    int pid1, pid2, output;
    int fd;
    const char *shm_name="/shmp";
    struct shmbuf *shmp;

    //pid1=fork();
    if((pid1=fork())==0)//child 1, initialize
    {    
        shm_unlink(shm_name);
        fd=shm_open(shm_name, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
        ftruncate(fd, sizeof(struct shmbuf));
       
        shmp=mmap(NULL, sizeof(struct shmbuf), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, 0);          
        sem_init(&shmp->num_data, 1, 0);//unnamed semaphore
        sem_init(&shmp->num_space, 1, 100);//unnamed semaphore
        shmp->in=-1;//initialize in variable
        shmp->out=-1;//initialize out variable
        exit(0);//pid1 child die
    }
    
    wait(&pid1);
    if((pid2=fork())==0)//child 2, producer
    {
        int i;
        fd=shm_open(shm_name, O_RDWR, 0); 
        shmp=mmap(NULL, sizeof(struct shmbuf), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, 0);            
        for(i=0; i<1000; i++)
        {
            sem_wait(&shmp->num_space);

            shmp->in++;
            shmp->in=shmp->in%100;
            shmp->buffer[shmp->in]=i;
            printf("producer[%d]=%d \n", shmp->in, i);
            sem_post(&shmp->num_data);      
        }
        exit(0);//pid2 child die
    }
    
    
    else//parent, consumer
    {
        int j;
        fd=shm_open(shm_name, O_RDWR, 0);
        shmp=mmap(NULL, sizeof(struct shmbuf), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, 0);
                
        for(j=0; j<1000; j++)
        {
            sem_wait(&shmp->num_data);

            shmp->out++;
            shmp->out=shmp->out%100;
            output=shmp->buffer[shmp->out];       
   
            sem_post(&shmp->num_space);     
            printf("    consumer[%d]=%d \n", shmp->out, output);
        }
        wait(&pid2);

        sem_destroy(&shmp->num_space);
        sem_destroy(&shmp->num_data);

        shm_unlink(shm_name);
        munmap((void *)shmp, sizeof(struct shmbuf));//memory release            
    }        
    return 0;
}
