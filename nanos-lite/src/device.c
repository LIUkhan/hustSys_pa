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
  int keycode = read_key();
  printf("len:%d\n",len);
  const char *k = "ku";
  //检查当前是什么键盘状态,检查是否被按下，如果是，取出keycode
  if(keycode & 0x8000) {
    keycode = keycode & 0x7fff;
    k = "kd";
  }
  if(keycode != _KEY_NONE) {
    //取出名字
    const char *name = keyname[keycode];
    int n = snprintf(buf, len, "%s %s\n", k, name);
    assert(n <= len);
    return n;
  } 
  else { //时间事件
    uint32_t time = uptime();
    int n = snprintf(buf, len, "t %d\n", time);
    return n;
  }
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
  int x = offset % screen_width();
  int y = offset / screen_width();
  draw_rect((uint32_t *)buf,x,y,len/4,1);
  return len;
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