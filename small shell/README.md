# Small Shell

#### 1. Problem definition(I/O, function)
 * /a.out을 했을 때, foreground에서 실행되도록 한다.
 * ./a.out& 했을 때, background에서 실행되도록 한다.
 * ./a.out> output을 했을 때, stdout의 a.out을 ‘output’파일로 exec()호출 전에 바꾼다. output file의 이름은 shell-input값에 의해 결정된다.