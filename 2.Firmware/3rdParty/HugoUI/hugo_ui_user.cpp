
#include "hugo_ui_user.h"
#include "hugo_ui_widget.h"
#include "hugo_ui_event_dap.h"

#include "ff.h"

// serialNumber
#include "common_inc.h"

#include "ws2812b.hpp"

#include "interface_uart.h"

using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */
extern WS2812B led;

// 开关控件变量
bool testFlag = false;
bool inverseModeFlag = false;
bool flipModeFlag = false;
extern bool flashEraseSectorFlag;
extern bool autoTriggerFlag;

extern uint8_t screenshotPrintFlag; // 是否开启画面投屏标志位

// 改变值控件变量
float toneVolume = 80.0f;
float ledRed = 0.0f;
float ledGreen = 0.0f;
float ledBlue = 0.0f;

/* 用户函数 ----------------------------------------------------------- */
void EventShowAboutUI(void);
void EventSetInverseMode(void);
void EventSetFlipScreen(void);
void EventFactoryResetUI(void);
void EventFormatStorageUI(void);
void EventSerialNumberUI(void);
void EventShowGyroUI(void);
void EventSetLedColor(void);
void EventShowPotCloudUI(void);
void EventSetScreenshotPrintUI(void);
void EventSelectSerialBaudUI(void);

// [test code]
void EventShowWidgetInfoBar(void);
void EventShowWidgetInfoBar2(void);
void EventShowWidgetPopUp(void);
void EventShowWidgetPopUp2(void);
void EventShowProgressBarUI(void);

