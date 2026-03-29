#ifndef __HUGO_UI_H
#define __HUGO_UI_H

#include <memory>
#include <vector>
#include <string>
#include <cstdarg>  

#include "main.h"
#include "oled_wrapper.h"
#include "hugo_ui_animation.h"

/* define ------------------------------------------------------- */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define FONT_WIDTH 8
#define FONT_HEIGHT 14
#define SCROLL_BAR_WIDTH 3

#ifdef FPU
typedef float       paramType; 
#else
typedef int32_t     paramType;
#endif

/* 封装宏 --------------------------------------------------- */
#define oled_set_font(font) u8g2_SetFont(&u8g2, font)
#define oled_draw_str(x, y, str) u8g2_DrawStr(&u8g2, x, y, str)
#define oled_draw_UTF8(x, y, str) u8g2_DrawUTF8(&u8g2, x, y, str)
#define oled_get_str_width(str) u8g2_GetStrWidth(&u8g2, str)
#define oled_get_UTF8_width(str) u8g2_GetUTF8Width(&u8g2, str)
#define oled_get_str_height() u8g2_GetMaxCharHeight(&u8g2)
#define oled_draw_pixel(x, y) u8g2_DrawPixel(&u8g2, x, y)
#define oled_draw_circle(x, y, r) u8g2_DrawCircle(&u8g2, x, y, r, U8G2_DRAW_ALL)
#define oled_draw_R_box(x, y, w, h, r) u8g2_DrawRBox(&u8g2, x, y, w, h, r)
#define oled_draw_box(x, y, w, h) u8g2_DrawBox(&u8g2, x, y, w, h)
#define oled_draw_frame(x, y, w, h) u8g2_DrawFrame(&u8g2, x, y, w, h)
#define oled_draw_R_frame(x, y, w, h, r) u8g2_DrawRFrame(&u8g2, x, y, w, h, r)
#define oled_draw_H_line(x, y, l) u8g2_DrawHLine(&u8g2, x, y, l)
#define oled_draw_V_line(x, y, h) u8g2_DrawVLine(&u8g2, x, y, h)
#define oled_draw_line(x1, y1, x2, y2) u8g2_DrawLine(&u8g2, x1, y1, x2, y2)
#define oled_draw_H_dotted_line(x, y, l) u8g2_DrawHDottedLine(&u8g2, x, y, l)
#define oled_draw_V_dotted_line(x, y, h) u8g2_DrawVDottedLine(&u8g2, x, y, h)
#define oled_draw_bMP(x, y, w, h, bitMap) u8g2_DrawXBMP(&u8g2, x, y, w, h, bitMap)
#define oled_set_draw_color(color) u8g2_SetDrawColor(&u8g2, color)
#define oled_set_font_mode(mode) u8g2_SetFontMode(&u8g2, mode)
#define oled_set_font_direction(dir) u8g2_SetFontDirection(&u8g2, dir)
#define oled_clear_buffer() u8g2_ClearBuffer(&u8g2)
#define oled_send_buffer() u8g2_SendBuffer(&u8g2)
#define oled_send_area_buffer(x, y, w, h) u8g2_UpdateDisplayArea(&u8g2, x, y, w, h)

#define oled_set_bitmap_mode(mode) u8g2_SetBitmapMode(&u8g2, mode)
#define oled_get_buffer_tile_height() (u8g2_GetBufferTileHeight(&u8g2))
#define oled_get_buffer_tile_width() (u8g2_GetBufferTileWidth(&u8g2))
#define oled_get_buffer_ptr() (u8g2_GetBufferPtr(&u8g2))

#define get_ticks() HAL_GetTick()
#define delay(ms) HAL_Delay(ms)

namespace HugoUI
{

/* enum --------------------------------------------------- */
enum class State {
    None = 0,             // 当前页面无状态
    RunPageDown,          // 当前页面上滑（左滑）
    RunPageUp,            // 当前页面下滑（右滑）
    ReadyToJumpPage,      // 当前页面准备跳转（在此状态设置跳转参数）
    JumpPage,             // 当前页面进行跳转
    JumpPageArrive        // 当前页面跳转结束（在此状态设置跳转结束参数）
};

enum class ItemType {
    Description,       // 描述
    CallFunction,      // 函数回调
    JumpPage,          // 页面跳转
    Switch,            // 开关
    ChangeValue,       // 改变值
    Checkbox,          // 勾选
    Message            // 消息
};

enum class PageType {
    List,          // 该页是列表
    Icon,          // 该页是ICON
    Custom         // 该页是定做
};

/* class --------------------------------------------------- */
class Item : public std::enable_shared_from_this<Item> {
public:
    using Ptr = std::shared_ptr<Item>;
    
