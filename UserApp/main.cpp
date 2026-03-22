#include "common_inc.h"
#include "oled_wrapper.h"
#include "interface_uart.h"
#include "multi_button_user.h"
#include "hugo_ui_user.h"

// 5 User-Timers, can choose from htim1/htim2/htim4/htim10/htim11
Timer timerCtrlLoop(&htim4, 200);

/* Thread Definitions -----------------------------------------------------*/


/* Timer Callbacks -------------------------------------------------------*/
void OnTimer4Callback()
{
    // multi_button
    // static uint32_t buttonTickCount = 0;
    // if (++buttonTickCount >= 5)
    // {
    //     button_ticks();
    //     buttonTickCount = 0;
    // }
    button_ticks();


    // HugoUI
    HugoUI::TickInc();
}

/* Default Entry -------------------------------------------------------*/
void Main(void)
{
    // give USB_DEVICE some times
    HAL_Delay(1000); 
    
    keyInit(&uiKeyNum);

    oledInit();
    oledSetFont(u8g2_font_wqy13_t_gb2312a);
    HugoUI::InitLayout();

    // oledClearBuffer();
    // oledDrawUTF8(30, 15, "HelloHOPE");
    // oledSendBuffer();

    // Start Timer Callbacks.
    timerCtrlLoop.SetCallback(OnTimer4Callback);
    timerCtrlLoop.Start();

    while (true)
    {   
        HugoUI::TaskHandler();
    }
}
