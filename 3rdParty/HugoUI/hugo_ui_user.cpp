
#include "hugo_ui_user.h"
#include "ff.h"

using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */

// 开关控件变量
bool test_flag = false;

/* 用户函数 ----------------------------------------------------------- */

void EventShowAboutUI(void);
void EventTestDapUI(void);
void AddItemsFromFolder(Page::Ptr page, const char *folderPath);

void HugoUI::InitLayout(void)
{
    /* 注册 Page */
    Page::Ptr pageMain = AddPage(PageType::Icon, "pageMain");
    Page::Ptr pageOffline = AddPage(PageType::List, "pageOffline");
    Page::Ptr pageSetting = AddPage(PageType::List, "pageSetting");
    Page::Ptr pagePID = AddPage(PageType::List, "pagePID");
    Page::Ptr pageAnimation = AddPage(PageType::List, "pageAnimation");
    Page::Ptr pageWS2812 = AddPage(PageType::List, "pageWS2812");
    Page::Ptr pageOflnSelFile = AddPage(PageType::List, "pageOflnSelFile");
    Page::Ptr pageOflnSelChip = AddPage(PageType::List, "pageOflnSelChip");

    /* 注册 Item */
    // PageMain
    pageMain->AddItem("Offline", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 0)
        ->SetIconSrc(Knife_BMP);

    pageMain->AddItem("Setting", ItemType::JumpPage)
        ->SetJumpId(pageSetting->pageId, 0)
        ->SetIconSrc(Setting_BMP);

    pageMain->AddItem("RGB", ItemType::JumpPage)
        ->SetJumpId(pageWS2812->pageId, 0)
        ->SetIconSrc(Unicorn_BMP);

    pageMain->AddItem("RC522", ItemType::CallFunction, nullptr)
        ->SetIconSrc(Cuteghost_BMP);

    pageMain->AddItem("About", ItemType::CallFunction, EventShowAboutUI)
        ->SetIconSrc(Home_BMP);

    // PageOffline
    pageOffline->AddItem("『离线下载固件』", ItemType::Description);
    pageOffline->AddItem("选择固件文件", ItemType::JumpPage)
        ->SetJumpId(pageOflnSelFile->pageId, 0);
    pageOffline->AddItem("选择芯片型号", ItemType::JumpPage)
        ->SetJumpId(pageOflnSelChip->pageId, 0);
    pageOffline->AddItem("设置下载地址", ItemType::CallFunction, nullptr);
    pageOffline->AddItem("自动触发下载", ItemType::Switch, nullptr, nullptr);
    pageOffline->AddItem("储存当前设置", ItemType::CallFunction, nullptr);
    pageOffline->AddItem("擦除芯片", ItemType::CallFunction, nullptr);
    pageOffline->AddItem("> 开始下载", ItemType::CallFunction, EventTestDapUI);

    pageOffline->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 0);

    // PageOfflineSelectFile
    pageOflnSelFile->AddItem("选择固件文件", ItemType::Description);
    AddItemsFromFolder(pageOflnSelFile, "0:/Firmware");
    pageOflnSelFile->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 1);

    // PageOfflineSelectChip
    pageOflnSelChip->AddItem("选择芯片型号", ItemType::Description);
    pageOflnSelChip->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 2);

    // PageSetting
    pageSetting->AddItem("『系统设置』", ItemType::Description);
    pageSetting->AddItem("PID Editor", ItemType::JumpPage)
        ->SetJumpId(pagePID->pageId, 0);

    pageSetting->AddItem("swi长文本测试demooooooo", ItemType::Switch, &test_flag, nullptr);
    pageSetting->AddItem("chx测试测试测试测实验测试啦啦啦啦啦啦啦", ItemType::Checkbox, &test_flag, nullptr);
    pageSetting->AddItem("chanval测试测试测试测实验测试啦啦啦啦啦啦啦", ItemType::Checkbox, &test_flag, nullptr);

    pageSetting->AddItem("反色模式", ItemType::Switch, nullptr, nullptr);
    pageSetting->AddItem("蜂鸣器音量", ItemType::ChangeValue, nullptr, nullptr);
    pageSetting->AddItem("格式化存储设备", ItemType::CallFunction, nullptr);
    pageSetting->AddItem("恢复出厂设置", ItemType::CallFunction, nullptr);
    pageSetting->AddItem("{关于本机}", ItemType::CallFunction, EventShowAboutUI);

    pageSetting->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 1);

    // PagePID
    pagePID->AddItem("PID Editor", ItemType::Description);
    pagePID->AddItem("Kp", ItemType::ChangeValue, nullptr, nullptr);
    pagePID->AddItem("Ki", ItemType::ChangeValue, nullptr, nullptr);
    pagePID->AddItem("Kd", ItemType::ChangeValue, nullptr, nullptr);
    pagePID->AddItem("Exit", ItemType::JumpPage)
        ->SetJumpId(pageSetting->pageId, 1);

    // PageWS2812
    pageWS2812->AddItem("『RGB灯珠测试』", ItemType::Description);
    pageWS2812->AddItem("SetRed", ItemType::ChangeValue,nullptr, nullptr);
    pageWS2812->AddItem("SetGreen", ItemType::ChangeValue, nullptr, nullptr);
    pageWS2812->AddItem("SetBlue", ItemType::ChangeValue,nullptr, nullptr);
    pageWS2812->AddItem("Exit", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 4);

}

