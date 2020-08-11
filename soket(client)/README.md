# Soket(Client)
### 1. Problem definition(I/O, function)  
 * Soket을 활용한 Client & Server 통신을 프로그래밍한다.
- - -  
### 2. Method solved  
#### __main문__
HOSTADDR는 리눅스 명령어중 hostname –I를 통해 address를 얻었다.  
 ① sock함수를 통해 두 시스템간의 통신을 위해 채널을 만들고 만든 채널을 통해 data를 주고 받도록 한다.  
 ② connect함수를 통해 server 주소와 길이로 server와 연결을 시도한다. 이때, server는 accept상태여야 client의 connect를 받아들여서 연결이 될 수 있다.  
 ③ while문을 통해 NULL이 아닐 때까지 문자열을 입력받고, send함수를 통해 server에게 문자열을 전송한다.  
 ④ 중첩 while문을 통해 recv_bytes가 send_bytes보다 작을 때까지 반복하도록 하고 server에게로부터 문자를 받도록 한다. 하지만 server가 문자를 원하는 만큼의 크기로 보냈어도 client에서 받을 때 잘려서 받을 수도 있기 때문에 문자열을 받을 때 2가지 type이 있는데 그중에서 들어오는 만큼 받고 그 남은 정도를 확인해서 모든 데이터를 받는 방법을 사용하였다.  
 ⑤ 마지막에 rcvdata[recv_bytes]=0을 통해 NULL을 넣고 close(sd)를 통해 소켓을 닫고 return 을 통해 종료한다.

 - - -