#include "hugo_ui.h"

#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <algorithm>
#include <cstdio> 

using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */
std::string HugoUIVersion = "v3.0.0";

// 页面和Item列表
std::vector<Page::Ptr> pages;
std::vector<Item::Ptr> allItems;

/* 控制参数变量 ------------------------------------------------------- */
Page::Ptr currentPage;  // 当前Page
Page::Ptr lastPage;     // 上一个Page
Item::Ptr currentItem;  // 当前Item

/* 状态变量 ----------------------------------------------------------- */
static int16_t uiIndex = 0, uiSelect = 0;
static State uiState = State::None;

static uint8_t jumpPage_flag = 0;
static uint8_t ChangeVal_flag = 0;

/* 输入变量 ----------------------------------------------------------- */
uint8_t uiKeyNum = 0, uiEncoderNum = 0;
uint8_t uiKeyNumInSide = 0, uiEncoderNumInSide = 0;

/* 动画变量 ----------------------------------------------------------- */
float frame_y = 0.0f, frame_y_trg = 0.0f;
float frame_x = (float)SCREEN_WIDTH, frame_x_trg = 0.0f;
float frame_width = 36.0f, frame_width_trg = 36.0f;
float slidbar_y = 0.0f, slidbar_y_trg = 0.0f;

float icon_move_x = 0.0f, icon_move_x_trg = 48.0f;
float icon_desc_y = 0.0f, icon_desc_y_trg = 24.0f;
float icon_rectangle_x = 0.0f, icon_rectangle_x_trg = 13.0f;

uint8_t isItemFuncRunning = false;

/* 速率变量初始化 ----------------------------------------------------- */
Rate Rate5Hz{200, 0};
Rate Rate10Hz{100, 0};
Rate Rate20Hz{50, 0};
Rate Rate60Hz{17, 0};
Rate Rate50Hz{20, 0};
Rate Rate100Hz{10, 0};
Rate Rate125Hz{8, 0};
Rate Rate1000Hz{1, 0};
static unsigned int uiTimestamp = 0; // UI渲染时间戳（ms）

/* 动画函数 ----------------------------------------------------------- */

/**
 * @brief 线性动画插值
 * @param a: 当前值
 * @param a_trg: 目标值
 * @param n: 缓动系数
 * @retval 1-动画进行中，0-动画完成
 */
uint8_t HugoUI::Animation_Linear(float *a, float *a_trg, uint8_t n)
{
    if (fabs(*a - *a_trg) < 0.001f) 
    {
        *a = *a_trg;
        return 0;
    }

    if (fabs(*a - *a_trg) < 0.15f)
        *a = *a_trg;
    else
        *a += (*a_trg - *a) / (n / 10.0f);

    return 1;
}

/**
 * @brief 缓出动画插值
 */
uint8_t HugoUI::Animation_EasyOut(float *a, float *a_trg, uint16_t n)
{
    if (fabs(*a - *a_trg) < 0.001f)
    {
        *a = *a_trg;
        return 0;
    }

    float cz = fabs(*a - *a_trg);

    if (cz <= 1)
        *a = *a_trg;
    else
    {
        if (cz < 10)
            n = static_cast<uint16_t>(n * cz * 0.1f);
        if (n < 10)
            n = 10;
        *a += (*a_trg - *a) / (n * 0.1f);
    }
    return 1;
}

/**
 * @brief 缓入动画插值
 */
uint8_t HugoUI::Animation_EasyIn(float *a, float *a_trg, uint16_t n)
{
    if (fabs(*a - *a_trg) < 0.001f)
    {
        *a = *a_trg;
        return 0;
    }

    float cz = fabs(*a - *a_trg);
    if (cz <= 1)
        *a = *a_trg;
    else if (cz > 20)
        n = n * 3;
    else if (cz > 15)
        n = n * 2;
    else if (cz > 5)
        n = n * 1;
        
    *a += (*a_trg - *a) / (n * 0.1f);
        
    return 1;
}

/**
 * @brief 模糊转场效果
 */
