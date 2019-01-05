#include "common.h"
#include "syscall.h"
#include "fs.h"
#include <sys/types.h>
extern char _end;
extern int my_execve(const char *,char *,char *);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // printf("0x%x 0x%x 0x%x 0x%x\n",a[0],a[1],a[2],a[3]);
  switch (a[0]) {
     case SYS_exit: {
      //  uintptr_t status = c->GPR2;
      // _halt(status);
      my_execve("/bin/init",NULL,NULL);
      break;
    }
    case SYS_yield: {
      _yield();
      c->GPR1 = 0;
      break;
    }
    case SYS_write: {
      uintptr_t fd = c->GPR2;
      const char *buf = (const char *)(c->GPR3);
      size_t len = c->GPR4;
      c->GPR1 = fs_write(fd,(void *)buf,len);
      //  printf("SYS_write\n");
      break;
    }
    case SYS_brk: {
      //  printf("SYS_brk\n");
      c->GPR1 = 0;
      break;
    }
    case SYS_open: {
      // printf("SYS_open\n");
      const char *filename  = (const char *)(c->GPR2);
      int flags = c->GPR3;
      mode_t mode = c->GPR4;
      c->GPR1 = fs_open(filename, flags, mode);
      break;
    }
    case SYS_close: {
      // printf("SYS_close\n");
      int fd = c->GPR2;
      c->GPR1 = fs_close(fd);
      break;
    }
    case SYS_read: {
      // printf("SYS_read\n");
      int fd = c->GPR2;
      const char *buf  = (const char *)c->GPR3;
      size_t count = c->GPR4;
      c->GPR1 = fs_read(fd,(void *)buf,count);
      break;
    }
    case SYS_lseek: {
      // printf("SYS_lseek\n");
      int fd = c->GPR2;
      off_t offset  = c->GPR3;
      int whence = c->GPR4;
      c->GPR1 = fs_lseek(fd, offset, whence);
      break;
    }
    case SYS_execve: {
       my_execve((const char *)a[1],(char *)a[2],(char *)a[3]);
       break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}