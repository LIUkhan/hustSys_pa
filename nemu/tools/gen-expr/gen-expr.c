#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
int bufindex = 0;

uint32_t choose(uint32_t n)
{
  return rand()%n;
}
//符号两边加上空格，适当加空格
static void gen_rand_op() {
  if(bufindex >= 65535)
    return;
  switch(choose(4))
  {
    case 0: {
      buf[bufindex++] = ' ';
      buf[bufindex++] = '+';
      buf[bufindex++] = ' ';
      break;
    }
    case 1: {
      buf[bufindex++] = ' ';
      buf[bufindex++] = '-';
      buf[bufindex++] = ' ';
      break;
    }
    case 2: {
      buf[bufindex++] = ' ';
      buf[bufindex++] = '*';
      buf[bufindex++] = ' ';
      break;
    }
    case 3: {
      buf[bufindex++] = ' ';
      buf[bufindex++] = '/';
      buf[bufindex++] = ' ';
      break;
    }
  }
}

static inline void gen(char c) {
  if(bufindex >= 65535)
    return;
  buf[bufindex++] = c;
}

static void gen_num(){
  uint32_t num = rand()%100;
  //保证不会除以0,但是生成的表达式会有0的存在,怎么办。。。。。。通过gccwarning输出控制重新来过
  if(num == 0 && buf[bufindex-2] == '/') {
    while(num == 0) {
      num = rand()%100;
    }
  }
  sprintf(buf+bufindex,"%uu",num);
  while(num /10 != 0)
  {
    num /= 10;
    if(bufindex >= 65535)
      return;
    bufindex++;
  }
  bufindex+=2;
}

static inline void gen_rand_expr() {
  if(bufindex >= 65535)
    return;
  switch(choose(3))
  {
    case 0:{
      gen_rand_expr();gen_rand_op();gen_rand_expr();break;
    }
    case 1:{
      gen_num();break;
    }
    case 2:{
      gen('(');gen_rand_expr();gen(')');break;
    }
  }
}
//示例程序
static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  //选择创建多少次表达式
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    bufindex = 0;//每次更新表达式index，作为刷新
    gen_rand_expr();
    //控制不会超出缓冲区的大小
    while(bufindex >= 65535)
    {
      bufindex = 0;//超过缓冲区，重新生成表达式
      gen_rand_expr();
    }
    buf[bufindex] = '\0'; //添加末尾的终结符

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);
    //顺便输出标准输出和标准错误
    // int ret = system("gcc .code.c -o .expr > result 2>&1 | grep division result");
    int ret = system("gcc .code.c -o .expr 2>err");
    if (ret != 0) continue; //一条表达式错误不影响后面的表达式
    ret = system("grep division err 1>result");
    //如果grep division成功
    if(ret == 0)//否则ret为256
    {
      i--;
      continue;
    }
    fp = popen("./.expr", "r");
    assert(fp != NULL);
    int result;
    int status = fscanf(fp, "%d", &result);
    if(status == 0)
      printf("Error:failed to read the result!\n");
    else
      printf("%u %s\n", result, buf);
    pclose(fp);
  }
  return 0;
}
