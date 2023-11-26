#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint16_t k=*(uint16_t *)(KBD_ADDR);
  kbd->keydown = (k&KEYDOWN_MASK)?1:0;
  kbd->keycode = k&(~KEYDOWN_MASK);
}