void HugoUI::InitLayout(void)
{
    /* 注册 Page */
    Page::Ptr pageMain = AddPage(PageType::Icon, "pageMain");
    Page::Ptr pageOffline = AddPage(PageType::List, "pageOffline");
    Page::Ptr pageSetting = AddPage(PageType::List, "pageSetting");
    Page::Ptr pageOflnSelFile = AddPage(PageType::List, "pageOflnSelFile");
    Page::Ptr pageOflnSelChip = AddPage(PageType::List, "pageOflnSelChip");
    Page::Ptr pageWS2812 = AddPage(PageType::List, "pageWS2812");
    Page::Ptr pageTest = AddPage(PageType::List, "pageTest");
    Page::Ptr pageBaud = AddPage(PageType::List, "pageBaud");

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

    pageMain->AddItem("Test", ItemType::JumpPage)
        ->SetJumpId(pageTest->pageId, 0)
        ->SetIconSrc(TreasureBox_BMP);

    pageMain->AddItem("IMU", ItemType::CallFunction, EventShowGyroUI)
        ->SetIconSrc(Poet_BMP);

    pageMain->AddItem("About", ItemType::CallFunction, EventShowAboutUI)
        ->SetIconSrc(Home_BMP);

    pageMain->AddItem("PotCloud", ItemType::CallFunction, EventShowPotCloudUI)
        ->SetIconSrc(Unicorn_BMP);

    // PageOffline
    pageOffline->AddItem("『离线下载固件』", ItemType::Description);
    pageOffline->AddItem("选择固件文件", ItemType::JumpPage)
        ->SetJumpId(pageOflnSelFile->pageId, 0);
    pageOffline->AddItem("选择芯片型号", ItemType::JumpPage)
        ->SetJumpId(pageOflnSelChip->pageId, 0);
    pageOffline->AddItem("设置下载地址", ItemType::CallFunction, EventSetFlashAddressUI);
    pageOffline->AddItem("全量擦除芯片", ItemType::CallFunction, EventEraseChipUI);
    pageOffline->AddItem("自动触发下载", ItemType::Switch, &autoTriggerFlag, EventAutoTriggerUI);
    pageOffline->AddItem("储存当前设置", ItemType::CallFunction, EventSaveConfigUI);
    pageOffline->AddItem("是否局部擦除", ItemType::Checkbox, &flashEraseSectorFlag, EventEraseSectorInfoBar);
    pageOffline->AddItem("> 开始下载", ItemType::CallFunction, EventBurnDapUI);
    pageOffline->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 0);

    // PageOfflineSelectFile
    pageOflnSelFile->AddItem("选择固件文件", ItemType::Description);
    AddItemsFromFirmwareFolder(pageOflnSelFile, "0:/Firmware");
    pageOflnSelFile->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 1);

    // PageOfflineSelectChip
    pageOflnSelChip->AddItem("选择芯片型号", ItemType::Description);
    AddItemsFromFlashAlgo(pageOflnSelChip);
    pageOflnSelChip->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 2);

    // PageSetting
    pageSetting->AddItem("『系统设置』", ItemType::Description);
    pageSetting->AddItem("反色模式", ItemType::Switch, &inverseModeFlag, EventSetInverseMode);
    pageSetting->AddItem("反转屏幕", ItemType::Switch, &flipModeFlag, EventSetFlipScreen);
    pageSetting->AddItem("画面投屏", ItemType::Switch, &screenshotPrintFlag, EventSetScreenshotPrintUI);
    pageSetting->AddItem("波特率设置", ItemType::JumpPage)
        ->SetJumpId(pageBaud->pageId, 0);
    pageSetting->AddItem("蜂鸣器音量", ItemType::ChangeValue, &toneVolume, nullptr);
    pageSetting->AddItem("格式化存储设备", ItemType::CallFunction, EventFormatStorageUI);
    pageSetting->AddItem("恢复出厂设置", ItemType::CallFunction, EventFactoryResetUI);
    pageSetting->AddItem("序列号", ItemType::CallFunction, EventSerialNumberUI);
    pageSetting->AddItem("{关于本机}", ItemType::CallFunction, EventShowAboutUI);
    pageSetting->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 1);

    // PageWS2812
    pageWS2812->AddItem("『RGB灯珠测试』", ItemType::Description);
    pageWS2812->AddItem("Set Red", ItemType::ChangeValue, &ledRed, EventSetLedColor);
    pageWS2812->AddItem("Set Green", ItemType::ChangeValue, &ledGreen, EventSetLedColor);
    pageWS2812->AddItem("Set Blue", ItemType::ChangeValue, &ledBlue, EventSetLedColor);
    pageWS2812->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 2);

    // PageTest
    pageTest->AddItem("『系统UI测试』", ItemType::Description);
    pageTest->AddItem("switch的长文本测试demoooo123456", ItemType::Switch, &testFlag, nullptr);
    pageTest->AddItem("checkbox的长文本测试demoooo123456", ItemType::Checkbox, &testFlag, nullptr);
    pageTest->AddItem("changevalue的长文本测试demoooo123456", ItemType::Checkbox, &testFlag, nullptr);
    pageTest->AddItem("weightInfoBar测试1", ItemType::Switch, &testFlag, EventShowWidgetInfoBar);
    pageTest->AddItem("weightInfoBar测试2", ItemType::Switch, &testFlag, EventShowWidgetInfoBar2);
    pageTest->AddItem("weightPopUp测试1", ItemType::Switch, &testFlag, EventShowWidgetPopUp);
    pageTest->AddItem("weightPopUp测试2", ItemType::Switch, &testFlag, EventShowWidgetPopUp2);
    pageTest->AddItem("进度条测试", ItemType::CallFunction, EventShowProgressBarUI);
    pageTest->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 3);

    // PageBaud
    pageBaud->AddItem("『波特率设置』", ItemType::Description);
    pageBaud->AddItem("短按设置波特率↓", ItemType::Description);
    pageBaud->AddItem("9600", ItemType::CallFunction, EventSelectSerialBaudUI);
    pageBaud->AddItem("115200", ItemType::CallFunction, EventSelectSerialBaudUI);
    pageBaud->AddItem("230400", ItemType::CallFunction, EventSelectSerialBaudUI);
    pageBaud->AddItem("460800", ItemType::CallFunction, EventSelectSerialBaudUI);
    pageBaud->AddItem("921600", ItemType::CallFunction, EventSelectSerialBaudUI);
    pageBaud->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageSetting->pageId, 4);
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
        if (Transition_Hourglass() == 0)
            isEnterAnimFinish = 1;
        return;
    }

    // Loop
    Animation_Elastic(&motion_a, &motion_a_trg, 235);

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
            if (Transition_Blur() == 0)
                isExitAnimFinish = 1;
            oled_send_buffer();
        }

        motion_a = 80.0f;
        isEnterAnimFinish = 0;
    }
}

