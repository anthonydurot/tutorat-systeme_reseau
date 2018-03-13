#ifndef SERIAL_H
#define SERIAL_H

/** Prototypes fonctions port série **/

void init_serial(unsigned long int);
void send_serial(char);
char get_serial(void);
void init_printf(void);

#endif
