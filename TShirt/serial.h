/** Definitions for serial port **/

void init_serial(unsigned long int speed);
void send_serial(uint8_t c);
uint8_t get_serial(void);
void init_printf(void);
