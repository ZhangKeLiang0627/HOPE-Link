#include "common_inc.h"
#include "oled_wrapper.h"
#include "interface_uart.h"
#include "multi_button_user.h"
#include "hugo_ui_user.h"
#include "tone.hpp"
#include "mpu6050.h"

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
    
    // keyInit(&uiKeyNum);
    // encoder.Start(&uiEncoderNum);

    Usart_debugMsg("MPU DMP 初始化...");

    // MPU_Init();
    mpu_dmp_init();
    // if (MPU_Init() == 0)
    // {
    //     // MPU DMP 初始化成功
    //     Usart_debugMsg("MPU DMP 初始化成功");
    // }
    // else
    // {
    //     // MPU DMP 初始化失败
    //     Usart_debugMsg("MPU DMP 初始化失败");
    // }

    // oledInit();
    // HugoUI::InitLayout();
    // HugoUI::LoadConfig();
    // // HugoUI::SaveConfig();
    // // tone.Play(BEEPER_WAKE);

    // // Start Timer Callbacks.
    // timerCtrlLoop.SetCallback(OnTimer4Callback);
    // timerCtrlLoop.Start();
    short gx, gy, gz = 0;
    float pitch, roll, yaw = 0;
    for (;;)
    {
        // MPU_Get_Gyroscope(&gx, &gy, &gz);
        // Usart_debugMsg("x:%d y:%d z:%d", gx, gy, gz);

        mpu_dmp_get_data(&pitch, &roll, &yaw);
        Usart_debugMsg("pitch:%f roll:%f yaw:%f", pitch, roll, yaw);
        //    HugoUI::TaskHandler();
    }
}
