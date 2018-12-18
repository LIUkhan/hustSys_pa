#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void exec_wrapper(bool);
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }
  char ch;
  while((ch=getchar())!=EOF);//清空缓冲区
  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  if(args == NULL)
    exec_wrapper(1);
  else {
    char *argnum = strtok(args," ");
    char *test = strtok(NULL," ");//检查有无多余参数
    if(test == NULL) {
      int cmdnum = atoi(argnum);
      for(int i = 0; i < cmdnum; i++)
        exec_wrapper(1);
    }
    else
      printf("Error:too many arguments\n");
  }
  return 0;
}


static int cmd_info(char *args) {
  if(args == NULL) {
    printf("Error:info instruction need an argument, r for register, w for watchpoint\n");
  }
  else {
    char *arg = strtok(args," ");
    char *test = strtok(NULL," ");//检查有无多余参数
    if(test == NULL)
    {
      if(!strcmp(arg,"r")) {
        for(int i = R_EAX; i <= R_EDI; i++) {
          printf("%s:0x%08x\n",regsl[i],cpu.gpr[i]._32);
        }
      }
      else if(!strcmp(arg,"w")) {
        printf("功能待完善\n");
      }
      else {
        printf("Error:the argument should be w or r\n");
      }
    }
    else
      printf("Error:too many arguments\n");
  } 
  return 0;
}

static int cmd_x(char *args) {
  if(args == NULL) {
    printf("Error: Instruction Format: x N EXPR,use help to learn more\n");
  }
  else {
    char *arg1 = strtok(args," ");
    char *arg2 = strtok(NULL," ");
    char *test = strtok(NULL," ");//检查有无多余参数
    if(test == NULL)
    {
      int n = atoi(arg1);
      uint32_t addr;
      sscanf(arg2,"%x",&addr);
      printf("%x\n",addr);
      for(int i = 0; i < n; i++)
      {
        printf("0x%08x\n",paddr_read(addr,4));
        addr += 4;//addr加1，移动一个字节,0xXXXXXXXX中高位对应高地址
      }
    }
    else
      printf("Error:too many arguments\n");
  }
  return 0;
}

static int cmd_p(char *args) {
  if(args == NULL) {
    printf("Error: Instruction Format: p EXPR,use help to learn more\n");
  }
  else {
    bool success;
    uint32_t ret = expr(args,&success);
    printf("result: %u\n",ret);
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","单步执行N条指令后暂停执行，N缺省为1",cmd_si},
  { "info","打印程序状态，r为寄存器，w为监视点信息",cmd_info},
  { "x","扫描内存，格式x N expr 求图expr的值，将结果作为起始内存地址，以16进制形式输出连续N个4字节",cmd_x},
  { "p","p EXPR 求出表达式EXPR的值",cmd_p},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif
    //在命令池里面比较是哪个命令,所有命令函数返回int，为了进行if的判断
    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
