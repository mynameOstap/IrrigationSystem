

#include <stdint.h>

#define LOW 0x0
#define HIGH 0x1

extern void digitalWrite(uint8_t pin, uint8_t value);

static const int pump_pins[] = {14,21};

void pump_on(int i)
{
    digitalWrite(pump_pins[i], HIGH);
}

void pump_off(int i)
{
    digitalWrite(pump_pins[i], LOW);
}

