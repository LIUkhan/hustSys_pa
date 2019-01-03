#include "fs.h"
#include "proc.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;  // 文件被打开之后的读写指针
  size_t open_offset;  
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  for(int i = 3; i < NR_FILES; i++) {
    file_table[i].read = ramdisk_read;
    file_table[i].write = ramdisk_write;
    file_table[i].open_offset = 0;
  }
}

size_t fs_filesz(int fd)
{
  return file_table[fd].size;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  assert(3 <= fd && fd < NR_FILES);
  //对应文件信息块起始地址
  Finfo *file = &file_table[fd];
  size_t filesz = fs_filesz(fd);
  int p_offset = file->open_offset + file->disk_offset;
  int rest = file->size - file->open_offset;
  if(len > rest) {
    len = rest;
  }
  assert(filesz >= file->open_offset + len);
  size_t ret = file->read(buf,p_offset,len);
  Log("openoff:%d len:%d newopenoff:%d poff:%d",file->open_offset,len,file->open_offset + ret,p_offset);
  if(ret < 0)
    return ret;
  file->open_offset += ret;
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  assert(3 <= fd && fd < NR_FILES);
  //对应文件信息块起始地址
  Finfo *file = &file_table[fd];
  size_t filesz = fs_filesz(fd);
  int p_offset = file->open_offset + file->disk_offset;
  int rest = file->size - file->open_offset;
  if(len > rest) {
    len = rest;
  }
  assert(filesz >= file->open_offset + len);
  size_t ret = file->write(buf,p_offset,len);
  if(ret < 0)
    return ret;
  file->open_offset += ret;
  return ret;
}
//计算并改变对应文件的open_offset
size_t fs_lseek(int fd, size_t offset, int whence)
{
  //对应文件信息块起始地址
  Finfo *file = &file_table[fd];
  size_t filesz = fs_filesz(fd);
  size_t base;
  switch(whence) {
    case SEEK_SET: {base = 0; break;}
    case SEEK_CUR: {base = file->open_offset; break;}
    case SEEK_END: {base = file->size; break;}
    default: panic("wrong whence!!!\n");
  }
  size_t newaddr = base + offset;
  Log("lseek fd:%d(size:%d, off:%d)(offset:%d, whence:%d) to %d", fd,
  file->size, file->open_offset, offset, whence, newaddr);
  //根据whence和base确定新的open_offset,offset是相对位移，文件开头为０
  
  // 边界控制
  assert(newaddr >= 0);
  assert(filesz >= newaddr);
  file->open_offset = newaddr;
  return newaddr;
}

int fs_open(const char *pathname, int flags, int mode)
{
  Log("opening %s", pathname);
  for(int i = 0; i < NR_FILES; i++) {
    Finfo *file = file_table + i;
    if(strcmp(file->name, pathname) == 0) {
      file->open_offset = 0;
      Log("Success! File fd = %d", i);
      return i;
    }
  }
  return -1;
}

int fs_close(int fd)
{
  Log("closing %d", fd);
  return 0;
}