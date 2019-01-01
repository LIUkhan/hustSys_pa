#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  printf("0x%x 0x%x 0x%x 0x%x\n",a[0],a[1],a[2],a[3]);
  switch (a[0]) {
     case SYS_exit: {
       uintptr_t status = c->GPR2;
      _halt(status);
      break;
    }
    case SYS_yield: {
      _yield();
      c->GPR1 = 0;
      break;
    }
    case SYS_write: {
      uintptr_t fd = c->GPR2;
      char *buf = (char *)(c->GPR3);
      uintptr_t len = c->GPR4;
      if(fd == 1 || fd == 2) {
        for(int i = 0; i < len; i++)
          _putc(buf[i]);
      }
      uint32_t bufsize = sizeof(buf);
      c->GPR1 = (len < bufsize) ? len : bufsize;
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
