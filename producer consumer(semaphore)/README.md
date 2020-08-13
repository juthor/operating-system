# Producer Consumer(Semaphore)
### 1. Problem definition(I/O, function)
 * Shared memory와 Semaphore을 이용하여 Consumer&Producer 문제를 해결한다.  
 * 첫 번째 프로세스는 Semaphore와 Shared Memory를 초기화 하고 끝난다.  
 * 두 번째 프로세스는 Producer로 동작하게 한다.  
 * 세 번째 프로세스는 Consumer로 동작하게 한다.  

### 2. Method solved  
 * __struct shmbuf__  
 ① producer와 consumer에서 사용할 공유메모리를 struct shmbuf으로 지정한다. semaphore로 사용하기 위해 sem_t num_space, num_data를 설정하고 int buffer[100]으로 버퍼 100개를 배열로 지정한다.  

 * __main문__  
 ① fork를 2번해야하기에 pid1, pid2를 두고 consumer에서 빼서 출력할 변수 output, 공유메모리 이름을 const char *shm_name=“\shmp”로 지정하고 공유메모리 이름을 shmp로 지정한다.  
  
 * __process1, initialize__  
 ① fork를 하여 pid1이 0이라면 이전에 공유메모리를 사용하고 있을 수 있기 때문에 shm_unlink를 통해 연결을 해제한다. ppt자료에 따라 fd=shm_open(shm_name, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR)을 하고, fruncate(fd, sizeof(struct shmbuf)을 통해 길이를 파악한다.  
 ② shmp=mmap(NULL, sizeof(struct shmbuf), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, 0)을 통해 공유메모리를 shmp에 맵핑한다.  
 ③ unnamed_semaphore로 초기화하기 위해 sem_init(&shmp->num_data, 1, 0)을 통해 num_data를 초기화하고, sem_init(&shmp->num_space, 1, 100)을 통해 num_space를 초기화한다.  
 ④ shmp->in=-1, shmp->out=-1를 통해 in과 out의 값을 -1로 초기화한다.  
 ⑤ exit(0)을 통해 process1은 죽게된다.  
  
 그리고 process2를 실행하기 위해 process2는 process1의 initialize과정이 끝나고 실행 되어야하기 때문에 process2를 만들기 위해 fork를 하기 전, wait(&pid1)을 통해 process1이 죽을 때까지 기다린다.  
  
 * __process2, producer__  
 ① init에서 한 것처럼 shm_open하고 shmp=mmap을 구조체에 공유메모리를 맵핑한다.  
 ② process2인 producer는 버퍼에 데이터를 넣으면 되기에 for문을 통해 i가 0부터 1000보다 작을 때까지 반복하도록 한다. for문 안에서는 공유메모리가 맵핑된 구조체의 변수들을 사용할 것이기 때문에 sem_wait(&shmp->num_space)을 통해 space가 있으면 바로 실행되지만 space가 없으면 signal을 받을 때까지 block상태가 되도록 한다.  
 ③ shmp->in++을 하고 버퍼의 크기가 100이므로 in%100을 통해 계속 반복하여 사용할 수 있도록 하였다. shmp->buffer[shmp->in]=i를 통해 buffer에 i를 넣고 sem_post(&shmp->num_data)를 통해 consumer쪽에서 버퍼에 data가 없어서 block되어 있을 경우를 위해 signal을 보내서 깨워준다.  
 ④ exit(0)을 통해 process2는 죽게 된다.  

 * __process3, consumer__
 ① init에서 한 것처럼 shm_open하고 shmp=mmap을 구조체에 공유메모리를 맵핑한다.  
 ② process3인 consumer는 parent로, producer가 버퍼에 넣은 데이터를 빼면 되기에 for문을 통해 i가 0부터 1000보다 작을 때까지 반복하도록 한다. for문 안에서는 공유메모리가 맵핑된 구조체의 변수들을 사용할 것이기 때문에 sem_wait(&shmp->num_data)을 통해 data가 있으면 바로 실행되지만 data가 없으면 signal을 받을 때까지 block상태가 되도록 한다.  
 ③ shmp->out++을 하고 버퍼의 크기가 100이므로 out%100을 통해 계속 반복하여 사용할 수 있도록 하였다. output=shmp->buffer[shmp->out]를 통해 buffer에서 shmp->out에 저장된 값을 output으로 가지고 오도록 한다.  
 ④ sem_post(&shmp->num_space)를 통해 producer가 space가 없어서 data를 버퍼에 집어넣지 못하고 있을 수 있으므로 signal을 보내 producer가 block된 상태라면 깨워준다. 그리고 consumer의 output값을 출력한다.  
 ⑤ wait(&pid2)를 하여 process2가 죽을 때까지 기다리도록 하였다. 그리고 sem_destroy를 통해 semaphore를 제거하고 shm_unlink와 munmap을 통해 맵핑된 공유메모리를 해제한다.  
