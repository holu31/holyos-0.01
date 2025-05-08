#include "i8042.h"
#include "kernel/cpu/io.h"
#include "kernel/kprintf.h"

__attribute__((constructor)) void i8042_init() {
	log_info("[I8042] Initializing PS/2 controller...\n");
	
	i8042_write_cmd(I8042_CMD_DISABLE_KBD);
	i8042_write_cmd(I8042_CMD_DISABLE_MOUSE);

	while (inb(I8042_STATUS_PORT) & 0x1) {
		inb(I8042_DATA_PORT);
	}

	log_ok("[I8042] Controller detected and working\n");

	i8042_write_cmd(I8042_CMD_ENABLE_KBD);
	i8042_write_cmd(I8042_CMD_ENABLE_MOUSE);
}

bool i8042_check() {
	uint8_t status;
	asm volatile ("inb %1, %0" : "=a"(status) : "Nd"(I8042_STATUS_PORT));

	if (status == 0x00 || status == 0xFF) {
		log_debug("[I8042] No controller (status=0x%02X)\n", status);
		return false;
	}

	i8042_write_cmd(I8042_CMD_DISABLE_KBD);
	i8042_write_cmd(I8042_CMD_DISABLE_MOUSE);

	i8042_write_cmd(0xAA);
	uint8_t response = i8042_read_data();
	if (response != 0x55) {
		log_err("[I8042] Self-test failed (got 0x%02X, expected 0x55)\n", response);
		return false;
	}

	i8042_write_cmd(I8042_CMD_ENABLE_KBD);
	i8042_write_cmd(I8042_CMD_ENABLE_MOUSE);

	return true;
}

uint8_t i8042_read_data() {
	while (!(inb(I8042_STATUS_PORT) & I8042_STATUS_OUTPUT_FULL));
	return inb(I8042_DATA_PORT);
}

void i8042_write_data(uint8_t data) {
	while ((inb(I8042_STATUS_PORT) & I8042_STATUS_INPUT_FULL));
	outb(I8042_DATA_PORT, data);
}

void i8042_write_cmd(uint8_t cmd) {
	while ((inb(I8042_STATUS_PORT) & I8042_STATUS_INPUT_FULL));
	outb(I8042_CMD_PORT, cmd);
}
