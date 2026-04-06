#include "hugo_ui.h"
#include "hugo_ui_page_list.h"
#include "hugo_ui_page_icon.h"
#include "hugo_ui_widget.h"

#include <cstdio> 
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <algorithm>

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
int16_t uiIndex = 0, uiSelect = 0;
State uiState = State::None;

uint8_t changeValFlag = 0;

/* 输入变量 ----------------------------------------------------------- */
uint8_t uiKeyNum = 0, uiEncoderNum = 0;
uint8_t uiKeyNumInSide = 0, uiEncoderNumInSide = 0;

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
    if (name.empty()) return nullptr;
    
    Page::Ptr newPage;
    
    // 根据PageType创建对应派生类的实例
    switch (mode) {
        case PageType::List:
            newPage = std::make_shared<PageList>(name);
            break;
        case PageType::Icon:
            newPage = std::make_shared<PageIcon>(name);
            break;
        // case PageType::Custom:
        //     newPage = std::make_shared<PageCustom>(name);
        //     break;
        default:
            newPage = std::make_shared<PageList>(name);
            break;
    }
    
    // 设置页面ID
    newPage->pageId = static_cast<uint8_t>(pages.size());
    
    // 添加到页面列表
    pages.push_back(newPage);
    
    return newPage;
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
        if (changeValFlag && currentItem->param)
        {
            // 数值修改逻辑
            #ifdef FPU
                *currentItem->param += 0.1f;
            #else
                *currentItem->param += 1;
            #endif
                *currentItem->param = *currentItem->param < 100 ? *currentItem->param : 100;

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
        if (changeValFlag && currentItem->param)
        {
            // 数值修改逻辑
            #ifdef FPU
                *currentItem->param -= 0.1f;
            #else
                *currentItem->param -= 1;
            #endif
                *currentItem->param = *currentItem->param > 0 ? *currentItem->param : 0;

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
            changeValFlag = !changeValFlag;
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
            changeValFlag = 0;
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
            
            // 根据页面类型进行渲染
            currentPage->Show(currentItem.get());

            // widget控件渲染
            WidgetShow();

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