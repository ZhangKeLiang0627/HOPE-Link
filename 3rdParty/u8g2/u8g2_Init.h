#ifndef __U8G2_INIT_H__
#define __U8G2_INIT_H__
#include "u8g2.h"

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

void u8g2_init(u8g2_t *u8g2);

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
#endif