#include "mpu6050.h"
/* Gyro的应用事件函数 */
void EventShowGyroUI(void)
{
    static uint8_t isEnterAnimFinish = 0;
    static uint8_t isInit = 0;
    static float pitch = 0.0f, roll = 0.0f, yaw = 0.0f;
    // Enter
    if (!isEnterAnimFinish && isInit)
    {
        oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (Transition_Blur() == 0)
            isEnterAnimFinish = 1;
        return;
    }

    if(!isInit)
    {
        WidgetDrawMessageBox("IMU初始化中...", true);

        if(MPU_DMP_Init_WithTimeout(1000) == 0)
        {
            isInit = 1;
        }
        else
        {
            WidgetPushInfoBar("IMU初始化失败!", 2000);
            uiKeyNumInSide = 2;
        }
    }
    else
    {
        oled_draw_str(0, 13, "IMU:");
        oled_draw_str(100, 13, "OK");
    }

    // Loop
    mpu_dmp_get_data(&pitch, &roll, &yaw);

    oled_draw_str(0, 13 * 2, "roll:");
    oled_draw_str(0, 13 * 3, "yaw:");
    oled_draw_str(0, 13 * 4, "pitch:");

    oledDrawFloat(50, 13 * 2, -roll, 2, 2);
    oledDrawFloat(50, 13 * 3, -yaw, 2, 2);
    oledDrawFloat(50, 13 * 4, -pitch, 2, 2);

    // Exit
    if (uiKeyNumInSide == 2)
    {
        uint8_t isExitAnimFinish = 0;
        oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        oled_send_buffer();

        while (!isExitAnimFinish)
        {
            if (Transition_Blur() == 0)
                isExitAnimFinish = 1;
            oled_send_buffer();
        }

        isEnterAnimFinish = 0;
    }
}

void EventFormatStorageUI(void)
{
    // Show Widget
    WidgetDrawMessageBox("当前暂无功能...", true);

    // Util
    // TODO: 格式化存储设备
    delay(2000);
    uiKeyNumInSide = 2; // 自动退出

    // Exit
    if (uiKeyNumInSide == 2)
    {
    }
}

void EventFactoryResetUI(void)
{
    // Show Widget
    WidgetDrawMessageBox("当前暂无功能...", true);

    // Util
    // TODO: 恢复出厂设置
    delay(2000);
    uiKeyNumInSide = 2; // 自动退出

    // Exit
    if (uiKeyNumInSide == 2)
    {
    }
}

void EventSerialNumberUI(void)
{
    // Show Widget
    uint8_t serialString[32] = {0};
    sprintf((char *)serialString, "s:%s", serialNumberStr);
    WidgetDrawMessageBox((const char *)serialString, true);

    // Util

    // Exit
    if (uiKeyNumInSide == 2 || uiKeyNumInSide == 1)
    {
        uiKeyNumInSide = 2; // 手动退出
    }
}

/* 画面投屏的事件回调 */
void EventSetScreenshotPrintUI(void)
{
    if (screenshotPrintFlag)
    {
        WidgetPushInfoBar("投屏很吃性能哦!", 2000);
        huart1.Init.BaudRate = 921600;
        HAL_UART_Init(&huart1);
    }
    else
    {
        WidgetPushInfoBar("投屏已关闭!", 2000);
        huart1.Init.BaudRate = 115200;
        HAL_UART_Init(&huart1);
    }
}

/* 反色模式的事件回调 */
void EventSetInverseMode(void)
{
    oledSetInverseColor(inverseModeFlag);
}

/* 反转屏幕的事件回调 */
void EventSetFlipScreen(void)
{
    oledSetFlipMode(flipModeFlag);
}

/* RGB灯珠颜色控制事件函数 */
void EventSetLedColor(void)
{
    led.SetPixels(1, (uint32_t)((uint16_t)ledGreen << 16 | (uint16_t)ledRed << 8 | (uint16_t)ledBlue));
    led.UpdatePixels();

    // Show Widget
    WidgetPushInfoBar("UpdatePixels!", 2000);
}

