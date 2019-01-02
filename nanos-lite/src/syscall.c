#include "common.h"
#include "syscall.h"
#include "fs.h"
#include <sys/types.h>
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
      //stdout stderr
      if(fd == 0)
        assert(0);
      else if(fd == 1 || fd == 2) {
        for(int i = 0; i < len; i++)
          _putc(buf[i]);
        c->GPR1 = len;
        // Log("%s",buf);
      }
      else {
        c->GPR1 = fs_write(fd,(void *)buf,len);
      }
      break;
    }
    case SYS_brk: {
      c->GPR1 = 0;
      break;
    }
    case SYS_open: {
      printf("SYS_open\n");
      const char *filename  = (const char *)c->GPR2;
      int flags = c->GPR3;
      mode_t mode = c->GPR4;
      printf("%s\n",filename);
      c->GPR1 = fs_open(filename, flags, mode);
      break;
    }
    case SYS_close: {
      printf("SYS_close\n");
      int fd = c->GPR2;
      c->GPR1 = fs_close(fd);
      break;
    }
    case SYS_read: {
      printf("SYS_read\n");
      int fd = c->GPR2;
      const char *buf  = (const char *)c->GPR3;
      size_t count = c->GPR4;
      c->GPR1 = fs_read(fd,(void *)buf,count);
      break;
    }
    case SYS_lseek: {
      printf("SYS_lseek\n");
      int fd = c->GPR2;
      off_t offset  = c->GPR3;
      int whence = c->GPR4;
      c->GPR1 = fs_lseek(fd, offset, whence);
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}