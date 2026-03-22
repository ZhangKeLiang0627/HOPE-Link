#include "common_inc.h"
#include "oled_wrapper.h"
#include "interface_uart.h"
#include "multi_button_user.h"

uint8_t testKeyNum = 0;
// 5 User-Timers, can choose from htim1/htim2/htim4/htim10/htim11
Timer timerCtrlLoop(&htim4, 200);
static uint8_t tempNum = 0;

/* Thread Definitions -----------------------------------------------------*/


/* Timer Callbacks -------------------------------------------------------*/
void OnTimer4Callback()
{
    tempNum ++;

    /*---------- multi_button ----------*/
    button_ticks();
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

    keyInit(&testKeyNum);

    // Start Timer Callbacks.
    timerCtrlLoop.SetCallback(OnTimer4Callback);
    timerCtrlLoop.Start();

    while (true)
    {   
        oledClearBuffer();
        oledDrawUTF8(30, 15, "HelloHOPE");
        oledDrawNum(30, 30, tempNum);
        oledDrawNum(30, 45, testKeyNum);
        oledSendBuffer();
        // uint8_t ch[32] = "this is HOPE-Link speaking!\n";
        // Usart_SendString(&huart1, ch);
        HAL_Delay(10);
		}
}