/* 波特率选择事件函数 */
void EventSelectSerialBaudUI(void)
{
    // 选择波特率
    // 通过获取当前item的title来判断波特率
    if (currentItem != nullptr)
    {
        const char* title = currentItem->title.c_str();

        if (strcmp(title, "9600") == 0)
            huart1.Init.BaudRate = 9600;
        else if (strcmp(title, "115200") == 0)
            huart1.Init.BaudRate = 115200;
        else if (strcmp(title, "230400") == 0)
            huart1.Init.BaudRate = 230400;
        else if (strcmp(title, "460800") == 0)
            huart1.Init.BaudRate = 460800;
        else if (strcmp(title, "921600") == 0)
            huart1.Init.BaudRate = 921600;

        // 按照选中的波特率重新初始化UART
        HAL_UART_Init(&huart1);
    }

    // Show Widget
    char string[16] = {0};
    sprintf(string, "StBd:%d", huart1.Init.BaudRate); // set baud
    WidgetDrawMessageBox((const char *)string, true);

    // Util
    delay(1000);
    uiKeyNumInSide = 2; // 自动退出
}

// [test code]

/* ShowWidgetInfoBar测试函数 */
void EventShowWidgetInfoBar(void)
{
    // Show Widget
    WidgetPushInfoBar("have fun:)", 2000);
}

void EventShowWidgetInfoBar2(void)
{
    // Show Widget
    WidgetPushInfoBar("测试弹窗功能:p", 2000);
}

void EventShowWidgetPopUp(void)
{
    // Show Widget
    WidgetPushPopUp("have fun:)", 2000);
}

void EventShowWidgetPopUp2(void)
{
    // Show Widget
    WidgetPushPopUp("测试弹窗功能:p", 2000);
}

void EventShowProgressBarUI(void)
{
    static float num = 0.0f;

    WidgetDrawProgressBar("进度条测试", num, true);

    // Util
    {
        delay(500);
        num += 3;
        WidgetDrawProgressBar("进度条测试", num, true);
        if(num >= 100)
        {
            uiKeyNumInSide = 2; // Exit
        }
    }

    // Exit
    if (uiKeyNumInSide == 2)
    {
        num = 0;
    }
}

void EventShowPotCloudUI(void)
{
    typedef struct
    {
        int8_t x, y;
    } Dot_t;

    const uint8_t dots_num = 64;
    const uint8_t cam_f = 64;

    static Dot_t dots[dots_num];
    static int16_t baseX;
    static uint8_t baseZ;
    static uint8_t speed;
    static int8_t vx;
    static uint8_t isInit = 0;
    static uint8_t isEnterAnimFinish = 0;

    // init
    if (!isInit)
    {
        for (int i = 0; i < dots_num; i++)
        {
            dots[i].x = GetRandom(-SCREEN_WIDTH, SCREEN_WIDTH);
            dots[i].y = GetRandom(-SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2);
        }
        baseX = 0;
        baseZ = 0;
        speed = 2;
        vx = 0;
        isInit = 1;
    }

    // Enter
    if (!isEnterAnimFinish)
    {
        if (Transition_Iris() == 0)
            isEnterAnimFinish = 1;
        return;
    }

    // loop
    static uint8_t vz = speed;
    // baseX -= vx * 2;
    baseZ = (baseZ - vz) & (dots_num - 1);

    // draw dots cloud
    for (int i = 0; i < dots_num; i++)
    {
        uint8_t depth = (i + baseZ) & (dots_num - 1);
        int16_t s = (SCREEN_HEIGHT * cam_f) / (cam_f + depth);
        int16_t x = SCREEN_WIDTH / 2 + ((dots[i].x + baseX) * s >> 6);
        int16_t y = SCREEN_HEIGHT / 2 + (dots[i].y * s >> 6);
        // 只在边缘区域绘制像素
        if (y < 24 || x < 48 || x >= SCREEN_WIDTH - 48)
            oled_draw_pixel(x, y);
    }

    // draw strings
    oled_draw_str(0, 13, "Point Cloud");
    oledDrawNum(120, 13, vz);

    if (uiEncoderNumInSide == 1)
    {
        vz ++;
        if (vz > 5)
            vz = 5;
    }
    else if (uiEncoderNumInSide == 2)
    {
        vz --;
        if (vz < 1)
            vz = 1;
    }

    // exit
    if (uiKeyNumInSide == 2)
    {
        uint8_t isExitAnimFinish = 0;
        oled_send_buffer();

        while (!isExitAnimFinish)
        {
            if (Transition_Iris() == 0)
                isExitAnimFinish = 1;
            oled_send_buffer();
        }

        isInit = 0;
        isEnterAnimFinish = 0;
    }
}