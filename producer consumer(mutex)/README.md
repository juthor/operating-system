# Producer Consumer(Mutex)

### 1. Problem definition(I/O, function)  
 * 배열로 만들어진 ring-buffer를 최대 100개의 linked list로 만든다.  
 * Producer의 insertion은 pthread로 만들고 만약 linked list가 다 차면 기다린다.  
  - tail pointer를 가지고 있어야 하고, 가능하면 데이터를 list의 tail에 insert한다.  
 * Consumer의 removal은 pthread로 만들고 만약 linked list가 비어있으면 기다린다.  
  - head pointer를 가지고 있어야 하고, 가능하면 데이터를 list의 head에서 get하고 remove한다.  
  - output data를 console에 출력한다.  

- - -

### 2. Method solved
#### 구조체 정의
 * 배열로 이루어진 버퍼를 linked list로 만들기 위해 구조체로 Node를 만든다.  
 * makeNode로 head, tail, basicNode를 초기화한다.  
 * __insertNode Function__  
  ① consumer함수에서 보낸 인자를 data로 받아 linked list에 저장한다. basicNode의 data는 매개인자 data를 가지도록 하고 next는 아무것도 없으므로(다음 노드는 tail로 pointer에 의해 insert되므로) NULL을 갖는다.  
  ② 만약 head가 NULL이면 아무것도 없다는 의미이므로, head와 tail을 basicNode를 가리키도록 한다.  
  ③ 아니면 입력된 노드들이 있다는 의미이므로, tail이 가리키는 Node의 next는 basicNode를 가리키도록 하고 tail은 basicNode를 가리키도록 한다.  
  
 * __deleteNode Function__  
  ① producer함수에서 호출되고 head가 가리키는 node를 지우고 데이터를 출력하도록 한다.  
  ② 만약 head가 NULL값을 가진다면 아무것도 없다는 의미이므로 return을 하도록 한다.  
  ③ 아니면 node는 head가 가리키는 값을 가리키도록 하고 head는 head의 next값을 가리키도록 한다. 그리고 node를 free상태로 만들고 return한도록 한다.  

#### Producer Function  
 * __for문__  
  ① if문을 사용하여 count가 100이라면 버퍼가 다 찼다는 의미이므로, thread는 cond_wait로 buffer_has_space signal이 들어올 때까지 기다리도록 하고, 그 때 signal이 들어올 수 있도록 mutex를 unlock한다.  
  ② 아니면 insertNode에 i를 넣도록 하고 전역변수인 count값을 1증가한다. 혹시 consumer에서 buffer에 data가 없어서 기다릴 수 있으므로 buffer_has_data signal을 보내주고 mutex를 unlock한다.  

#### Consumer Function  
 * __for문__  
  ① if문을 사용하여 count가 0이라면 버퍼가 비어있다는 의미이므로 buffer_has_data signal이 들어올 때까지 cond_wait하고, mutex를 unlock한다.  
  ② deleteNode()를 통해 delete는 맨 처음부터 지우고 값을 얻도록 한다.  
  ③ count값을 1 감소하고 producer에서 버퍼가 자리가 남을 때까지 기다리고 있을 수 있으므로 cond_signal을 통해 buffer_has_space signal을 보낸다.  
  ④ mutex를 unlock하고 printf를 통해 deleteNode를 통해 print_data에 저장된 값을 출력한다.  
  
#### Main  
 * main문에서 makeNode함수를 행하고 pthread_create를 통해 threads[0], threads[1] 각각을 생성한다.  
 * 생성한 pthread를 join상태로 만들고 return 0를 한다.  