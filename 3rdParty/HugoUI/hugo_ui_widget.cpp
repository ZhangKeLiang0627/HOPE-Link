#include "hugo_ui_widget.h"

void HugoUI::WidgetDrawMessageBox(const char *msg, bool isRefreshImme)
{   
    if(isRefreshImme)
        oled_clear_buffer();

    // Blur
    int len = 8 * oled_get_buffer_tile_height() * oled_get_buffer_tile_width();
    uint8_t *p = oled_get_buffer_ptr();

    currentPage->Show(currentItem.get());

    // 给原本的渲染内容打上一层模糊的棋盘格效果
    for (uint16_t i = 0; i < len; i++)
    {
        if (i % 2 == 0)
        {
            p[i] = p[i] & (0x55);
            p[i] = p[i] & (0x00);
        }
        else
            p[i] = p[i] & (0xaa);
    }

    // Show Msg
    uint16_t msg_width = oled_get_UTF8_width(msg) + FONT_WIDTH * 2;

    // 画后面的立体阴影
    oled_draw_R_frame((128 - msg_width) / 2 + 3, 26 - 2, msg_width, FONT_HEIGHT, 1);

    // 反色擦除需要绘制的区域
    oled_set_draw_color(0);
    oled_draw_R_box((128 - msg_width) / 2 - 1, 26 - 1, msg_width + 2, FONT_HEIGHT + 2, 0);
    oled_set_draw_color(1);

    // 绘制文本内容
    oled_draw_UTF8((128 - msg_width) / 2 + FONT_WIDTH, 26 + FONT_HEIGHT - 2, msg);

    // 绘制反色框
    oled_set_draw_color(2);
    oled_draw_R_box((128 - msg_width) / 2, 26, msg_width, FONT_HEIGHT, 0);
    oled_set_draw_color(1);

    if (isRefreshImme)
        oled_send_buffer();
}