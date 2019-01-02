#include "common.h"
#include "syscall.h"
#include <unistd.h>
extern char _end;

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // printf("0x%x 0x%x 0x%x 0x%x\n",a[0],a[1],a[2],a[3]);
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
      Log("%s",buf);
      uint32_t bufsize = sizeof(buf);
      c->GPR1 = (len < bufsize) ? len : bufsize;
      break;
    }
    case SYS_brk: {
      c->GPR1 = 0;
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
//      case SYS_brk: {
//             // int increment = c->GPR2;
//             _def(new_addr, 2, void*);
//             if((void*)&_end < new_addr && new_addr < _heap.end) {
//                 _heap.start = new_addr;
//                 _set_ret(new_addr);
//                 break;
//                 // c->GPR1 = (size_t)old;
//             } else {
//                 void* old_addr = (void*)_heap.start;
//                 _set_ret(old_addr);
//                 break;
//             }
//             break;
// }