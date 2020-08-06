# Pipe

### 1. Problem definition(I/O, function)  
 * p2, p3을 fork하고 나서 10개의 문자를 읽고 pipe1에 넣는다. p2, p3이 exit할 때까지 기다린다.  
 * pipe1에 10개의 문자를 채우고 대문자로 바꾼 후, p2에 넣는다.  
 * 10개의 문자를 pipe2로부터 얻은 후 출력한다.  
---------------------------------------------
### 2. Method solved  
#### process1  
 ① 첫 번째, 두 번째 fork를 통해 pid1, pid2가 모두 0이 아닐 경우는 부모인 경우이므로 process1에 해당한다.  
 ② close(fd1[0])을 하고 10개의 문자를 입력받도록 한다.  
 ③ write(fd1[1], char_string, EOD)를 통해 fd1[1]에 char_string을 쓰도록 한다.  
 ④ 자식들 pid1과 pid2가 죽을 때까지 wait를 한다.  

#### process2  
 ① 첫 번째 fork를 통해 첫 번째 자식을 생성한다. 그리고 pipe의 close(fd1[1])을 하여 사용하지 않을 pipe의 입구를 닫는다.  
 read(fd1[0], chan_up, EOD)를 통해 fd1[0]으로부터 읽어서 EOD(End Of Data)만큼 읽어서 chan_up에 저장한다.
 ② for문을 통해 소문자로 입력되었을 경우 대문자로 바꿔준다.  
 ③ write(fd2[1], chan_up, EOD)를 통해 fd2[1]에 chan_up을 쓰도록 한다.  
 ④ close(fd2[1]), close(fd1[0])을 하고 exit(0)을 통해 자식 프로세스 process2가 끝이 났다.  

### process3  
 ① close(fd2[1])을 통해 사용하지 않을 channel을 닫는다.  
 ② read(fd2[0], output, EOD)를 통해 output에 fd2[0]의 값을 쓰고 printf를 통해 출력하도록 한다.  
 ③ close(fd2[0])을 하고 exit(0)을 통해 자식 프로세스 process3이 끝이 났다.  
