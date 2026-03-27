#include "hugo_ui_page_list.h"

#include <cmath>

using namespace HugoUI;

/**
 * @brief 通用列表页面显示
 */
void PageList::Show(Item* thisitem)
{
    if (!thisitem) return;

    // 计算动画
    Animation_Linear(&this->list_y, &this->list_y_trg, 85);
    Animation_Linear(&this->frame_y, &this->frame_y_trg, 65);
    Animation_Linear(&this->frame_width, &this->frame_width_trg, 40);
    Animation_Linear(&this->slidbar_y, &this->slidbar_y_trg, 65);

    // 绘制目录树和目录名
    int16_t Item_x = static_cast<int16_t>(this->page_x);
    int16_t Item_y;
    
    // 遍历页面Item
    for (const auto& item : this->items)
    {
        if (!item) continue;

        // 绘制滚动条分隔线
        int16_t bar_y = static_cast<int16_t>(this->page_y + item->lineId * ceil((float)SCREEN_HEIGHT / this->itemMax));
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

        Item_y = FONT_HEIGHT - 1 + static_cast<int16_t>(this->page_y + item->lineId * FONT_HEIGHT + this->list_y);
        
        switch (item->funcType)
        {
        case ItemType::Description:
            oled_draw_str(2 + Item_x, Item_y, "#");
            oled_draw_UTF8(2 + 9 + Item_x, Item_y, item->title.c_str());
            break;
            
        case ItemType::JumpPage:
            oled_draw_str(2 + Item_x, Item_y, "+");
            oled_draw_UTF8(2 + 10 + Item_x, Item_y, item->title.c_str());
            break;
            
        case ItemType::Checkbox:
            oled_draw_str(2 + Item_x, Item_y, "-");
            oled_draw_UTF8(2 + 9 + Item_x, Item_y, item->title.c_str());

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
            oled_draw_UTF8(2 + 9 + Item_x, Item_y, item->title.c_str());

            // 显示On/Off
            oled_draw_str(SCREEN_WIDTH - FONT_WIDTH * 3 + Item_x, Item_y, 
                          (item->flag && *item->flag) ? "On" : "Off");
            break;

        case ItemType::ChangeValue:
            oled_draw_str(2 + Item_x, Item_y, "-");
            oled_draw_UTF8(2 + 9 + Item_x, Item_y, item->title.c_str());

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
            oled_draw_UTF8(2 + 9 + Item_x, Item_y, item->title.c_str());
            break;
        }
    }

    // 绘制选择框（反色）
    oled_set_draw_color(2);
    oled_draw_R_box(static_cast<int16_t>(this->page_x), 
                    static_cast<int16_t>(this->page_y + this->frame_y), 
                    static_cast<int16_t>(this->page_x + this->frame_width + 5), 
                    FONT_HEIGHT + 2, 0);
    oled_set_draw_color(1);

    // 绘制滚动条
    oled_draw_line(static_cast<int16_t>(this->page_x + SCREEN_WIDTH - 2), 
                   static_cast<int16_t>(this->page_y), 
                   static_cast<int16_t>(this->page_x + SCREEN_WIDTH - 2), 
                   static_cast<int16_t>(this->page_y + SCREEN_HEIGHT));
    
    // 绘制滚动条滑块
    oled_draw_box(static_cast<int16_t>(this->page_x + SCREEN_WIDTH - 3), 
                  static_cast<int16_t>(this->page_y + this->slidbar_y), 
                  SCROLL_BAR_WIDTH, 
                  static_cast<int16_t>(ceil((float)SCREEN_HEIGHT / this->itemMax)));

    // 页面状态处理
    switch (uiState)
    {
    case State::None: 
        break;
        
    case State::RunPageDown:
        // 列表滚动
        if (uiSelect >= SCREEN_HEIGHT / 16)
            this->list_y_trg = -(uiSelect - 3) * FONT_HEIGHT;
        else
            this->list_y_trg = 0;

        // 选择框滚动
        this->frame_y = this->frame_y_trg - FONT_HEIGHT * 1.5f;
        if (uiSelect < SCREEN_HEIGHT / 16)
            this->frame_y_trg = uiSelect * FONT_HEIGHT;
        else
            this->frame_y_trg = 3 * FONT_HEIGHT;

        // if (uiSelect == 0)
        //     this->frame_y_trg = 0;

        this->frame_width_trg = oled_get_UTF8_width(thisitem->title.c_str()) + FONT_WIDTH;

        // 滚动条滑块
        this->slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / this->itemMax);

        uiState = State::None;
        break;
        
    case State::RunPageUp:

        // 列表滚动
        if (uiSelect >= SCREEN_HEIGHT / 16)
            this->list_y_trg = -(uiSelect - 3) * FONT_HEIGHT;
        else
            this->list_y_trg = 0;

        // 选择框滚动
        this->frame_y = this->frame_y_trg + FONT_HEIGHT * 1.5f;
        if (uiSelect < SCREEN_HEIGHT / 16)
            this->frame_y_trg = uiSelect * FONT_HEIGHT;
        else
            this->frame_y_trg = 3 * FONT_HEIGHT;

        this->frame_width_trg = oled_get_UTF8_width(thisitem->title.c_str()) + FONT_WIDTH;

        // 滚动条滑块
        this->slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / this->itemMax);

        uiState = State::None;
        break;
        
    case State::ReadyToJumpPage:
        // 页面位置
        this->page_x_trg = 0;
        this->page_x = 100;

        // // 选择框位置
        // this->frame_y = SCREEN_HEIGHT * 1.5f;
        // this->frame_y_trg = (uiSelect % (SCREEN_HEIGHT / 16)) * FONT_HEIGHT;
        // this->frame_width_trg = oled_get_UTF8_width(thisitem->title.c_str()) + FONT_WIDTH;
        // // 列表位置调整
        // if ((uiSelect % (SCREEN_HEIGHT / 16)) - 1)
        // {
        //     if (this->list_y_trg / FONT_HEIGHT == -(uiSelect - 1))
        //         this->list_y_trg -= FONT_HEIGHT;
        //     else
        //         this->list_y_trg -= FONT_HEIGHT * (SCREEN_HEIGHT / 16);
        // }

        // 滚动条位置
        this->slidbar_y = SCREEN_HEIGHT;
        this->slidbar_y_trg = uiSelect * (SCREEN_HEIGHT / (this->itemMax + 1));

        // 列表滚动
        if (uiSelect >= SCREEN_HEIGHT / 16)
            this->list_y_trg = -(uiSelect - 3) * FONT_HEIGHT;
        else
            this->list_y_trg = 0;

        // 选择框滚动
        this->frame_y = this->frame_y_trg + FONT_HEIGHT * 1.5f;
        if (uiSelect < SCREEN_HEIGHT / 16)
            this->frame_y_trg = uiSelect * FONT_HEIGHT;
        else
            this->frame_y_trg = 3 * FONT_HEIGHT;

        if (uiSelect == 0)
            this->list_y_trg = 0;

        uiState = State::JumpPage;
        break;
        
    case State::JumpPage:
    {
        static uint8_t isJumpPageFinish = 0;
         
        if (Animation_EasyOut(&this->page_x, &this->page_x_trg, 85) == 0)
        {
            isJumpPageFinish |= 0x0f;
        }

        if((isJumpPageFinish & 0xf0) != 0xf0)
        {
            oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            if(Animation_Blur() == 0)
            {
                isJumpPageFinish |= 0xf0;
            }
        }

        if (isJumpPageFinish == 0xff)
        {
            isJumpPageFinish = 0;
            uiState = State::JumpPageArrive;
        }
        break;
    }
    case State::JumpPageArrive:
        this->frame_y_trg = (uiSelect % (SCREEN_HEIGHT / 16)) * FONT_HEIGHT;
        this->frame_width_trg = oled_get_UTF8_width(thisitem->title.c_str()) + FONT_WIDTH;
        this->slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / this->itemMax);

        uiState = State::None;
        break;

    default:
        uiState = State::None;
        break;
    }
}