uint8_t HugoUI::Animation_Blur(void)
{
    int len = 8 * oled_get_buffer_tile_height() * oled_get_buffer_tile_width();
    uint8_t *p = oled_get_buffer_ptr();
    uint8_t return_flag = 0;

    static uint8_t blur_effect_temp = 0; // UI模糊转场动效

    if (blur_effect_temp >= 6)
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 == 0)
                p[i] = p[i] & (0x55);
        }
    }
    if (blur_effect_temp >= 13)
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 != 0)
                p[i] = p[i] & (0xaa);
        }
    }
    if (blur_effect_temp >= 17)
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 == 0)
                p[i] = p[i] & (0x00);
        }
    }
    if (blur_effect_temp >= 20)
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 != 0)
                p[i] = p[i] & (0x00);
        }
    }
    
    blur_effect_temp += 1;
    if (blur_effect_temp > 21)
    {
        return_flag = 1;
        blur_effect_temp = 0;
    }
    
    return return_flag ? 0 : 1;
}

/* 工具函数 ----------------------------------------------------------- */

/**
 * @brief 频率控制函数
 * @param er: 频率配置
 * @retval 1-到达执行时间，0-未到达
 */
uint8_t HugoUI::ExecuteRate(Rate *er)
{
    uiTimestamp = get_ticks(); 
    
    if (er == nullptr) {
        return 0;
    }
    
    // 判断是否到达执行间隔
    if (uiTimestamp - er->last_timestamp >= er->executeT)
    {
        er->last_timestamp = uiTimestamp;  // 更新上次执行时间
        return 1;
    }
    else {
        return 0;
    }
}

/* 核心功能函数 ------------------------------------------------------- */

/**
 * @brief 添加Item到页面
 */
Item::Ptr Page::AddItem(const std::string& title, ItemType itemType, ...) 
{
    if (title.empty())
        return nullptr;
        
    // 创建新Item
    auto newItem = std::make_shared<Item>(itemType, title, this->pageId);
    
    // 设置ID
    newItem->itemId = static_cast<uint16_t>(allItems.size());
    newItem->lineId = static_cast<uint16_t>(this->items.size());
    
    // 添加到页面和全局列表
    this->items.push_back(newItem);
    allItems.push_back(newItem);
    
    // 更新Item数量
    this->itemMax = static_cast<uint16_t>(this->items.size());
    
    // 处理可变参数
    va_list variableArg;
    va_start(variableArg, itemType);

    switch (itemType)
    {
    case ItemType::JumpPage:
        newItem->JumpPageId = static_cast<uint8_t>(va_arg(variableArg, int));
        newItem->JumpItemLineId = static_cast<uint8_t>(va_arg(variableArg, int));
        break;
        
    case ItemType::Checkbox:
    case ItemType::Switch:
        newItem->flag = va_arg(variableArg, bool *);
        newItem->FuncCallBack = va_arg(variableArg, void (*)(void));
        break;
        
    case ItemType::ChangeValue:
        newItem->param = va_arg(variableArg, paramType *);
        newItem->FuncCallBack = va_arg(variableArg, void (*)(void));
        break;
        
    case ItemType::Message:
        newItem->msg = va_arg(variableArg, char *);
        break;
        
    case ItemType::CallFunction:
        newItem->FuncCallBack = va_arg(variableArg, void (*)(void));
        break;
        
    default:
        break;
    }
    
    va_end(variableArg);

    return newItem;
}

/**
 * @brief 添加新页面
 */
Page::Ptr HugoUI::AddPage(PageType mode, const std::string& name)
{
    if (name.empty())
        return nullptr;
        
    // 创建新Page
    auto newPage = std::make_shared<Page>(mode, name);
    
    // 设置页面ID
    newPage->pageId = static_cast<uint8_t>(pages.size());
    
    // 添加到页面列表
    pages.push_back(newPage);
    
    return newPage;
}

/**
 * @brief 通用列表页面显示
 */