    ItemType funcType;      // 作用类型
    uint16_t itemId;        // 全局item ID
    uint16_t lineId;        // 在所在页中的ID
    const uint8_t* pic;     // 图片
    std::string title;      // 名称
    std::string msg;        // 消息内容
    bool* flag;             // 开关/勾选标志位
    paramType* param;       // 可改变的参数
    uint8_t inPage;         // 所属页面ID
    uint8_t JumpPageId;     // 跳转页面ID
    uint8_t JumpItemLineId; // 跳转Item LineID
    void (*FuncCallBack)(void);   // 回调函数

    // 构造函数
    Item(ItemType type = ItemType::Description, 
         const std::string& itemTitle = "",
         uint8_t pageId = 0) 
        : funcType(type), 
          title(itemTitle),
          inPage(pageId),
          itemId(0),
          lineId(0),
          pic(nullptr),
          flag(nullptr),
          param(nullptr),
          JumpPageId(0),
          JumpItemLineId(0),
          FuncCallBack(nullptr) {}

    // 设置图标
    Ptr SetIconSrc(const uint8_t* pic) {
        this->pic = pic;
        return shared_from_this(); // 修复类型转换错误
    }

    // 设置跳转ID
    Ptr SetJumpId(uint8_t pageId, uint8_t itemLineId) {
        this->JumpPageId = pageId;
        this->JumpItemLineId = itemLineId;
        return shared_from_this(); // 修复类型转换错误
    }

    // 返回自身
    Ptr ReturnThisItem() {
        return shared_from_this(); // 修复类型转换错误
    }
};

class Page : public std::enable_shared_from_this<Page> {
public:
    using Ptr = std::shared_ptr<Page>;
    
    PageType funcType;      // 页面类型
    std::string title;      // 页面名称
    uint8_t pageId;         // 页面ID
    uint16_t itemMax;       // item数量
    float page_x, page_x_trg;
    float page_y, page_y_trg;
    
    // Item列表
    std::vector<Item::Ptr> items;
    
    // 回调函数
    void (*FuncCallBack)(void);
    void (*PageEventProc)(void);

    // 构造函数
    Page(PageType type = PageType::List, 
         const std::string& pageTitle = "") 
        : funcType(type), 
          title(pageTitle),
          pageId(0),
          itemMax(0),
          page_x(0), page_x_trg(0),
          page_y(0), page_y_trg(0),
          FuncCallBack(nullptr),
          PageEventProc(nullptr) {}

    // 纯虚函数 - 强制派生类实现
    virtual void Show(Item* thisitem) = 0;

    // 添加Item
    Item::Ptr AddItem(const std::string& title, ItemType itemType, ...);

    // 设置页面回调
    Ptr SetPageFunCallBack(void (*cbFunc)(void)) {
        this->FuncCallBack = cbFunc;
        return shared_from_this();
    }

    // 设置页面事件处理
    Ptr SetPageEventProc(void (*eventProc)(void)) {
        this->PageEventProc = eventProc;
        return shared_from_this();
    }

    // 虚析构函数（必须）
    virtual ~Page() = default;
};

// 执行频率结构体
struct Rate {
    unsigned int executeT;    // 执行周期（ms）
    unsigned int last_timestamp; // 上次执行时间戳
    
    Rate(unsigned int t = 0, unsigned int ts = 0) 
        : executeT(t), last_timestamp(ts) {}
};

/* API-Function ------------------------------------------------------- */

Page::Ptr AddPage(PageType mode, const std::string& name);

void TaskHandler(void);
uint8_t ExecuteRate(Rate *er);
void CommonEventProc(void);

} /* namespace HugoUI */

/* 声明变量 ------------------------------------------------------- */
extern HugoUI::Rate Rate5Hz;
extern HugoUI::Rate Rate10Hz;
extern HugoUI::Rate Rate20Hz;
extern HugoUI::Rate Rate50Hz;
extern HugoUI::Rate Rate60Hz;
extern HugoUI::Rate Rate100Hz;
extern HugoUI::Rate Rate125Hz;
extern HugoUI::Rate Rate1000Hz;

extern uint8_t uiKeyNum, uiEncoderNum;
extern uint8_t uiKeyNumInSide, uiEncoderNumInSide;

extern int16_t uiIndex, uiSelect;
extern HugoUI::State uiState;

extern HugoUI::Page::Ptr currentPage; // 当前Page
extern HugoUI::Page::Ptr lastPage;    // 上一个Page
extern HugoUI::Item::Ptr currentItem; // 当前Item

extern uint8_t ChangeVal_flag;

#endif // __HUGO_UI_H