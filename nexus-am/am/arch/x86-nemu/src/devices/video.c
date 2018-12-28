#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>

#define SCREEN_PORT 0x100

// #define W 400
// #define H 300

static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;


static inline uint32_t pixel(uint8_t r, uint8_t g, uint8_t b) {
  return (r << 16) | (g << 8) | b;
}
static inline uint8_t R(uint32_t p) { return p >> 16; }
static inline uint8_t G(uint32_t p) { return p >> 8; }
static inline uint8_t B(uint32_t p) { return p; }


size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;
      uint32_t WH = inl(SCREEN_PORT);//from vga.c
      info->width =  (WH >>16)/2;
      info->height = (WH && 0xffff)/2;
      // info->width = W;
      // info->height = H;
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
      int size = screen_width() * screen_height();
      for (i = 0; i < size; i ++) fb[i] = i;

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
