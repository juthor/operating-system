#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

#define num_threads 10

typedef struct TCB
{
    int period;
    int time_left_to_invoke; //clock tick variable that keeps track of how many times if 0, wake up
    int thread_id;
    
}TCB;

pthread_cond_t cond_array[10]=PTHREAD_COND_INITIALIZER;//for 10 threads in maxs
pthread_mutex_t API_mutex=PTHREAD_MUTEX_INITIALIZER;//Mutex name is API_mutex, do initiaialize
struct TCB TCB_array[10];
timer_t timerid;

void posix_timer_handler(int signo, siginfo_t *info, void *context);
void time_triggered_thread();
void startclock();

int ticks=0;//Global variable
int myperiod=1000;
int myid=0;

void tt_thread_register(int mypriod, int myid)//Save myperiod, myid to TCB_array
{
    pthread_mutex_lock(&API_mutex);
    TCB_array[myid].period=myperiod;
    TCB_array[myid].thread_id=myid;
    TCB_array[myid].time_left_to_invoke=myperiod;
    pthread_mutex_unlock(&API_mutex);
}

int tt_thread_wait_invocation(int id_tmp)
{
    pthread_mutex_lock(&API_mutex);
    pthread_cond_wait(&cond_array[id_tmp], &API_mutex);
    pthread_mutex_unlock(&API_mutex);
    
    return 1;
}

void time_triggered_thread()
{
    tt_thread_register(myperiod, myid);
    int id_tmp=myid;
    myid++;
    myperiod+=1000;
    
    while(tt_thread_wait_invocation(id_tmp))
    {    
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
    pthread_t time_triggered[3];
    int ret, i;    

    startclock();
    for(i=0; i<3; i++)
        pthread_create(&time_triggered[i], NULL, (void *)time_triggered_thread, NULL);
    
    while(1)
    {
        pause();
    }
    
    return 0;
}

void startclock()
{
    struct itimerspec itval;
    struct itimerspec oitval;
    struct sigevent sigev;
    struct sigaction newact;
    
    //Signal handler setup for SIGTIMER
    sigemptyset(&newact.sa_mask);
    newact.sa_flags=SA_SIGINFO;
    newact.sa_sigaction=posix_timer_handler;
    sigaction(SIGRTMIN, &newact, NULL);

    //Timer set up
    sigev.sigev_notify=SIGEV_SIGNAL;
    sigev.sigev_signo=SIGRTMIN;
    sigev.sigev_value.sival_ptr=&timerid;
    timer_create(CLOCK_REALTIME, &sigev, &timerid);
    
    //Timer interval set up
    itval.it_value.tv_sec=0;
    itval.it_value.tv_nsec=(long)10*(1000000L);
    itval.it_interval.tv_sec=0;
    itval.it_interval.tv_nsec=(long)10*(1000000L);
    timer_settime(timerid, 0, &itval, &oitval);
    
}

void posix_timer_handler(int signo, siginfo_t *info, void *context)
{
    int i=0;
    int n_overrun;

    pthread_mutex_lock(&API_mutex);    

    n_overrun=timer_getoverrun(timerid); //get overrun
        
    if(n_overrun>=1)
    {
        printf("clock overrun %d \n", n_overrun);
        fflush(stdout);
    }        

    for(i=0; i<num_threads; i++)
    {
        TCB_array[i].time_left_to_invoke-=10;// because of 10msec
        if(TCB_array[i].time_left_to_invoke<=0)
        {
            TCB_array[i].time_left_to_invoke=TCB_array[i].period;
            pthread_cond_signal(&cond_array[i]);
        }
    }
    pthread_mutex_unlock(&API_mutex);
}