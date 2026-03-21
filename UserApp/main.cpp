#include "common_inc.h"
#include "oled_wrapper.h"

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

    }
}
