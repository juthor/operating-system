# Timer(One Thread)
### 1. Problem definition(I/O, function)
 * thread는 interval timer에 의해 매 1초마다 실행되도록 만든다.  
 * itimer를 만들고 tick=10msec를 한다.  
 * tick을 1씩 증가시킨다.  
- - -   
### 2. Method solved
#### __main문__ 
 ① signal(SIGALRM, alarm_handler)를 하여 SIGALRM이 들어오면 alarm_handler로 갈 수 있도록 set한다.  
 ② itimerval 구조체의 정해진 규정대로 delay.it_val delay.it_value.tv_sec=1, delay.it_value.tv_usec=0, delay.it_interval.tv_sec=0, delay.it_interval.tv_usec=10000을 설정한다.  
 ③ ret=setitimer(ITIMER_REAL, &delay, NULL)을 통해 ret로 if문을 통해 에러가 발생했는지 여부를 체크하여 에러가 발생했을 경우 perror("setitiemr")를 출력한다.  
 ④ pthread_create &time_triggered, NULL, (void *)time_thread, NULL)을 통해 thread를 생성한다.  
  
#### __alarm_handler__
  ① pthread_mutex_lock(&mutex)를 하여 스케쥴링이 일어나서 값이 바뀔 경우를 방지한다.
  ② ticks을 증가시키고 if문을 통해 만약 ticks가 100으로 나눈 나머지가 0이라면 1초가 방금 되었다는 의미이므로 ticks값을 0으로 초기화하고 pthread_cond_signal(&cond)를 통해 thread에게 1초가 지났다는 signal을 보내도록 한다.
  ③ pthread_mutex_unlock(&mutex)를 통해 lock을 풀고 signal(SIGALRM, alarm_handler)를 통해 다시 SIGALRM이 발생하면 alarm_handler로 갈 수 있도록 set한다.
  
 #### __time_thread__ 
 ① pthread_mutex_lock(&mutex)을 하여 스케쥴링이 일어나서 값이 바뀔 경우를 방지한다.  
 ② while(!pthread_cond_wait(&cond, &mutex))을 통해 time_t current_time, struct tm *current_date로 현재시간을 가져올 수 있도록 한다.  
 ③ char tmp[100]을 설정하고 time(&current_time)을 통해 현재 시간을 가져오도록 하고 current_date=localtime(&current_time), strcmp(tmp, asctime(current_date))로 tmp에 current_date를 넣도록 하였다.  
 ④ pthread_mutex_unlock을 해주고 다시 lock을 하여 현재 시간을 출력하고 다시 mutex를 unlock한다.  
 - - - 