void HugoUI::CommonListShow(Page* thispage, Item* thisitem)
{
    if (!thispage || !thisitem)
        return;

    // 计算动画
    Animation_Linear(&thispage->page_y_forlist, &thispage->page_y_forlist_trg, 65);
    Animation_Linear(&frame_y, &frame_y_trg, 65);
    Animation_Linear(&frame_width, &frame_width_trg, 65);
    Animation_Linear(&slidbar_y, &slidbar_y_trg, 65);

    // 绘制目录树和目录名
    int16_t Item_x = static_cast<int16_t>(thispage->page_x);
    int16_t Item_y;
    
    // 遍历页面Item
    for (const auto& item : thispage->items)
    {
        if (!item) continue;

        // 绘制滚动条分隔线
        int16_t bar_y = static_cast<int16_t>(thispage->page_y + item->lineId * ceil((float)SCREEN_HEIGHT / thispage->itemMax));
        if (item->lineId % 2 == 0)
        {
            oled_draw_line(Item_x + SCREEN_WIDTH - 3, bar_y,
                           Item_x + SCREEN_WIDTH - 1, bar_y);
        }
        else
        {
            oled_draw_line(Item_x + SCREEN_WIDTH - 3, bar_y,
                           Item_x + SCREEN_WIDTH - 2, bar_y);
        }

        // 不在可视区域则跳过
        if (item->lineId <= uiSelect - 5 || item->lineId >= uiSelect + 6)
            continue;

        Item_y = FONT_HEIGHT - 1 + static_cast<int16_t>(thispage->page_y + item->lineId * FONT_HEIGHT + thispage->page_y_forlist);
        
        switch (item->funcType)
        {
        case ItemType::Description:
            oled_draw_str(2 + Item_x, Item_y, "-");
            oled_draw_str(2 + 9 + Item_x, Item_y, item->title.c_str());
            break;
            
        case ItemType::JumpPage:
            oled_draw_str(2 + Item_x, Item_y, "+");
            oled_draw_str(2 + 10 + Item_x, Item_y, item->title.c_str());
            break;
            
        case ItemType::Checkbox:
            oled_draw_str(2 + Item_x, Item_y, "-");
            oled_draw_str(2 + 9 + Item_x, Item_y, item->title.c_str());

            // 绘制勾选标记
            if (item->flag && *item->flag)
            {
                oled_set_draw_color(2);
                oled_draw_str(SCREEN_WIDTH - 18 + Item_x, Item_y + 1, "*");
                oled_set_draw_color(1);
            }

            // 绘制勾选框
            oled_draw_frame(SCREEN_WIDTH - 20 + Item_x, Item_y - 12 + 3, 11, 11);

            // 当前选项高亮
            if (item->lineId == uiSelect)
            {
                oled_set_draw_color(2);
                oled_draw_box(SCREEN_WIDTH - 21 + Item_x, Item_y - 12 + 2, 12, 12);
                oled_set_draw_color(1);
            }
            break;

        case ItemType::Switch:
            oled_draw_str(2 + Item_x, Item_y, "-");
            oled_draw_str(2 + 9 + Item_x, Item_y, item->title.c_str());

            // 显示On/Off
            oled_draw_str(SCREEN_WIDTH - FONT_WIDTH * 3 + Item_x, Item_y, 
                          (item->flag && *item->flag) ? "On" : "Off");
            break;

        case ItemType::ChangeValue:
            oled_draw_str(2 + Item_x, Item_y, "-");
            oled_draw_str(2 + 9 + Item_x, Item_y, item->title.c_str());
            
            // 显示数值
            if (item->param) {
                char float_str[20] = {0};
                #ifdef FPU
                    sprintf(float_str, "%.1f", *item->param);
                #else
                    sprintf(float_str, "%d", *item->param);
                #endif
                
                int16_t val_x = (*item->param < 100) ? (SCREEN_WIDTH - FONT_WIDTH * 4 + Item_x) : (SCREEN_WIDTH - FONT_WIDTH * 5 + Item_x);
                oled_draw_str(val_x, Item_y, float_str);

                // 当前数值高亮
                if (item->lineId == uiSelect && ChangeVal_flag)
                {
                    oled_set_draw_color(2);
                    int16_t box_x = (*item->param < 0) ? (SCREEN_WIDTH - FONT_WIDTH * 7 + Item_x) : (SCREEN_WIDTH - FONT_WIDTH * 5 + Item_x);
                    int16_t box_w = (*item->param < 0) ? (FONT_WIDTH * 6) : (FONT_WIDTH * 4);
                    oled_draw_box(box_x, Item_y - 12 + 2, box_w, 11);
                    oled_set_draw_color(1);
                }
            }
            break;

        default:
            oled_draw_str(2 + Item_x, Item_y, "-");
            oled_draw_str(2 + 9 + Item_x, Item_y, item->title.c_str());
            break;
        }
    }

    // 绘制选择框（反色）
    oled_set_draw_color(2);
    oled_draw_R_box(static_cast<int16_t>(thispage->page_x), 
                    static_cast<int16_t>(thispage->page_y + frame_y), 
                    static_cast<int16_t>(thispage->page_x + frame_width + 5), 
                    FONT_HEIGHT + 2, 0);
    oled_set_draw_color(1);

    // 绘制滚动条
    oled_draw_line(static_cast<int16_t>(thispage->page_x + SCREEN_WIDTH - 2), 
                   static_cast<int16_t>(thispage->page_y), 
                   static_cast<int16_t>(thispage->page_x + SCREEN_WIDTH - 2), 
                   static_cast<int16_t>(thispage->page_y + SCREEN_HEIGHT));
    
    // 绘制滚动条滑块
    oled_draw_box(static_cast<int16_t>(thispage->page_x + SCREEN_WIDTH - 3), 
                  static_cast<int16_t>(thispage->page_y + slidbar_y), 
                  SCROLL_BAR_WIDTH, 
                  static_cast<int16_t>(ceil((float)SCREEN_HEIGHT / thispage->itemMax)));

    // 页面状态处理
    switch (uiState)
    {
    case State::None: 
        break;
        
    case State::RunPageDown:
        // 列表滚动
        if (uiSelect >= SCREEN_HEIGHT / 16)
            thispage->page_y_forlist_trg -= FONT_HEIGHT;
        if (uiSelect == 0)
            thispage->page_y_forlist_trg = 0;

        // 选择框滚动
        frame_y = frame_y_trg - FONT_HEIGHT * 1.5f;
        if (uiSelect < SCREEN_HEIGHT / 16)
        {
            frame_y_trg += FONT_HEIGHT;
        }
        if (uiSelect == 0)
            frame_y_trg = 0;

        frame_width_trg = oled_get_UTF8_width(thisitem->title.c_str()) + FONT_WIDTH;

        // 滚动条滑块
        slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / thispage->itemMax);

        uiState = State::None;
        break;
        
    case State::RunPageUp:
        // 列表滚动
        if (frame_y_trg == 0)
            thispage->page_y_forlist_trg = -uiSelect * FONT_HEIGHT;

        // 选择框滚动
        frame_y = frame_y_trg + FONT_HEIGHT * 1.5f;
        frame_y_trg -= FONT_HEIGHT;
        if (frame_y_trg <= 0)
            frame_y_trg = 0;
            
        frame_width_trg = oled_get_UTF8_width(thisitem->title.c_str()) + FONT_WIDTH;

        // 滚动条滑块
        slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / thispage->itemMax);

        uiState = State::None;
        break;
        
    case State::ReadyToJumpPage:
        // 页面位置
        thispage->page_x_trg = 0;
        thispage->page_x = 100;

        // 选择框位置
        frame_y = SCREEN_HEIGHT * 1.5f;
        frame_y_trg = (uiSelect % (SCREEN_HEIGHT / 16)) * FONT_HEIGHT;
        frame_width_trg = oled_get_UTF8_width(thisitem->title.c_str()) + FONT_WIDTH;

        // 滚动条位置
        slidbar_y = SCREEN_HEIGHT;
        slidbar_y_trg = uiSelect * (SCREEN_HEIGHT / (thispage->itemMax + 1));

        // 列表位置调整
        if ((uiSelect % (SCREEN_HEIGHT / 16)) - 1)
        {
            if (thispage->page_y_forlist_trg / FONT_HEIGHT == -(uiSelect - 1))
                thispage->page_y_forlist_trg -= FONT_HEIGHT;
            else
                thispage->page_y_forlist_trg -= FONT_HEIGHT * (SCREEN_HEIGHT / 16);
        }

        if (uiSelect == 0)
            thispage->page_y_forlist_trg = 0;

        uiState = State::JumpPage;
        break;
        
    case State::JumpPage:
        if (Animation_EasyOut(&thispage->page_x, &thispage->page_x_trg, 85) == 0)
        {
            jumpPage_flag |= 0x0f;
        }

        if((jumpPage_flag & 0xf0) != 0xf0)
        {
            oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            if(Animation_Blur() == 0)
            {
                jumpPage_flag |= 0xf0;
            }
        }

        if (jumpPage_flag == 0xff)
        {
            jumpPage_flag = 0;
            uiState = State::JumpPageArrive;
        }
        break;
        
    case State::JumpPageArrive:
        frame_y_trg = (uiSelect % (SCREEN_HEIGHT / 16)) * FONT_HEIGHT;
        frame_width_trg = oled_get_UTF8_width(thisitem->title.c_str()) + FONT_WIDTH;
        slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / thispage->itemMax);

        uiState = State::None;
        break;

    default:
        uiState = State::None;
        break;
    }
}

