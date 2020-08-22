# Soket(Server)
### 1. Problem definition(I/O, function) 
 * Soket을 활용한 Client & Server 통신을 프로그래밍한다. 
- - -
### 2. Method solved  
fork로 client가 connect를 원할 때마다 자식을 생성하였는데, fork가 thread에 비해 상대적으로 무거워서 thread로 만들었다. THREAD_MAX는 최대로 생성가능한 thread의 개수를 정하여 client가 최대 5개 접속이 가능하도록 하였다.
  
#### __main__
 ① client들이 접속을 시도할 때 fork를 안 쓰고 thread를 사용하니까 acccpt를 통해 새 socket을 열어야 하므로 accp_sock[THREAD_MAX]를 설정하였다. thread 생성시 제대로 생성됐는지를 위해 stat변수를 두었다.  
 ② sock을 열고 bzero를 통해 server addr를 초기화 해주었다. 그리고 servaddr.sin_family=AF_INET, servaddr.sin_addr.s_addr=htonl(INADDR_ANY),  servaddr.sin_port=htons(PORT)를 통해 기본적인 설정을 해주었다.  
 ③ bind함수를 통신을 위해 생성된 소켓에다 local host의 IP주소와 PORT주소를 연결하는 과정을 한다.  
 ④ while문을 통해　listen을 하여 client-queue의 길이를 선언하고 accp_sock에다가 accept를 통해 반환된 새 sock을 저장한다.  
 ⑤　acc_sock[count_cli]<0이면 에러가 발생한 것으로 에러확인을 하고 pthread_create(&thread[count_cli], NULL, &threadfunc, (void *) &accp_sock[count_cli])를 통해 thread를 생성한다. 그리고 count_cli수를 증가시키고 만약 count_cli가 6이면 5개가 생성되었으므로 break를 걸어 더 이상 생성하지 못하도록 하였다.  
  
#### __threadfunc__
 ① main문에서 thread 생성을 통해 동작한다. 변수를 설정하고 nsd=(int)*((int *)arg)를 통해 main문에서 연결이 성공적으로 이루어져서 반환된 새소켓 번호를 가지도록 한다.  
 ② while문에서 recv함수로 문자를 받고 받은 문자를 그대로 send함수를 통해 전송한다.   
 ③ close(nsd)를 통해 threadfunc를 종료한다.  