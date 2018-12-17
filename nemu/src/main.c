#include "nemu.h"
#include <stdio.h>

int init_monitor(int, char *[]);
void ui_mainloop(int);

struct{
  char result[100];
  char buf[65536];
} test[100];
extern uint32_t expr(char *, bool *);
int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
  bool success;
  FILE *fptr = fopen("src/testinput","r");
  if(fptr == NULL)
    printf("1\n");
  else
    printf("2\n");
  for(int i = 0; i < 100; i++)
  {
    int sta1 = fscanf(fptr,"%s",test[i].result);
    char* sta2 = fgets(test[i].buf,65536,fptr);
    printf("%d\n",sta1);
    if(sta1 == -1 || sta2 == NULL){
      printf("ERR\n");
      continue;
    }
    printf("%u \n",expr(test[i].buf,&success));
  }
  fclose(fptr);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);
  return 0;
}
