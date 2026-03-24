#include "hugo_ui_page_icon.h"

using namespace HugoUI;

/**
 * @brief 通用图标页面显示
 */
void PageIcon::Show(Item* thisitem)
{
    if (!thisitem) return;

    // 计算动画
    Animation_Linear(&this->page_x, &this->page_x_trg, 65);
    Animation_Linear(&this->page_y, &this->page_y_trg, 65);
    Animation_Linear(&this->frame_x, &this->frame_x_trg, 65);
    Animation_Linear(&this->icon_move_x, &this->icon_move_x_trg, 75);
    Animation_Linear(&this->icon_rectangle_x, &this->icon_rectangle_x_trg, 65);
    
    // 绘制参数
    int16_t Item_y = static_cast<int16_t>(this->page_y);
    int16_t Item_x;

    oled_set_bitmap_mode(1);
    
    // 遍历Item
    for (const auto& item : this->items)
    {
        if (!item) continue;
        
        // 不在可视区域则跳过
        if (item->lineId <= uiSelect - 5 || item->lineId >= uiSelect + 6)
            continue;

        Item_x = 48 + static_cast<int16_t>(this->page_x + this->icon_move_x * item->lineId);

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
    oled_draw_R_box(48 + static_cast<int16_t>(this->frame_x), 
                    static_cast<int16_t>(this->page_y), 
                    32, 32, 0);
    
    // 绘制矩形
    oled_draw_box(0, 
                  static_cast<int16_t>(this->page_y + SCREEN_HEIGHT / 2 + 4), 
                  static_cast<int16_t>(this->icon_rectangle_x), 24);
    oled_set_draw_color(1);

    // 状态处理
    switch (uiState)
    {
    case State::None: 
        break;
        
    case State::RunPageDown:
        this->page_x_trg = -(uiSelect * 48);
        this->frame_x -= 24;

        this->icon_rectangle_x = 0;
        if (uiSelect == 0)
        {
            this->icon_move_x = 0;
        }

        uiState = State::None;
        break;
        
    case State::RunPageUp:
        this->page_x_trg = -(uiSelect * 48);
        this->frame_x += 24;

        this->icon_rectangle_x = 0;

        uiState = State::None;
        break;
        
    case State::ReadyToJumpPage:
        // 页面位置
        this->page_y_trg = 0;
        this->page_y = 40;

        this->icon_move_x = 160;
        this->frame_x = 160;

        this->page_x_trg = -(uiSelect * 48);
        this->icon_rectangle_x = 0;

        uiState = State::JumpPage;
        break;
        
    case State::JumpPage:
    {
        static uint8_t isJumpPageFinish = 0;
         
        if (Animation_EasyOut(&this->page_x, &this->page_x_trg, 75) == 0)
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
        this->page_x_trg = -(uiSelect * 48);
        uiState = State::None;
        break;

    default:
        uiState = State::None;
        break;
    }
}
