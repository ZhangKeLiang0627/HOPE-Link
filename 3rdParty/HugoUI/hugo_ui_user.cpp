
#include "hugo_ui_user.h"
#include "hugo_ui_event_dap.h"

#include "ff.h"

using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */

// 开关控件变量
bool testFlag = false;
bool inverseModeFlag = false;

/* 用户函数 ----------------------------------------------------------- */
void EventShowAboutUI(void);
void EventSetInverseMode(void);

void HugoUI::InitLayout(void)
{
    /* 注册 Page */
    Page::Ptr pageMain = AddPage(PageType::Icon, "pageMain");
    Page::Ptr pageOffline = AddPage(PageType::List, "pageOffline");
    Page::Ptr pageSetting = AddPage(PageType::List, "pageSetting");
    Page::Ptr pageOflnSelFile = AddPage(PageType::List, "pageOflnSelFile");
    Page::Ptr pageOflnSelChip = AddPage(PageType::List, "pageOflnSelChip");
    Page::Ptr pageWS2812 = AddPage(PageType::List, "pageWS2812");

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

    pageMain->AddItem("About", ItemType::CallFunction, EventShowAboutUI)
        ->SetIconSrc(Home_BMP);

    // PageOffline
    pageOffline->AddItem("『离线下载固件』", ItemType::Description);
    pageOffline->AddItem("选择固件文件", ItemType::JumpPage)
        ->SetJumpId(pageOflnSelFile->pageId, 0);
    pageOffline->AddItem("选择芯片型号", ItemType::JumpPage)
        ->SetJumpId(pageOflnSelChip->pageId, 0);
    pageOffline->AddItem("设置下载地址", ItemType::CallFunction, nullptr);
    pageOffline->AddItem("全量擦除芯片", ItemType::CallFunction, EventEraseChipUI);
    pageOffline->AddItem("自动触发下载", ItemType::Switch, nullptr, nullptr);
    pageOffline->AddItem("储存当前设置", ItemType::CallFunction, nullptr);
    pageOffline->AddItem("> 开始下载", ItemType::CallFunction, EventTestDapUI);

    pageOffline->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 0);

    // PageOfflineSelectFile
    pageOflnSelFile->AddItem("选择固件文件", ItemType::Description);
    AddItemsFromFirmwareFolder(pageOflnSelFile, "0:/Firmware");
    pageOflnSelFile->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 1);

    // PageOfflineSelectChip
    pageOflnSelChip->AddItem("选择芯片型号", ItemType::Description);
    pageOflnSelChip->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 2);

    // PageSetting
    pageSetting->AddItem("『系统设置』", ItemType::Description);
    pageSetting->AddItem("switch的长文本测试demooooooo", ItemType::Switch, &testFlag, nullptr);
    pageSetting->AddItem("checkbox的长文本测试demoooo123456", ItemType::Checkbox, &testFlag, nullptr);
    pageSetting->AddItem("changevalue的长文本测试demoooo123456", ItemType::Checkbox, &testFlag, nullptr);

    pageSetting->AddItem("反色模式", ItemType::Switch, &inverseModeFlag, EventSetInverseMode);
    pageSetting->AddItem("蜂鸣器音量", ItemType::ChangeValue, nullptr, nullptr);
    pageSetting->AddItem("格式化存储设备", ItemType::CallFunction, nullptr);
    pageSetting->AddItem("恢复出厂设置", ItemType::CallFunction, nullptr);
    pageSetting->AddItem("{关于本机}", ItemType::CallFunction, EventShowAboutUI);

    pageSetting->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 1);

    // PageWS2812
    pageWS2812->AddItem("『RGB灯珠测试』", ItemType::Description);
    pageWS2812->AddItem("SetRed", ItemType::ChangeValue,nullptr, nullptr);
    pageWS2812->AddItem("SetGreen", ItemType::ChangeValue, nullptr, nullptr);
    pageWS2812->AddItem("SetBlue", ItemType::ChangeValue,nullptr, nullptr);
    pageWS2812->AddItem("Exit", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 2);

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
            if (Animation_Blur() == 0)
                isExitAnimFinish = 1;
            oled_send_buffer();
        }

        motion_a = 80.0f;
        isEnterAnimFinish = 0;
    }
}

/* 反色模式的事件回调 */
void EventSetInverseMode(void)
{
    oledSetInverseColor(inverseModeFlag);
}