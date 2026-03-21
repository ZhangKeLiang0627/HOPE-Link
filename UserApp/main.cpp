#include "common_inc.h"
#include "oled_wrapper.h"
#include "interface_uart.h"

/* Thread Definitions -----------------------------------------------------*/


/* Timer Callbacks -------------------------------------------------------*/


/* Default Entry -------------------------------------------------------*/
void Main(void)
{
    HAL_Delay(1000); // give USB_DEVICE some times

    oledInit();
    oledSetFont(u8g2_font_wqy13_t_gb2312a);

    oledClearBuffer();
    oledDrawUTF8(30, 15, "HelloHOPE");
    oledSendBuffer();

    while (true)
    {
        uint8_t ch[32] = "this is HOPE-Link speaking!\n";
        Usart_SendString(&huart1, ch);
        HAL_Delay(5000); // give USB_DEVICE some times
    }
}
