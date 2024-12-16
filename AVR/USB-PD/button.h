#ifndef __button__
#define __button__
#include <stdint.h>

void button_init(void);
void button_process(void);
void button_setLong(uint16_t ms);
int8_t button_turns(void);
bool button_down(void);
bool button_pressed(void);
bool button_held(void);

#endif //__button__