/* About的应用事件函数 */
void EventShowAboutUI(void)
{
    static float motion_a = 80.0f, motion_a_trg = 0;
    static uint8_t isEnterAnimFinish = 0;

    // Enter
    if (!isEnterAnimFinish)
    {
        oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (Animation_Blur() == 0)
            isEnterAnimFinish = 1;
        return;
    }
    
    // Loop
    Animation_Linear(&motion_a, &motion_a_trg, 85);

    oled_draw_bMP(motion_a, 0, 40, 50, HeadSculpture_BMP);
    oled_draw_str(48, FONT_HEIGHT, "HOPE- Link");
    oled_draw_UTF8(45, FONT_HEIGHT * 2, "版本: Ver1.3");
    oled_draw_UTF8(45, FONT_HEIGHT * 3, "储存: 16 MB");
    oled_draw_UTF8(10, FONT_HEIGHT * 4, " By @kkl_aka科良");
    oled_set_draw_color(2);
    oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    oled_draw_R_box(86, 3, 30, FONT_HEIGHT, 0);
    oled_set_draw_color(1);

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

        motion_a = 80.0f;
        isEnterAnimFinish = 0;
    }
}

#include "DAP.h"
#include "SWD_host.h"
#include "SWD_flash.h"

/* CMSIS-DAP test 的应用事件函数 */
void EventTestDapUI(void)
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
                                    oled_draw_UTF8(20, FONT_HEIGHT * 1, "校验失败!!");
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
                                oled_draw_UTF8(0, FONT_HEIGHT*2, "烧录中");
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
                        oled_draw_UTF8(0, FONT_HEIGHT*2, "烧录完成!!!");
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

void AddItemsFromFolder(Page::Ptr page, const char *folderPath)
{
    if (page == nullptr || folderPath == nullptr)
        return;

    DIR dir;
    FILINFO fno;
    FRESULT res;

    // 打开文件夹
    res = f_opendir(&dir, (const TCHAR*)folderPath);
    if (res != FR_OK)
    {
        page->AddItem("-> Firmware <-", ItemType::Description);
        page->AddItem("该文件夹不存在", ItemType::Description);
        return;
    }

    // 遍历所有文件
    for (;;)
    {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0)
            break;

        // 跳过文件夹，只添加文件
        if (fno.fattrib & AM_DIR)
            continue;

        // 添加文件名作为 Item，点击触发回调
        page->AddItem((char *)fno.fname, ItemType::Checkbox, nullptr, nullptr);
    }

    f_closedir(&dir);
}