#ifndef __PS2_KEYMAP_H
#define __PS2_KEYMAP_H

#include "stdint.h"

char keymap_to_ascii(uint8_t scancode, int shift);

#endif // __PS2_KEYMAP_H
