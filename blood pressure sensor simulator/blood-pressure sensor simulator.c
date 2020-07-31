#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

typedef struct TCB
{
    int period;
    int time_left_to_invoke; //clock tick variable that keeps track of how many times if 0, wake up
    int thread_id;
    
}TCB;

pthread_cond_t wake_data=PTHREAD_COND_INITIALIZER;
pthread_cond_t wake_bp=PTHREAD_COND_INITIALIZER;
pthread_cond_t wake_API=PTHREAD_COND_INITIALIZER;

pthread_mutex_t API_mutex=PTHREAD_MUTEX_INITIALIZER;//Mutex name is API_mutex, do initiaialize
pthread_mutex_t global_mutex=PTHREAD_MUTEX_INITIALIZER;//Mutex name is global_mutex, do initiaialize


struct TCB TCB_array[1];
timer_t timerid;

void posix_timer_handler(int signo, siginfo_t *info, void *context);
void Data_acquisition_thread();
void Bp_processing_thread();
void startclock();

int ticks=0;//Global variable
int myperiod=1000;
int myid=0;
int bp_q[1000];
int in=-1, out=-1;

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
    pthread_cond_wait(&wake_API, &API_mutex);
    pthread_mutex_unlock(&API_mutex);

    return 1;
}

void Data_acquisition_thread()
{
    tt_thread_register(myperiod, myid);
    int id_tmp=myid;
    int r1, r2;
    int i=0, turn=0;
    int count_wake=0;
    char tmp[100];
    
    while(tt_thread_wait_invocation(id_tmp))
    {    
        pthread_mutex_lock(&global_mutex);
        in++;
        in=in%1000;
        count_wake++;
        
        if(turn%2==0)
        {
            r1=(rand()%31+60);//bp//random, bp[60, 90]between//diastolic
            bp_q[in]=r1;
        }
        else
        {
            r1=(rand()%41+110);//random bp[110, 150]between systolic
            bp_q[in]=r1;
        }

        if(count_wake==10)        
        {    
            pthread_cond_signal(&wake_bp);
            count_wake=0;            
            turn++;
        }
        pthread_mutex_unlock(&global_mutex);
    }
}

void Bp_processing_thread()
{
    int turn=0;
    int avg_bp=0;

    char tmp[100];
    time_t current_time;
    struct tm *current_date;

    while(1)
    {
        pthread_mutex_lock(&global_mutex);
        pthread_cond_wait(&wake_bp, &global_mutex);

        avg_bp=0;
        int i=0;
        for(i=0; i<10; i++)
        {
            out++;
            out=out%1000;
            avg_bp+=bp_q[out];    
            
        }
        avg_bp=avg_bp/10;

        time(&current_time);//Get the current time
        current_date=localtime(&current_time);
        strcpy(tmp, asctime(current_date));//Insert the current_day to tmp
        if(turn%2==0)
             printf("diastolic bp=%d %s", avg_bp, tmp);//diastolic
        else 
             printf("systolic bp=%d %s", avg_bp, tmp);//systolic

        turn++;
        avg_bp=0;
        //pthread_cond_signal(&wake_data);
        pthread_mutex_unlock(&global_mutex);
    }
}


int main(void)
{
    pthread_t time_triggered[2];
    int ret, i;

    startclock();
    pthread_create(&time_triggered[0], NULL, (void *)Data_acquisition_thread, NULL);
    pthread_create(&time_triggered[1], NULL, (void *)Bp_processing_thread, NULL);


    
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
        
    TCB_array[0].time_left_to_invoke-=1000;
    n_overrun=timer_getoverrun(timerid); //get overrun
    
    if(n_overrun>=1)
    {
        printf("clock overrun %d \n", n_overrun);
        fflush(stdout);
    }        
    
    if(TCB_array[0].time_left_to_invoke<=0)
    {
        TCB_array[0].time_left_to_invoke=TCB_array[0].period;
        pthread_cond_signal(&wake_API);
    }

    pthread_mutex_unlock(&API_mutex);
}