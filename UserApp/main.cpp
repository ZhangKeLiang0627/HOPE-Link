#include "common_inc.h"
#include "oled_wrapper.h"
#include "interface_uart.h"


// 5 User-Timers, can choose from htim1/htim2/htim4/htim10/htim11
Timer timerCtrlLoop(&htim4, 200);

/* Thread Definitions -----------------------------------------------------*/


/* Timer Callbacks -------------------------------------------------------*/
void OnTimer4Callback()
{

    static uint8_t tempNum = 0;
    tempNum ++;

    oledClearBuffer();
    oledDrawUTF8(30, 15, "HelloHOPE");
    oledDrawNum(30, 30, tempNum);
    oledSendBuffer();
}

/* Default Entry -------------------------------------------------------*/
void Main(void)
{
    // give USB_DEVICE some times
    HAL_Delay(1000); 

    oledInit();
    oledSetFont(u8g2_font_wqy13_t_gb2312a);

    oledClearBuffer();
    oledDrawUTF8(30, 15, "HelloHOPE");
    oledSendBuffer();

    // Start Timer Callbacks.
    timerCtrlLoop.SetCallback(OnTimer4Callback);
    timerCtrlLoop.Start();

    while (true)
    {
        // uint8_t ch[32] = "this is HOPE-Link speaking!\n";
        // Usart_SendString(&huart1, ch);
        HAL_Delay(5000);
		}
}
