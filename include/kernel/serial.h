#ifndef SERIAL_H
#define SERIAL_H

int serial8086_init();
int serial_received();
char serial_read();
int is_transmit_empty();
void serial_write(char c);

#endif // SERIAL_H
