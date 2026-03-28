#include "hugo_ui_page_list.h"
#include "hugo_ui_bmp.h"

#include <cmath>

using namespace HugoUI;

static int16_t GetTitleDrawX(PageList *page, const Item::Ptr &item, int16_t base_x, int textMaxPixel)
{
    if (!item)
        return base_x;

    if (item->lineId != uiSelect)
    {
        return base_x;
    }

    if (page->selected_line_last != uiSelect)
    {
        page->selected_line_last = uiSelect;
        page->scroll_x = 0;
        page->scroll_dir = 1;
        page->scroll_delay = 0;
    }

    int full_w = oled_get_UTF8_width(item->title.c_str());
    bool too_long = full_w > textMaxPixel;

    if (too_long)
    {
        if (++page->scroll_delay >= 3)
        {
            page->scroll_delay = 0;
            page->scroll_x += page->scroll_dir;
        }

        int max_scroll = full_w - textMaxPixel + FONT_WIDTH * 2;
        if (page->scroll_x > max_scroll)
        {
            page->scroll_x = max_scroll;
            page->scroll_dir = -1;
        }
        else if (page->scroll_x < 0)
        {
            page->scroll_x = 0;
            page->scroll_dir = 1;
        }

        return static_cast<int16_t>(base_x - page->scroll_x);
    }

    page->scroll_x = 0;
    page->scroll_dir = 1;
    page->scroll_delay = 0;

    return base_x;
}

static void DrawTitle(PageList *page, const Item::Ptr &item, int16_t base_x, int16_t y, int textMaxPixel)
{
    int16_t draw_x = GetTitleDrawX(page, item, base_x, textMaxPixel);
    oled_draw_UTF8(draw_x, y, item->title.c_str());
}

static void ClearControlArea(int16_t start_x, int16_t item_x, int16_t item_y)
{
    int16_t clear_x = start_x;
    int16_t clear_y = item_y - FONT_HEIGHT + 2;
    int16_t max_clear_x = item_x + SCREEN_WIDTH - SCROLL_BAR_WIDTH;
    if (clear_x >= max_clear_x)
        return;

    int16_t clear_w = max_clear_x - clear_x;
    if (clear_w > 0)
    {
        oled_set_draw_color(0);
        oled_draw_box(clear_x, clear_y, clear_w, FONT_HEIGHT);
        oled_set_draw_color(1);
    }
}

/**
 * @brief 通用列表页面显示
 */
