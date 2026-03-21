#include "interface_uart.h"

void Usart_SendString(UART_HandleTypeDef* handle, uint8_t *str)
{
    uint16_t k = 0;
    do
    {
        HAL_UART_Transmit(handle, (uint8_t *)(str + k), 1, 1000);
        k++;
    } while (*(str + k) != '\0');
}