/**
 * @brief 通用Icon页面显示
 */
void HugoUI::CommonIconShow(Page* thispage, Item* thisitem)
{
    if (!thispage || !thisitem)
        return;

    // 计算动画
    Animation_Linear(&thispage->page_x, &thispage->page_x_trg, 65);
    Animation_Linear(&frame_x, &frame_x_trg, 65);
    Animation_Linear(&icon_move_x, &icon_move_x_trg, 75);
    Animation_Linear(&icon_rectangle_x, &icon_rectangle_x_trg, 65);
    
    // 绘制参数
    int16_t Item_y = static_cast<int16_t>(thispage->page_y);
    int16_t Item_x;

    oled_set_bitmap_mode(1);
    
    // 遍历Item
    for (const auto& item : thispage->items)
    {
        if (!item) continue;
        
        // 不在可视区域则跳过
        if (item->lineId <= uiSelect - 5 || item->lineId >= uiSelect + 6)
            continue;

        Item_x = 48 + static_cast<int16_t>(thispage->page_x + icon_move_x * item->lineId);

        // 绘制图标
        if (item->pic) {
            oled_draw_bMP(Item_x, Item_y, 32, 32, item->pic);
        }
    }
    
    oled_set_bitmap_mode(0);

    // 绘制标题
    oled_set_font(u8g2_font_luBS14_tr);
    int16_t title_x = (SCREEN_WIDTH - oled_get_UTF8_width(thisitem->title.c_str())) / 2;
    int16_t title_y = Item_y + SCREEN_HEIGHT / 2 + 22 + (FONT_HEIGHT - static_cast<int16_t>(icon_rectangle_x));
    oled_draw_str(title_x, title_y, thisitem->title.c_str());
    oled_set_font(u8g2_font_wqy13_t_gb2312a);

    // 绘制选择框
    oled_set_draw_color(2);
    oled_draw_R_box(48 + static_cast<int16_t>(frame_x), 
                    static_cast<int16_t>(thispage->page_y), 
                    32, 32, 0);
    
    // 绘制矩形
    oled_draw_box(0, 
                  static_cast<int16_t>(thispage->page_y + SCREEN_HEIGHT / 2 + 4), 
                  static_cast<int16_t>(icon_rectangle_x), 24);
    oled_set_draw_color(1);

    // 状态处理
    switch (uiState)
    {
    case State::None: 
        break;
        
    case State::RunPageDown:
        thispage->page_x_trg = -(uiSelect * 48);
        frame_x -= 24;

        icon_rectangle_x = 0;
        if (uiSelect == 0)
        {
            icon_move_x = 0;
        }

        uiState = State::None;
        break;
        
    case State::RunPageUp:
        thispage->page_x_trg = -(uiSelect * 48);
        frame_x += 24;

        icon_rectangle_x = 0;

        uiState = State::None;
        break;
        
    case State::ReadyToJumpPage:
        // 页面位置
        thispage->page_y_trg = 0;
        thispage->page_y = 40;

        icon_move_x = 160;
        frame_x = 160;
        frame_y = SCREEN_HEIGHT * 1.5f;

        thispage->page_x_trg = -(uiSelect * 48);
        icon_rectangle_x = 0;

        uiState = State::JumpPage;
        break;
        
    case State::JumpPage:
        if (Animation_EasyIn(&thispage->page_y, &thispage->page_y_trg, 75) == 0)
        {
            jumpPage_flag |= 0x0f;
        }

        if((jumpPage_flag & 0xf0) != 0xf0)
        {
            oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            if(Animation_Blur() == 0)
            {
                jumpPage_flag |= 0xf0;
            }
        }

        if (jumpPage_flag == 0xff)
        {
            jumpPage_flag = 0;
            uiState = State::JumpPageArrive;
        }
        break;
        
    case State::JumpPageArrive:
        thispage->page_x_trg = -(uiSelect * 48);
        uiState = State::None;
        break;

    default:
        uiState = State::None;
        break;
    }
}

