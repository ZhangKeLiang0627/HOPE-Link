
#include "hugo_ui_user.h"

using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */

// 开关控件变量

extern float icon_move_x;

/* 用户函数 ----------------------------------------------------------- */

void HugoUI::InitLayout(void)
{
    /* 注册 Page */
    Page *pageMain = AddPage(PAGE_ICON, "pageMain");
    Page *pageSetting = AddPage(PAGE_LIST, "pageSetting");
    Page *pagePID = AddPage(PAGE_LIST, "pagePID");
    Page *pageAnimation = AddPage(PAGE_LIST, "pageAnimation");
    Page *pageWS2812 = AddPage(PAGE_LIST, "pageWS2812");

    /* 注册 Item */
    // PageMain
    pageMain->AddItem(pageMain, "Setting", ITEM_JUMP_PAGE)
        ->SetJumpId(pageSetting->pageId, 0)
        ->SetIconSrc(Setting_BMP);

    pageMain->AddItem(pageMain, "BlueTeeth", ITEM_CALL_FUNCTION, nullptr)
        ->SetIconSrc(Knife_BMP);

    pageMain->AddItem(pageMain, "RealTime", ITEM_CALL_FUNCTION, nullptr)
        ->SetIconSrc(Cuteghost_BMP);

    pageMain->AddItem(pageMain, "RGB", ITEM_JUMP_PAGE)
        ->SetJumpId(pageWS2812->pageId, 0)
        ->SetIconSrc(Unicorn_BMP);

    pageMain->AddItem(pageMain, "???", ITEM_CALL_FUNCTION, nullptr)
        ->SetIconSrc(TreasureBox_BMP);

    pageMain->AddItem(pageMain, "MyFiles", ITEM_JUMP_PAGE)
        ->SetJumpId(pageFiles->pageId, 0)
        ->SetIconSrc(TreasureBox_BMP);

    pageMain->AddItem(pageMain, "RC522", ITEM_CALL_FUNCTION, nullptr)
        ->SetIconSrc(Cuteghost_BMP);

    pageMain->AddItem(pageMain, "About", ITEM_CALL_FUNCTION, EventShowAboutUI)
        ->SetIconSrc(Home_BMP);

    // PageSetting
    pageSetting->AddItem(pageSetting, "Setting", ITEM_PAGE_DESCRIPTION);
    pageSetting->AddItem(pageSetting, "PID Editor", ITEM_JUMP_PAGE)
        ->SetJumpId(pagePID->pageId, 0);

    pageSetting->AddItem(pageSetting, "InverseColor", ITEM_SWITCH, nullptr, nullptr);
    pageSetting->AddItem(pageSetting, "FilpScreen", ITEM_SWITCH, nullptr, nullptr);
    pageSetting->AddItem(pageSetting, "LightLevel", ITEM_CHANGE_VALUE, nullptr, nullptr);
    pageSetting->AddItem(pageSetting, "BeepEnable", ITEM_SWITCH, nullptr, nullptr);
    pageSetting->AddItem(pageSetting, "UASRT62Printf", ITEM_SWITCH, nullptr, nullptr);
    pageSetting->AddItem(pageSetting, "Page2List", ITEM_SWITCH, nullptr, nullptr);
    pageSetting->AddItem(pageSetting, "Volume Ctrl", ITEM_CHANGE_VALUE, nullptr, nullptr);
    pageSetting->AddItem(pageSetting, "SaveAll", ITEM_CALL_FUNCTION, nullptr);

    pageSetting->AddItem(pageSetting, "Exit", ITEM_JUMP_PAGE)
        ->SetJumpId(pageMain->pageId, 0);

    // PageAnimation
    pageAnimation->AddItem(pageAnimation, "Animation", ITEM_PAGE_DESCRIPTION);
    pageAnimation->AddItem(pageAnimation, "SmoothAnim", ITEM_SWITCH, nullptr, nullptr);
    pageAnimation->AddItem(pageAnimation, "-PlsSetSpeed--", ITEM_PAGE_DESCRIPTION);
    pageAnimation->AddItem(pageAnimation, "Fre_x", ITEM_CHANGE_VALUE, nullptr, nullptr);
    pageAnimation->AddItem(pageAnimation, "Fre_y", ITEM_CHANGE_VALUE, nullptr, nullptr);
    pageAnimation->AddItem(pageAnimation, "Fre_width", ITEM_CHANGE_VALUE, nullptr, nullptr);
    pageAnimation->AddItem(pageAnimation, "Slidbar_y", ITEM_CHANGE_VALUE, nullptr, nullptr);
    pageAnimation->AddItem(pageAnimation, "list_y", ITEM_CHANGE_VALUE, nullptr, nullptr);
    pageAnimation->AddItem(pageAnimation, "icon_x", ITEM_CHANGE_VALUE, nullptr, nullptr);
    pageAnimation->AddItem(pageAnimation, "Exit", ITEM_JUMP_PAGE)
        ->SetJumpId(pageMain->pageId, 3);

    // // PagePID
    // pagePID->AddItem(pagePID, "PID Editor", ITEM_PAGE_DESCRIPTION);
    // pagePID->AddItem(pagePID, "PID_Kp", ITEM_CHANGE_VALUE, nullptr, nullptr);
    // pagePID->AddItem(pagePID, "PID_Ki", ITEM_CHANGE_VALUE, nullptr, nullptr);
    // pagePID->AddItem(pagePID, "PID_Kd", ITEM_CHANGE_VALUE, nullptr, nullptr);
    // pagePID->AddItem(pagePID, "Exit", ITEM_JUMP_PAGE)
    //     ->SetJumpId(pageSetting->pageId, 1);

    // // PageWS2812
    // pageWS2812->AddItem(pageWS2812, "WS2812RGBSet", ITEM_PAGE_DESCRIPTION);
    // pageWS2812->AddItem(pageWS2812, "SetRed", ITEM_CHANGE_VALUE,nullptr, nullptr);
    // pageWS2812->AddItem(pageWS2812, "SetGreen", ITEM_CHANGE_VALUE, nullptr, nullptr);
    // pageWS2812->AddItem(pageWS2812, "SetBlue", ITEM_CHANGE_VALUE,nullptr, nullptr);
    // pageWS2812->AddItem(pageWS2812, "Exit", ITEM_JUMP_PAGE)
    //     ->SetJumpId(pageMain->pageId, 9);

}

/* About的应用事件函数 */
void EventShowAboutUI(void)
{
    static float motion_a = 80.0f, motion_a_trg = 0;

    oled_draw_bMP(motion_a, 0, 40, 50, HeadSculpture_BMP);
    oled_draw_str(50, FONT_HEIGHT, "HOPE  Pro");
    oled_draw_UTF8(45, FONT_HEIGHT * 2, "版本: Ver1.3");
    oled_draw_UTF8(45, FONT_HEIGHT * 3, "储存: 16 MB");
    oled_draw_UTF8(10, FONT_HEIGHT * 4, " By @kkl_aka科良");
    oled_set_draw_color(2);
    oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    oled_draw_R_box(86, 3, 30, FONT_HEIGHT, 0);
    oled_set_draw_color(1);

    HugoUI_Animation_Linear(&motion_a, &motion_a_trg, 85);

    if (ui_Key_num == 2)
    {
        motion_a = 80.0f;
        // icon_move_x = 128; 
    }
}
