#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/time.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

pthread_t time_triggered;//Thread name is time_triggered
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;//Mutex name is mutex, do initiaialize
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;//Condition name is cond

void alarm_handler(int signo);
int ticks=0;//Global variable

void time_thread()
{    
    pthread_mutex_lock(&mutex);//mutex lock    
    while(!pthread_cond_wait(&cond, &mutex))
    {
        time_t current_time;
        struct tm *current_date;
        char tmp[100];
    
        time(&current_time);//Get the current time
        current_date=localtime(&current_time);
        strcpy(tmp, asctime(current_date));//Insert the current_day to tmp
        
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        printf("%s", tmp);//print current time
        pthread_mutex_unlock(&mutex);
    }
}


int main(void)
{
    //set the SIGALRM handler
    //make a condition variable and initialize it
    //create the time-triggered thread;
    //create an itimer(tick=10 msec)
    
    struct itimerval delay;
    
    int ret;
    signal(SIGALRM, alarm_handler);//if SIGALRM have occur, go to alarm_handler
    delay.it_value.tv_sec=1;
    delay.it_value.tv_usec=0;
    delay.it_interval.tv_sec=0;
    delay.it_interval.tv_usec=10000;//10msec
    
    ret=setitimer(ITIMER_REAL, &delay, NULL);
    if(ret)
        perror("setitiemr");    

    pthread_create(&time_triggered, NULL, (void *)time_thread, NULL);
    
    while(1)
    {
        pause();
    }
    
    return 0;
}

void alarm_handler(int signo)
{
    pthread_mutex_lock(&mutex);
    ticks++;

    if(ticks%100==0)//now a second has just passed
    {
        ticks=0;//Initialize to 0
        pthread_cond_signal(&cond);//now 1 second has changed, sending a signal cond
    }
    pthread_mutex_unlock(&mutex);
    signal(SIGALRM, alarm_handler);//set a user defined signal handler
}