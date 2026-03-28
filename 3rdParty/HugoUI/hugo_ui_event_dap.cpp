#include "hugo_ui_event_dap.h"

#include "ff.h"

#include "DAP.h"
#include "SWD_host.h"
#include "SWD_flash.h"

/* CMSIS-DAP test 的应用事件函数 */
void HugoUI::EventTestDapUI(void)
{
    static uint8_t isTestDapInit = 0;
    static uint8_t isEnterAnimFinish = 0;
    static uint8_t isWriteFinish = 0;

    // 所有变量都放在函数内部
    FRESULT Res;
    FIL fnew;
    uint8_t rData[1024];
    uint8_t Check_Data[1024];
    uint8_t readflag;
    uint32_t Burn_cnt = 0;
    char buf[16]; // 专门用来显示数字

    // Init
    if (!isTestDapInit)
    {
        isTestDapInit = 1;
    }
    else
    {
        oled_draw_str(110, FONT_HEIGHT * 4, "OK");
    }

    // Enter Anim
    if (!isEnterAnimFinish)
    {
        oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (Animation_Blur() == 0)
            isEnterAnimFinish = 1;
        oled_send_buffer();
        return;
    }

    // Loop
    oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP测试』");

    if (isWriteFinish)
    {
        oled_draw_UTF8(0, FONT_HEIGHT * 2, "烧录非常的成功!");
    }
    else
    {
        if (swd_init_debug())
        {
            oled_clear_buffer();
            oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP测试』");
            oled_draw_UTF8(0, FONT_HEIGHT * 2, "目标芯片已连接!!!");
            oled_send_buffer();

            if (target_flash_init(0x08000000) == ERROR_SUCCESS)
            {
                oled_clear_buffer();
                oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP测试』");
                oled_draw_UTF8(0, FONT_HEIGHT * 2, "正在擦除目标芯片!!!");
                oled_send_buffer();

                if (target_flash_erase_chip() == ERROR_SUCCESS)
                {
                    oled_clear_buffer();
                    oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP测试』");
                    oled_draw_UTF8(0, FONT_HEIGHT * 2, "目标芯片已擦除!!!");
                    oled_send_buffer();

                    // ====================== 烧录核心 ======================
                    Res = f_open(&fnew, (const TCHAR *)"0:/Firmware/HOPE-Link.bin", FA_READ);
                    if (Res == FR_OK)
                    {
                        uint32_t progess = 0, burn_addr = 0, time1, time2;
                        UINT bytesread;
                        readflag = 1;
                        time1 = HAL_GetTick();

                        while (readflag)
                        {
                            f_read(&fnew, rData, 1024, &bytesread);
                            if (bytesread < 1024)
                                readflag = 0;

                            if (target_flash_program_page(0x08000000 + burn_addr, rData, 1024) == ERROR_SUCCESS)
                            {
                                swd_read_memory(0x08000000 + burn_addr, Check_Data, 1024);
                                if (memcmp(Check_Data, rData, 1024) != 0)
                                {
                                    oled_clear_buffer();
                                    oled_draw_UTF8(20, FONT_HEIGHT * 1, "校验失败!!!");
                                    oled_send_buffer();
                                    HAL_Delay(1000);
                                    burn_addr = 0;
                                    readflag = 1;
                                    f_close(&fnew);
                                    return;
                                }

                                burn_addr += 1024;
                                progess = ((double)burn_addr / f_size(&fnew)) * 100;

                                // 安全显示进度（无编译错误）
                                oled_clear_buffer();
                                oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP测试』");
                                oled_draw_UTF8(0, FONT_HEIGHT * 2, "烧录中");
                                oled_draw_UTF8(0, FONT_HEIGHT * 3, "进度:");
                                sprintf(buf, "%d%%", (int)progess);
                                oled_draw_str(50, FONT_HEIGHT * 3, buf);
                                oled_send_buffer();
                            }
                            else
                            {
                                f_close(&fnew);
                                return;
                            }
                        }

                        time2 = HAL_GetTick();
                        Burn_cnt++;
                        oled_clear_buffer();
                        oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP测试』");
                        oled_draw_UTF8(0, FONT_HEIGHT * 2, "烧录完成!!!");
                        oled_send_buffer();
                        isWriteFinish = 1;
                        swd_set_target_reset(0);
                        HAL_Delay(1000);
                        f_close(&fnew);
                    }
                    // ======================================================
                }
            }
        }
        else
        {
            oled_draw_UTF8(0, FONT_HEIGHT * 2, "还没有芯片接入噢...");
            oled_send_area_buffer(0, FONT_HEIGHT * 2, SCREEN_WIDTH, FONT_HEIGHT);
        }
    }

    // Exit
    if (uiKeyNumInSide == 2)
    {
        uint8_t isExitAnimFinish = 0;
        oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        oled_send_buffer();

        while (!isExitAnimFinish)
        {
            if (Animation_Blur() == 0)
                isExitAnimFinish = 1;
            oled_send_buffer();
        }

        isEnterAnimFinish = 0;
        isWriteFinish = 0;
    }
}