void PageList::Show(Item *thisitem)
{
    if (!thisitem)
        return;

    // 计算动画
    Animation_Linear(&this->list_y, &this->list_y_trg, 85);
    Animation_Linear(&this->frame_y, &this->frame_y_trg, 65);
    Animation_Linear(&this->frame_width, &this->frame_width_trg, 40);
    Animation_Linear(&this->slidbar_y, &this->slidbar_y_trg, 65);

    // 绘制目录树和目录名
    int16_t item_x = static_cast<int16_t>(this->page_x);
    int16_t item_y;

    // 长文本滚动参数
    uint8_t bar_h = ceil((float)SCREEN_HEIGHT / this->itemMax);

    // 遍历页面Item
    for (const auto &item : this->items)
    {
        if (!item)
            continue;

        // 不在可视区域则跳过 / 仅渲染可视区域
        if (!(item->lineId <= uiSelect - 5 || item->lineId >= uiSelect + 6))
        {
            item_y = FONT_HEIGHT - 1 + static_cast<int16_t>(this->page_y + item->lineId * FONT_HEIGHT + this->list_y);

            // 根据控件类型确定文本最大像素
            int textMaxPixel = (item->funcType == ItemType::Switch ||
                                item->funcType == ItemType::ChangeValue ||
                                item->funcType == ItemType::Checkbox)
                                   ? 96
                                   : 120;

            switch (item->funcType)
            {
            case ItemType::Description:
                oled_draw_str(2 + item_x, item_y, "#");
                DrawTitle(this, item, 2 + 9 + item_x, item_y, textMaxPixel);
                break;

            case ItemType::JumpPage:
                oled_draw_str(2 + item_x, item_y, "+");
                DrawTitle(this, item, 2 + 10 + item_x, item_y, textMaxPixel);
                break;

            case ItemType::Checkbox:
                oled_draw_str(2 + item_x, item_y, "-");
                DrawTitle(this, item, 2 + 9 + item_x, item_y, textMaxPixel);

                // 清除长文本和控件穿模区域
                ClearControlArea(static_cast<int16_t>(SCREEN_WIDTH - FONT_WIDTH * 4 + item_x), item_x, item_y);

                // 绘制勾选标记
                if (item->flag && *item->flag)
                {
                    oled_set_bitmap_mode(1);
                    oled_draw_bMP(SCREEN_WIDTH - 20 + item_x, item_y + 3 - FONT_HEIGHT, 10, 10, CheckBoxSelection_BMP);
                    oled_set_bitmap_mode(0);
                }

                // 绘制勾选框
                oled_draw_frame(SCREEN_WIDTH - 20 + item_x, item_y - 12 + 2, 11, 11);

                // 当前选项高亮
                if (item->lineId == uiSelect)
                {
                    oled_set_draw_color(2);
                    oled_draw_box(SCREEN_WIDTH - 21 + item_x, item_y - 12 + 1, 13, 13);
                    oled_set_draw_color(1);
                }
                break;

            case ItemType::Switch:
                oled_draw_str(2 + item_x, item_y, "-");
                DrawTitle(this, item, 2 + 9 + item_x, item_y, textMaxPixel);

                // 清除长文本和控件穿模区域
                ClearControlArea(static_cast<int16_t>(SCREEN_WIDTH - FONT_WIDTH * 4 + item_x), item_x, item_y);

                // 显示On/Off
                oled_draw_str(SCREEN_WIDTH - FONT_WIDTH * 3 + item_x, item_y,
                              (item->flag && *item->flag) ? "On" : "Off");
                break;

            case ItemType::ChangeValue:
                oled_draw_str(2 + item_x, item_y, "-");
                DrawTitle(this, item, 2 + 9 + item_x, item_y, textMaxPixel);

                // 清除长文本和控件穿模区域
                ClearControlArea(static_cast<int16_t>(SCREEN_WIDTH - FONT_WIDTH * 4 + item_x), item_x, item_y);

                // 显示数值
                if (item->param)
                {
                    char float_str[20] = {0};
#ifdef FPU
                    sprintf(float_str, "%.1f", *item->param);
#else
                    sprintf(float_str, "%d", *item->param);
#endif

                    int16_t val_x = (*item->param < 100) ? (SCREEN_WIDTH - FONT_WIDTH * 4 + item_x) : (SCREEN_WIDTH - FONT_WIDTH * 5 + item_x);
                    oled_draw_str(val_x, item_y, float_str);

                    // 当前数值高亮
                    if (item->lineId == uiSelect && ChangeVal_flag)
                    {
                        oled_set_draw_color(2);
                        int16_t box_x = (*item->param < 0) ? (SCREEN_WIDTH - FONT_WIDTH * 7 + item_x) : (SCREEN_WIDTH - FONT_WIDTH * 5 + item_x);
                        int16_t box_w = (*item->param < 0) ? (FONT_WIDTH * 6) : (FONT_WIDTH * 4);
                        oled_draw_box(box_x, item_y - 12 + 2, box_w, 11);
                        oled_set_draw_color(1);
                    }
                }
                break;

            default:
                oled_draw_str(2 + item_x, item_y, "-");
                DrawTitle(this, item, 2 + 9 + item_x, item_y, textMaxPixel);
                break;
            }
        }
    }

    // 清除长文本穿模滚动条的区域
    oled_set_draw_color(0);
    oled_draw_box(SCREEN_WIDTH - SCROLL_BAR_WIDTH, 0, SCROLL_BAR_WIDTH, SCREEN_WIDTH);
    oled_set_draw_color(1);
    // 绘制滚动条分隔线
    for (const auto &item : this->items)
    {
        if (!item)
            continue;

        uint8_t bar_y = static_cast<int16_t>(this->page_y + item->lineId * bar_h);

        if (item->lineId % 2 == 0)
            oled_draw_line(item_x + SCREEN_WIDTH - 3, bar_y, item_x + SCREEN_WIDTH - 1, bar_y);
        else
            oled_draw_line(item_x + SCREEN_WIDTH - 3, bar_y, item_x + SCREEN_WIDTH - 2, bar_y);
    }

    // 绘制选择框（反色）
    oled_set_draw_color(2);
    oled_draw_R_box(static_cast<int16_t>(this->page_x),
                    static_cast<int16_t>(this->page_y + this->frame_y),
                    static_cast<int16_t>(this->page_x + this->frame_width + 5),
                    FONT_HEIGHT + 2, 0);
    oled_set_draw_color(1);

    // 绘制滚动条直线
    oled_draw_line(static_cast<int16_t>(this->page_x + SCREEN_WIDTH - 2),
                   static_cast<int16_t>(this->page_y),
                   static_cast<int16_t>(this->page_x + SCREEN_WIDTH - 2),
                   static_cast<int16_t>(this->page_y + SCREEN_HEIGHT));

    // 绘制滚动条滑块
    oled_draw_box(static_cast<int16_t>(this->page_x + SCREEN_WIDTH - 3),
                  static_cast<int16_t>(this->page_y + this->slidbar_y),
                  SCROLL_BAR_WIDTH,
                  bar_h);

    // 页面状态处理
    switch (uiState)
    {
    case State::None:
        break;

    case State::RunPageDown:
    {
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

        // 选择框宽度
        uint16_t title_max_w = (thisitem->funcType == ItemType::Switch ||
                                   thisitem->funcType == ItemType::ChangeValue ||
                                   thisitem->funcType == ItemType::Checkbox)
                                      ? 96
                                      : 120;
        uint16_t title_w = oled_get_UTF8_width(thisitem->title.c_str());
        this->frame_width_trg = (title_w > title_max_w ? title_max_w : title_w + FONT_WIDTH);

        // 滚动条滑块
        this->slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / this->itemMax);

        uiState = State::None;
        break;
    }
    case State::RunPageUp:
    {
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

        // 选择框宽度
        uint16_t title_max_w = (thisitem->funcType == ItemType::Switch ||
                                   thisitem->funcType == ItemType::ChangeValue ||
                                   thisitem->funcType == ItemType::Checkbox)
                                      ? 96
                                      : 120;
        uint16_t title_w = oled_get_UTF8_width(thisitem->title.c_str());
        this->frame_width_trg = (title_w > title_max_w ? title_max_w : title_w + FONT_WIDTH);

        // 滚动条滑块
        this->slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / this->itemMax);

        uiState = State::None;
        break;
    }
    case State::ReadyToJumpPage:
    {
        // 页面位置
        this->page_x_trg = 0;
        this->page_x = 100;

        // 选择框位置
        this->frame_y = 128;
        this->frame_y_trg = (uiSelect % (SCREEN_HEIGHT / 16)) * FONT_HEIGHT;

        // 滚动条位置
        this->slidbar_y = SCREEN_HEIGHT;
        this->slidbar_y_trg = uiSelect * ceil((float)SCREEN_HEIGHT / this->itemMax);

        // 列表滚动
        if (uiSelect >= SCREEN_HEIGHT / 16)
            this->list_y_trg = -(uiSelect - 3) * FONT_HEIGHT;
        else
            this->list_y_trg = 0;

        // 选择框滚动
        this->frame_y = 128;
        if (uiSelect < SCREEN_HEIGHT / 16)
            this->frame_y_trg = uiSelect * FONT_HEIGHT;
        else
            this->frame_y_trg = 3 * FONT_HEIGHT;

        // 选择框宽度
        uint16_t title_max_w = (thisitem->funcType == ItemType::Switch ||
                                   thisitem->funcType == ItemType::ChangeValue ||
                                   thisitem->funcType == ItemType::Checkbox)
                                      ? 96
                                      : 120;
        uint16_t title_w = oled_get_UTF8_width(thisitem->title.c_str());
        this->frame_width_trg = (title_w > title_max_w ? title_max_w : title_w + FONT_WIDTH);

        uiState = State::JumpPage;
        break;
    }

    case State::JumpPage:
    {
        static uint8_t isJumpPageFinish = 0;

        if (Animation_EasyOut(&this->page_x, &this->page_x_trg, 85) == 0)
        {
            isJumpPageFinish |= 0x0f;
        }

        if ((isJumpPageFinish & 0xf0) != 0xf0)
        {
            oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            if (Animation_Blur() == 0)
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
        uiState = State::None;
        break;

    default:
        uiState = State::None;
        break;
    }
}