/**
 * @brief 通用事件处理
 */
void HugoUI::CommonEventProc(void)
{
    if (uiState != State::None || !currentPage || !currentItem)
        return;

    /* 编码器处理 */
    if (uiEncoderNumInSide == 1) // 向下
    {
        if (ChangeVal_flag && currentItem->param)
        {
            // 数值修改逻辑
            #ifdef FPU
                *currentItem->param += 0.1f;
            #else
                *currentItem->param += 1;
            #endif
            
            // 执行回调
            if (currentItem->FuncCallBack)
                currentItem->FuncCallBack();
        }
        else
        {
            uiSelect++;
            uiState = State::RunPageDown;
            
            // 边界检查
            if (uiSelect >= static_cast<int16_t>(currentPage->items.size()))
                uiSelect = 0;
        }
    }
    else if (uiEncoderNumInSide == 2) // 向上
    {
        if (ChangeVal_flag && currentItem->param)
        {
            // 数值修改逻辑
            #ifdef FPU
                *currentItem->param -= 0.1f;
            #else
                *currentItem->param -= 1;
            #endif
            
            // 执行回调
            if (currentItem->FuncCallBack)
                currentItem->FuncCallBack();
        }
        else
        {
            uiSelect--;
            uiState = State::RunPageUp;
            
            // 边界检查
            if (uiSelect < 0)
                uiSelect = static_cast<int16_t>(currentPage->items.size()) - 1;
        }
    }
    
    /* 按键处理 */
    if (uiKeyNumInSide == 1) // 短按
    {
        switch (currentItem->funcType)
        {
        case ItemType::JumpPage:
            // 页面跳转
            uiState = State::ReadyToJumpPage;
            uiIndex = currentItem->JumpPageId;
            uiSelect = currentItem->JumpItemLineId;
            lastPage = currentPage;
            break;
            
        case ItemType::CallFunction:
            isItemFuncRunning = true;
            break;
            
        case ItemType::Checkbox:
        case ItemType::Switch:
            // 反转状态
            if (currentItem->flag)
                *currentItem->flag = !*currentItem->flag;
                
            // 执行回调
            if (currentItem->FuncCallBack)
                currentItem->FuncCallBack();
            break;
            
        case ItemType::ChangeValue:
            ChangeVal_flag = !ChangeVal_flag;
            break;
            
        default:
            break;
        }
    }
    else if (uiKeyNumInSide == 2) // 长按
    {
        // 返回上一页
        if (lastPage)
        {
            uiState = State::ReadyToJumpPage;
            uiIndex = lastPage->pageId;
            uiSelect = 0;
            lastPage = currentPage;
            ChangeVal_flag = 0;
        }
    }
}

