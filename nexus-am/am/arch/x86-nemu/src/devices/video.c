#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>

#define SCREEN_PORT 0x100

// #define W 400
// #define H 300

static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;



size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;
      uint32_t WH = inl(SCREEN_PORT);//from vga.c
      info->width =  WH >>16;
      info->height = WH && 0xffff;
      // info->width = screen_width();
      // info->height = screen_height();
      return sizeof(_VideoInfoReg);
    }
  }
  return 0;
}

size_t video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _FBCtlReg *ctl = (_FBCtlReg *)buf;
      int i;
      int size =  screen_width() * screen_height();
      for (i = 0; i < size; i ++) fb[i] = i;
      // int width = screen_width();
      // for(int y = ctl->y; y < ctl->y + ctl->h; y++)
      //   for(int x = ctl->x; x < ctl->x + ctl->w; x++)
      //     fb[y*width+x] = *(ctl->pixels);
      if (ctl->sync) {
        // do nothing, hardware syncs.
      }
      return sizeof(_FBCtlReg);
    }
  }
  return 0;
}

void vga_init() {
}
