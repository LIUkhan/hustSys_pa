#include "proc.h"
#include "fs.h"
#define DEFAULT_ENTRY 0x4000000


static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  // size_t rsize = get_ramdisk_size();
  // ramdisk_read((void *)DEFAULT_ENTRY,0,rsize);
  assert(filename != NULL);
  int fd = fs_open(filename, 0, 0);
  int size = fs_filesz(fd);
  Log("load program %s {fd=%d} with size=%d", filename, fd, size);
  fs_read(fd, (void *)DEFAULT_ENTRY, size);
  fs_close(fd);
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
