#include <am.h>
#include <x86.h>
#include <amdev.h>

#define I8042_DATA_PORT 0x60
#define KEYDOWN_MASK 0x8000

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      kbd->keydown = 0;
      kbd->keycode = _KEY_NONE;
      uint32_t code = inl(I8042_DATA_PORT);//读取通码或者断码

      kbd->keydown = (code & KEYDOWN_MASK ? 1 : 0);
      kbd->keycode = code & ~KEYDOWN_MASK;

      return sizeof(_KbdReg);
    }
  }
  return 0;
}
