#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j++;
    _yield();
  }
}

void init_proc() {
  // naive_uload(NULL, "/bin/bmptest");
  // naive_uload(NULL, "/bin/events");
  naive_uload(NULL, "/bin/init");
}

_Context* schedule(_Context *prev) {
  return NULL;
}


void execve(const char * filename,char * const argv[ ],char * const envp[ ])
{
  naive_uload(NULL, filename);
}
