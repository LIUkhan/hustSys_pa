#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  printf("0x%x\n",a[0]);
  switch (a[0]) {

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