/**
 * @brief UI主任务处理
 */
void HugoUI::TaskHandler(void)
{
    uiKeyNumInSide = uiKeyNum, uiEncoderNumInSide = uiEncoderNum;
    uiKeyNum = 0, uiEncoderNum = 0;

    // 更新当前页面
    if (!currentPage || currentPage->pageId != uiIndex)
    {
        if (uiIndex >= 0 && uiIndex < static_cast<int16_t>(pages.size()))
        {
            currentPage = pages[uiIndex];
        }
    }

    // 更新当前Item
    if (currentPage && (!currentItem || currentItem->lineId != uiSelect || currentItem->inPage != currentPage->pageId))
    {
        if (uiSelect >= 0 && uiSelect < static_cast<int16_t>(currentPage->items.size()))
        {
            currentItem = currentPage->items[uiSelect];
        }
    }

    // Item回调执行
    if (isItemFuncRunning && currentItem)
    {
        oled_clear_buffer();

        if (currentItem->FuncCallBack)
            currentItem->FuncCallBack();
        else
            isItemFuncRunning = false;

        // 长按退出
        if (uiKeyNumInSide == 2)
            isItemFuncRunning = false;

        oled_send_buffer();
    }
    // 页面显示
    else if (currentPage && currentItem)
    {

        if (ExecuteRate(&Rate60Hz)) // 60Hz刷新
        {   
            oled_clear_buffer();
            
            // 根据页面类型显示
            if (currentPage->funcType == PageType::List) {
                CommonListShow(currentPage.get(), currentItem.get());
            } 
            else if (currentPage->funcType == PageType::Icon) {
                CommonIconShow(currentPage.get(), currentItem.get());
            } 
            else if (currentPage->funcType == PageType::Custom) {
                if (currentPage->PageUIShow) {
                    currentPage->PageUIShow(currentPage.get(), currentItem.get());
                } else {
                    CommonListShow(currentPage.get(), currentItem.get());
                }
            }

            oled_send_buffer();
        }

        // 页面回调
        if (currentPage->FuncCallBack)
            currentPage->FuncCallBack();

        // 页面事件处理
        if (currentPage->PageEventProc)
            currentPage->PageEventProc();
        else
            CommonEventProc();
    }
}