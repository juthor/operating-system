# Small Shell

### 1. Problem definition(I/O, function)  
 * /a.out을 했을 때, foreground에서 실행되도록 한다.  
 * ./a.out& 했을 때, background에서 실행되도록 한다.  
 * ./a.out> output을 했을 때, stdout의 a.out을 ‘output’파일로 exec()호출 전에 바꾼다. output file의 이름은 shell-input값에 의해 결정된다.  
- - -
### 2. Method solved
#### main문-입력, parse  

 * __while__  

  getlogin()을 조건으로 login상태동안 반복한다. while문 안에는 get_current_dir_name()함수를 호출하여 printf를 통해 출력한다. 그리고 gets함수를 통해 input_line변수를 통해 prompt에서 입력받도록 한다.  
    
 * __parsing__  

 * while문  
  getlogin()을 조건으로 login상태동안 반복한다. while문 안에는 get_current_dir_name()함수를 호출하여 printf를 통해 출력한다. 그리고 gets함수를 통해 input_line변수를 통해 prompt에서 입력받도록 한다.  
    
 * parsing  

  입력받은 문자를 하나하나 구분하여 argv에 넣어야 fork와 exec함수 실행이 제대로 되기 때문에, 분리하기 위해 변수 설정에서 정해놓은 ptr, ndx, argv 변수를 사용하여 parse을 진행한다.  
  ① ptr=NULL, ndx=0으로 다시 초기화 해주고 strok함수(문자열 분리함수)를 사용하여 input_line을 공백을 기준으로 나눠서 ptr=strtok(input_line, “ ”)을 실행하여 ptr이 가리키도록 한다.  
  ② while문을 이용하여 ptr이 NULL이 아닐 때까지 반복하도록 하고(더이상 자를 문자열이 없으면 NULL을 반환하기 때문) argv[ndx]=ptr을 하고 ptr은 다시 strtok함수를 이용하여 NULL, 공백을 기준으로 나누도록 한다. 이는 자른 문자열을 NULL로 채우기 때문에 반복해서 문자열을 자르는 작업을 하기에 대상 문자열을 NULL로 주는 것이다. 그리고 ndx++을 통해 argv의 인덱스 값을 증가한다.  
  ③ while문 밖에서는 argv[ndx]=NULL로 하고 argc=ndx를 한다.  
  ④ if문을 사용하여 문자열 비교 함수인 strcmp((argv[0], “exit”)==0)이면 끝내도록 하고 execute(argv)를 통해 execute함수를 실행한다.    
  
#### execute함수  
 execute함수에서는 인자로 char **argv를 받는다. fork하고 파일을 새로 생성할 것이기에 pid_t pid, int status, fd, i, background로 돌아가는지 여부에 따른 변수 background, redirection인지 여부에 따른 변수 re_dir, 새로 만들 file을 위한 file이름 변수인 char형 file_name을 변수로 한다.  
  
 * __반복문 for(i=0; argv[i]!=0; i++)__  
  ① for문을 통해 i=0부터 argv[i]가 0이 아닐 때까지 반복한다.  
  ② if문을 이용해서 strcmp((argv[i], “>”)==0)이면 redirection 기호가 있다는 의미이므로, 해당 argv[i]값을 NULL로 하고 re_dir=1로 한다.  
  ③ 아니면 strcmp((argv[i], “&”==0))이면 &기호가 있다는 의미이므로 argv[i]를 NULL로 하고 background를 1로 한다.  
  
 * __조건문 if((pid=fork())==0)__  
  ① if문을 통해서 fork를 해주고 child process이면 re_dir이 1인지 if문을 통해 확인하고 fd=open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)를 통해 argv[2]를 파일 이름으로 하는 파일을 open하거나 없다면 creat하도록 한다.  
  ② argv[2]는 re_dir=1일 때 “>” 다음으로 들어오는 문자로 파일명 의미를 가진다. 그리고 dup2(fd, 1)을 통해 1은 stdout를 fd로 바꾸라는 의미로 stdout이 아닌 fd로 출력이 되도록 한다. 그리고 close(fd)를 하여 끝나면 닫도록 한다.  
  ③ 그리고 execvp를 통해 argv[0], argv를 넣어 exec을 하고 0보다 작다면 에러가 발생한 것이기에 exit(1)을 한다.  
  
 * __조건문 if(background==0)__  
  ① background가 0이라면 foreground로 실행되어야 하므로 while(wait(&status)!=pid)를 통해 자식이 끝날 때까지 기다린다.  
  
 * __조건문 if(background==1)__  
  ① background가 1이라면 background로 실행되어야 하므로 waitpid(WNOHANG, &status, 0)을 통해 자식 process가 끝날 때까지 기다리지 않고 부모프로세스가 하던 일을 하도록 한다.  

- - -