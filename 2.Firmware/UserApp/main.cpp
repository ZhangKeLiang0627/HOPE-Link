#include "common_inc.h"
#include "oled_wrapper.h"
#include "interface_uart.h"
#include "multi_button_user.h"
#include "hugo_ui_user.h"
#include "tone.hpp"

// 5 User-Timers, can choose from htim1/htim2/htim4/htim10/htim11
Timer timerCtrlLoop(&htim4, 200);

// Encoder
Encoder encoder(&htim3);
Tone tone(&htim9);

/* Thread Definitions -----------------------------------------------------*/


/* Timer Callbacks --------------------------------------------------------*/
static uint32_t encoderTickCount = 0;
static uint32_t buttonTickCount = 0;
static uint32_t toneTickCount = 0;

void OnTimer4Callback()
{
    // multi_button
    button_ticks();

    // Encoder
    if (++encoderTickCount >= 2)
    {
        encoder.Update();
        encoderTickCount = 0;
    }

    // // Tone
    // if (++toneTickCount >= 2)
    // {
    //     tone.Update();
    //     toneTickCount = 0;
    // }
}

/* Default Entry ---------------------------------------------------------*/
void Main(void)
{
    // give USB_DEVICE some times
    HAL_Delay(1000); 
    
    keyInit(&uiKeyNum);
    encoder.Start(&uiEncoderNum);

    oledInit();
    HugoUI::InitLayout();
    HugoUI::LoadConfig();
    // HugoUI::SaveConfig();
    // tone.Play(BEEPER_WAKE);

    // Start Timer Callbacks.
    timerCtrlLoop.SetCallback(OnTimer4Callback);
    timerCtrlLoop.Start();

    for (;;)
    {
        HugoUI::TaskHandler();
    }
}
