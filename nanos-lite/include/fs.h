#ifndef __FS_H__
#define __FS_H__

#include "common.h"

#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

extern size_t fs_read(int, void *, size_t);
extern size_t fs_write(int, const void *, size_t);
extern size_t fs_lseek(int, size_t, int);
extern int fs_open(const char *, int, int);
extern int fs_close(int);
extern size_t fs_filesz(int);
extern size_t serial_write(const void *buf, size_t, size_t);
extern size_t events_read(void *, size_t, size_t);
extern size_t dispinfo_read(void *, size_t, size_t);
extern size_t fb_write(const void *, size_t, size_t);

#endif
