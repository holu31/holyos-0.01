#include "i8042.h"
#include "keymap.h"
#include "kernel/input.h"
#include "kernel/cpu/interrupt.h"
#include "kernel/kprintf.h"
#include "../prefs.h"

static int shift_pressed = 0;

static int ps2kbd_probe(void) {
	return i8042_check();
}

static void ps2kbd_handler(struct regs *r) {
	uint8_t scancode = i8042_read_data();
	key_event event;

	if (scancode == 0x2A || scancode == 0x36) {
        	shift_pressed = 1;
	} else if (scancode == 0xAA || scancode == 0xB6) {
		shift_pressed = 0;
       	}

	event.type = (scancode & 0x80) ? KEY_RELEASED : KEY_PRESSED;
	event.scancode = scancode & 0x7F;
	event.ascii = (event.type == KEY_PRESSED) ?
		keymap_to_ascii(event.scancode, shift_pressed) : 0;

	input_push_event(event);
}

static int ps2kbd_init(void) {
	interrupt_register_handler(33, ps2kbd_handler);
	return 0;
}

input_driver_t ps2_keyboard_driver = {
	.name = "PS/2 Keyboard",
	.priority = VIDEO_PRIORITY_PS2,
	.probe = ps2kbd_probe,
	.init = ps2kbd_init
};

__attribute__((constructor)) void register_ps2kbd() {
	input_register_driver(&ps2_keyboard_driver);
}
