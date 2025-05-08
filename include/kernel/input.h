#ifndef INPUT_H
#define INPUT_H

#include "stdint.h"
#include "stddef.h"

#define INPUT_BUFFER_SIZE 256

typedef struct input_driver {
	const char *name;    
	int priority;

	int (*probe)(void);
	int (*init)(void);
} input_driver_t;

typedef enum {
    KEY_PRESSED,
    KEY_RELEASED
} key_event_type;

typedef struct {
    key_event_type type;
    uint8_t scancode;
    char ascii;
} key_event;

void input_init();
void input_push_event(key_event event);
key_event input_pop_event();
int input_has_events();

void input_register_driver(input_driver_t *driver);

#endif // INPUT_H
