
#include "hugo_ui_user.h"

using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */

// 开关控件变量

/* 用户函数 ----------------------------------------------------------- */

void EventShowAboutUI(void);

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
    pageOffline->AddItem("> 开始下载", ItemType::CallFunction, nullptr);

    pageOffline->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageMain->pageId, 0);

    // PageOfflineSelectFile
    pageOflnSelFile->AddItem("选择固件文件", ItemType::Description);
    pageOflnSelFile->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 1);

    // PageOfflineSelectChip
    pageOflnSelChip->AddItem("选择芯片型号", ItemType::Description);
    pageOflnSelChip->AddItem("返回", ItemType::JumpPage)
        ->SetJumpId(pageOffline->pageId, 2);

    // PageSetting
    pageSetting->AddItem("Setting", ItemType::Description);
    pageSetting->AddItem("PID Editor", ItemType::JumpPage)
        ->SetJumpId(pagePID->pageId, 0);

    pageSetting->AddItem("InverseColor", ItemType::Switch, nullptr, nullptr);
    pageSetting->AddItem("FilpScreen", ItemType::Switch, nullptr, nullptr);
    pageSetting->AddItem("LightLevel", ItemType::ChangeValue, nullptr, nullptr);
    pageSetting->AddItem("BeepEnable", ItemType::Switch, nullptr, nullptr);
    pageSetting->AddItem("UASRT62Printf", ItemType::Switch, nullptr, nullptr);
    pageSetting->AddItem("Page2List", ItemType::Switch, nullptr, nullptr);
    pageSetting->AddItem("Volume Ctrl", ItemType::ChangeValue, nullptr, nullptr);
    pageSetting->AddItem("SaveAll", ItemType::CallFunction, nullptr);

    pageSetting->AddItem("Exit", ItemType::JumpPage)
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

    Animation_Linear(&motion_a, &motion_a_trg, 85);

    oled_draw_bMP(motion_a, 0, 40, 50, HeadSculpture_BMP);
    oled_draw_str(50, FONT_HEIGHT, "HOPE  Pro");
    oled_draw_UTF8(45, FONT_HEIGHT * 2, "版本: Ver1.3");
    oled_draw_UTF8(45, FONT_HEIGHT * 3, "储存: 16 MB");
    oled_draw_UTF8(10, FONT_HEIGHT * 4, " By @kkl_aka科良");
    oled_set_draw_color(2);
    oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    oled_draw_R_box(86, 3, 30, FONT_HEIGHT, 0);
    oled_set_draw_color(1);

    if (uiKeyNumInSide == 2)
    {
        motion_a = 80.0f;
    }
}
