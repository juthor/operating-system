# Blood Pressure Sensor Simulator

### 1. Problem definition(I/O, function)  
* __Data_acquistion_thread__  
① Data_acquistion_thread는 첫 번째 thread로 blood-pressure sensor simulator를 하고 주기적으로(100Hz)마다 실행된다.   
② 실행이 될 때 turn을 2로나눈 나머지 값이 0이라면 bp[60, 90]범위의 random number를 생성하고 diastolic blood pressure를 의미한다. 아니면 나머지 값이 1이라면 [110, 150]사이의 random number를 생성하여 systolic bp를 만든다.  
③ bp는 bp-queue에 넣고 이건 critical section이 된다. 매 1/10초마다 bp_brocessing_thread에 signal을 보내서 깨워주고 turn값을 증가시켜 systolic을 diastolic이 되거나 diastolic이 systolic이 되도록 한다.  
  
* __Bp_processing_thread__  
① Bp_processing_thread는 두 번째 thread로 생성한다.  
② turn의 초기값은 0으로 local변수이고 처음은 diastolic이다.  
③ Data_acquistion_thread에서 signal을 받을 때까지 waiting loop을 돌도록 한다.  
④ bp-queue에 있는 모든 값을을 빼서 avg_bp값을 계산하고 diastolic avg_bp와 systolic avg_bp를 출력하도록 하고 turn값을 증가시켜 systolic을 diastolic이 되거나 diastolic이 systolic이 되도록 한다.  
  
* __POSIX timer signal handler__  
① main문에서 POSIX timer를 생성한다.(period=100Hz)  
② Data_acquistion_thread에 시그널을 보내 1/100 sec마다 깨워준다.  

- - -

### 2. Method solved  
 * __main 함수__  
 ① pthread_t time_triggered[2]를 하고 startclock()함수를 호출하였다.  
 ② pthread_create(&time_triggered[0], NULL, (void *)Data_acquisition_thread, NULL)와 pthread_create(&time_triggered[1], NULL, (void *)Bp_processing_thread, NULL)을 통해 Data_acquistion_thread와 Bp_processing_thread를 생성한다.  
 ③ while문을 통해 pause()를 하도록 하고 return 0을 한다.  
  
 * __startclock함수__  
 ① struct itimerspec itval, struct itimerspec oitval, struct sigevent sigev, struct sigaction newact를 바탕으로 clock을 setting하기 위해 사용한다.  
 ② SIGTIMER를 위해 signal handler를 setup하고, TIMER를 setup한 후 Timer interval set up과정을 통해 start clock함수를 끝낸다.  
  
 * __posix_timer_handler 함수__  
 ① int n_overrun을 통해 overrun이 발생하면 개수를 카운트하기 위한 변수를 설정한다.  
 ② mutex lock을 먼저 걸고 n_overrun=timer_getoverrun(timerid)를 통해 overrun된 개수를 카운트하고 if문을 통해 만약 overrun이 1보다 크다면 printf("clock overrun %d \n", n_overrun)을 통해 출력한다.   
 ③ if문을 사용해서 TCB_array[0].time_left_to_invoke가 0보다 작거나 같으면 TCB_array[0].time_left_to_invoke=TCB_array[0].period을 하고,  pthread_cond_signal(&wake_API)을 통해 시그널을 보낸다.  
 ④ 끝났으면 mutex를 unlock한다.  
  
 * __Data_acquisition_thread__  
 ① tt_thread_register(myperiod, myid)를 통해 현재 thread를 등록한다.  
 ② while(tt_thread_wait_invocation(id_tmp))을 통해 tt_thread_wait_invocation이 끝나고 return1을 해서 반환된다면 while문안이 실행된다.  
 ③ mutex_lock(&global_mutex)를 걸고 전역변수인 in값을 증가시키고 in=in%1000을 한 후, count_wait을 증가한다.  
 ④ if문으로 turn%2가 0과 같다면, r1=(rand()%31+60)을 통해 diastolic값을 랜덤으로 받고 bp_q[in]=r1를 통해 bp_q에 생성된 값을 집어넣는다.  
 ⑤ 아니면 r1=(rand()%41+110)을 통해 systolic값인 110부터 150사이의 값을 random하게 뽑아서 r1에 넣고 bp_q에 생성된 값을 집어넣는다.  
 ⑥ if문을 써서 만약 count_wake==10이면 10번이 반복되었으니까 0.1초가 지났다는 의미로 Bp_processing_thread를 깨워줘야 하므로 pthread_cond_signal(&wake_bp)을 통해 signal을 보내고 count_wake=0을 통해 초기화하고 turn 값을 1증가시킨다.  
 ⑦ 그리고 pthread_mutex_unlock(&global_mutex)을 통해 lock을 풀어준다.  
  
 * __Bp_proessing_thread__  
 ① while문안에 pthread_mutex_lock(&global_mutex),  pthread_cond_wait(&wake_bp, &global_mutex)을 하고 avg_bp=0, i=0으로 초기화한다.  
 ② for문을 통해 0부터 10보다 작을 때까지 반복하도록 하고 out을 증가시키고 out=out%1000을 한 후 avg_bp+=bp_q[out]을 통해 agv_bp의 값을 구하기 위해 10개씩 뽑아낸다.  
 ③ avg_bp=avg_bp/10을하고 time(&current_time)을 통해 현재 시간을 가져오도록 하고 current_date=localtime(&current_time), strcmp(tmp, asctime(current_date))로 tmp에 current_date를 넣는다.  
 ④ time(&current_time), current_date=localtime(&current_time)을 통해 얻은 값을 바탕으로 strcpy(tmp, asctime(current_date)) tmp에 복사한다.  
 ⑤ if문을 서서 turn%2가 0이라면 diastolic이라는 문구와 avg_bp, tmp를 바탕으로 현재 시간을 출력하고 아니면 systolic이라는 문구와 avg_b, tmp를 바탕으로 현재 시간을 출력한다.  
 ⑦ 그리고 turn값을 증가시키고 avg_bp값을 0으로 초기화하rh mutex를 unlock한다.  