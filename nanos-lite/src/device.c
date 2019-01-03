#include "common.h"
#include <amdev.h>
#include "klib.h"

extern void setsize(int,size_t);

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for(int i = 0; i < len; i++)
    _putc(((char*)buf)[i]);
  return 0;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  assert(offset <= 128);
  if(len + offset >= 128)
    len = 128 - offset;
  memcpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  offset /= 4;
  int x = offset % screen_height();
  int y = offset / screen_width();
  draw_rect((uint32_t *)buf,x,y,len/4,1);
  return len/4;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  int w = screen_width();
  int h = screen_height();
  //提前写入到字符串中
  int sz = sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", w, h);
  setsize(4,sz);
}