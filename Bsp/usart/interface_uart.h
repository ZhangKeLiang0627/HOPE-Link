#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"
#include "usart.h"

#include <stdint.h>

void Usart_SendString(UART_HandleTypeDef *handle, uint8_t *str);

#ifdef __cplusplus
}
#endif

#endif // __USART_H
