#ifndef PORTIO_H
#define PORTIO_H

#include <stdint.h>

uint8_t inport8 (uint16_t);
uint16_t inport16 (uint16_t);
uint32_t inport32 (uint16_t);

uint8_t outport8 (uint16_t, uint8_t);
uint16_t outport16 (uint16_t, uint16_t);
uint32_t outport32 (uint16_t, uint32_t);

#endif
