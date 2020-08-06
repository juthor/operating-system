#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

#define num_threads 3

typedef struct TCB
{
    int period;
    int time_left_to_invoke;
    int thread_id;
    
}TCB;

pthread_cond_t cond_array[10]=PTHREAD_COND_INITIALIZER;//for 10 threads in maxs
pthread_mutex_t API_mutex=PTHREAD_MUTEX_INITIALIZER;//Mutex name is API_mutex, do initiaialize
struct TCB TCB_array[10];

void alarm_handler(int signo);
void time_triggered_thread();


int ticks=0;//Global variable
int myperiod=3000;
int myid=0;

void tt_thread_register(int mypriod, int myid)
{
    pthread_mutex_lock(&API_mutex);
    TCB_array[myid].period=myperiod;
    TCB_array[myid].thread_id=myid;
    TCB_array[myid].time_left_to_invoke=myperiod;
    pthread_mutex_unlock(&API_mutex);
}

void tt_thread_wait_invocation(int id_tmp)
{
    pthread_mutex_lock(&API_mutex);
    pthread_cond_wait(&cond_array[id_tmp], &API_mutex);
    pthread_mutex_unlock(&API_mutex);
}

void time_triggered_thread()
{
    tt_thread_register(myperiod, myid);
    int id_tmp=myid;
    myid++;
    myperiod+=3000;
    
    while(1)
    {    
        pthread_mutex_unlock(&API_mutex);
        tt_thread_wait_invocation(id_tmp);
        pthread_mutex_lock(&API_mutex);
    
        char tmp[100];
        time_t current_time;
        struct tm *current_date;

        time(&current_time);//Get the current time
        current_date=localtime(&current_time);
        strcpy(tmp, asctime(current_date));//Insert the current_day to tmp
        
        printf("threadid %d  %s", id_tmp, tmp);//print current time
        pthread_mutex_unlock(&API_mutex);
    }
}

int main(void)
{
    struct itimerval delay;
    pthread_t time_triggered[3];
    int ret, i;
    
    signal(SIGALRM, alarm_handler);//if SIGALRM have occur, go to alarm_handler
    delay.it_value.tv_sec=1;
    delay.it_value.tv_usec=0;
    delay.it_interval.tv_sec=0;
    delay.it_interval.tv_usec=10000;//10msec
    ret=setitimer(ITIMER_REAL, &delay, NULL);
    if(ret)
        perror("setitiemr");    

    for(i=0; i<3; i++)
        pthread_create(&time_triggered[i], NULL, (void *)time_triggered_thread, NULL);
    
    while(1)
    {
        pause();
    }
    
    return 0;
}


void alarm_handler(int signo)
{
    int i=0;
    
    pthread_mutex_lock(&API_mutex);    
    for(i=0; i<num_threads; i++)
    {
        TCB_array[i].time_left_to_invoke-=10;
        if((TCB_array[i].time_left_to_invoke-=10)<=0)
        {
            TCB_array[i].time_left_to_invoke=TCB_array[i].period;
            pthread_cond_signal(&cond_array[i]);
        }
    }
    pthread_mutex_unlock(&API_mutex);
    signal(SIGALRM, alarm_handler);//set a user defined signal handler
}