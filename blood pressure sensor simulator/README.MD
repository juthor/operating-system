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