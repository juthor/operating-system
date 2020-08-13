# Timer(Multi Thread)

### 1. Problem definition(I/O, function)  
 * timer-triggered thread을 통해 thread를 관리하는 시스템을 만든다.  
 * One thread Timer 코드 기반으로 문제를 해결한다.
 * pthread_condition cond_array[10]을 하고 pthread_mutex API_Mutex를 한다. struct TCB TCB_array[10]을 만들고 API tt_thread_register로 인자 period, thread_id를 가지는 thread 등록 함수를 만든다.  
  - 10msec를 주기로 하고 pthread_mutex_lock을 걸고 TCB[thread_id].period=period, TCB[thread_id].thread_id=thread_id, TCB[thread_id].time_left_to_invoke=period를 하고 pthread_mutex_unlock을 통해 Mutex를 풀어준다.  
  
### 2. Method solved  

#### __tt_thread_regiser__  
 ① 인자로 mypriod, myid를 가지고 먼저 mutex lock을 걸고 TCB_array[myid]에 변수들에 받은 인자들을 넣어주고 mutex unlock을 통해 mutex를 풀어준다.  
  
#### __tt_thread_wait_invocation__  
 ① mutex_lock을 걸고 pthread_cond_wait을 한 후에 mutex_unlock을 한다.

#### __time_triggered thread__  
 ① tt_thread_register에 myperiod, myid를 넣어서 TCB_array를 set하고 id_tmp에 myid를 넣어 값이 변할 것을 미리 고려하여 저장한다.  
 ② myid++, myperiod+=3000을 하고, current time을 출력한다.  

#### __main__  
 ① pthread_t time_triggered[3]을 하고 signal을 통해 SIGALRM이 발생했을 때, alarm_handler로 가도록 한다.  
  - struct itimerval delay를 통해 interval time을 정하고 for문을 통해 pthread를 create한 후, while로 pause하고 return 0을 한다.   

#### __alarm_handler__  
 ① mutex lock을 먼저 걸고 for문을 통해 TCB_array[i].time_left_to_invoke-=10, if문을 통해 TCB_array[i].time_left_to_invoke-=10이 0보다 작다면 TCB_array[i].period값을 넣어주고 signal을 보낸다.  
 ② mutex를 unlock하고 signal을 통해 SIGALRM이 발생했을시, alarm_handler로 가도록 set 한다.  
