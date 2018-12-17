#include "nemu.h"
int init_monitor(int, char *[]);
void ui_mainloop(int);

struct{
  uint32_t result;
  char buf[65536];
} test[100];
extern uint32_t expr(char *, bool *);
int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  // int is_batch_mode = init_monitor(argc, argv);

  /* Receive commands from user. */
  // ui_mainloop(is_batch_mode);
  printf("hello\n");
  bool success;
  FILE *fp = fopen("testinput", "r");
  if(fp == NULL)
    printf("1\n");
  else
    printf("2\n");
  for(int i = 0; i < 100; i++)
  {
    int sta1 = fscanf(fp,"%u",&test[i].result);
    char* sta2 = fgets(test[i].buf,65536,fp);
    if(sta1 == 0 || sta2 == NULL){
      printf("ERR\n");
      continue;
    }
    printf("%u  %u\n",expr(test[i].buf,&success),test[i].result);
  }
  return 0;
}
