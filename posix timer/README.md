# POSIX Timer

### 1. Problem definition(I/O, function)
 * itimer 대신 POSIX Timer를 이용하여 timer-triggered thread을 통해 thread를 관리하는 시스템을 만든다.  
 * time-triggered thread 함수를 직접 만들어서 현재 시간을 출력하도록 한다.
- - - 
### 2. Method solved  
 * __main 함수__  
 ① main함수에서는 pthread_t time_triggered[3];을 하고 startclock()함수를 호출하였다.  
 ② for문을 통해 0부터 3보다 작을 때까지 반복하고, pthread_create(&time_triggered[i], NULL, (void *)time_triggered_thread, NULL)을 통해 thread를 생성하도록 하였다.  
 ③ while문을 통해 pause()를 하도록 하고 return 0을 한다.  

 * __startclock함수__  
 ① struct itimerspec itval, struct itimerspec oitval, struct sigevent sigev, struct sigaction newact를 바탕으로 clock을 setting하기 위해 사용한다.  
 ② SIGTIMER를 위해 signal handler를 setup하고, TIMER를 setup한 후 Timer interval set up과정을 통해 start clock함수를 끝낸다.  

 * __posix_timer_handler 함수__  
 ① n_overrun을 통해 overrun이 발생하면 개수를 카운트하기 위한 변수를 설정한다.  
 ② mutex lock을 먼저 걸고 n_overrun=timer_getoverrun(timerid)를 통해 overrun된 개수를 카운트하고 if문을 통해 만약 overrun이 1보다 크다면 printf("clock overrun %d \n", n_overrun)을 통해 출력하도록 하였다.   
 ③ for문을 i가 0부터 thread수보다 작을 때까지 TCB_array[i].time_left_to_invoke-=10을 하고 if문을 통해 TCB_array[i].time_left_to_invoke이 0보다 작거나 같다면, TCB_array[i].time_left_to_invoke=TCB_array[i].period을 하고, cond_arrun[i]에 signal을 보내도록 하였다.  
 ④ for문이 끝났으면 mutex를 unlock하도록 하였다.  

 * __time_triggered_thread__  
 ① while(tt_thread_wait_invocation(id_tmp))을 통해 tt_thread_wait_invocation이 끝나고 return 1을 해서 반환된다면 while문안이 실행되기 시작한다.  
 ② mutex_lock(&API_mutex)를 걸고 time_t current_time, struct tm *current_date로 현재시간을 가져올 수 있도록 한다.  
 ③ char tmp[100]을 설정하고 time(&current_time)을 통해 현재 시간을 가져오도록 하고 current_date=localtime(&current_time), strcmp(tmp, asctime(current_date))로 tmp에 current_date를 넣도록 한다.  
 ④ time(&current_time), current_date=localtime(&current_time)을 통해 얻은 값을 바탕으로 strcpy(tmp, asctime(current_date)) tmp에 복사한다.  
 ⑤ id_tmp와 tmp를 바탕으로 현재 시간을 출력하고 다시 mutex를 unlock한다.  