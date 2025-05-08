#include "kernel/input.h"
#include "kernel/kprintf.h"
#include "kernel/cpu/spinlock.h"

#define MAX_INPUT_DRIVERS 8

static key_event buffer[INPUT_BUFFER_SIZE];
static uint32_t head = 0;
static uint32_t tail = 0;
static spinlock_t lock;
static input_driver_t *drivers[MAX_INPUT_DRIVERS];

void input_init() {
	spinlock_init(&lock);
	head = tail = 0;

	input_driver_t* best = NULL;
	for (int i = 0; drivers[i]; i++) {
		if (drivers[i]->priority < best->priority) continue;
		if (drivers[i]->probe()) {
			best = drivers[i];	
			break;
		}
	}

	if (best) {
		if (best->init)
			best->init();
		
		log_ok("[INPUT] Initializated default input: %s\n", best->name);
	}
	else log_err("[INPUT] No suitable input driver found!\n");

}

void input_push_event(key_event event) {
	spinlock_lock(&lock);
	uint32_t next = (head + 1) % INPUT_BUFFER_SIZE;
	if (next != tail) {
		buffer[head] = event;
		head = next;
	}
	spinlock_unlock(&lock);
}

key_event input_pop_event() {
	spinlock_lock(&lock);
	key_event event = {0};
	if (tail != head) {
		event = buffer[tail];
		tail = (tail + 1) % INPUT_BUFFER_SIZE;
	}
	spinlock_unlock(&lock);
	return event;
}

int input_has_events() {
	return head != tail;
}

void input_register_driver(input_driver_t *driver) {
	 for (int i = 0; i < MAX_INPUT_DRIVERS; i++) {
		 if (!drivers[i]) {
			 drivers[i] = driver;
			 log_info("[VIDEO] Registered driver '%s' (priority=%d)\n", 
					 driver->name, driver->priority);
			 return;
		 }
	 }
	 log_err("[INPUT] Cannot register driver '%s', no slots left\n", driver->name);

}
