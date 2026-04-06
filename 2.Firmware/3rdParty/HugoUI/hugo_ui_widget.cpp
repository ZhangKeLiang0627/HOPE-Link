#include "hugo_ui_widget.h"

using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */
widget_info_bar_t widgetInfoBar = {0, 1, 0 - 2 * INFO_BAR_HEIGHT, 0 - 2 * INFO_BAR_HEIGHT, 80, 80, false, 0, 1};
widget_pop_up_t widgetPopUp = {0, 1, 0 - 2 * POP_UP_HEIGHT, 0 - 2 * POP_UP_HEIGHT, 80, 80, false, 0, 1};

/* 用户函数 ----------------------------------------------------------- */

// widget当中所有控件的渲染函数
void HugoUI::WidgetShow(void)
{
    WidgetDrawInfoBar();
    WidgetDrawPopUp();
}

// widgetInfoBar的用户函数调用
void HugoUI::WidgetPushInfoBar(const char *_content, const uint16_t _span)
{
    // 设定显示时间的概念，超过了显示时间，就将ytrg设为初始位置，如果在显示时间之内，有新的消息涌入，则y和ytrg都不变，继续显示，且显示时间清零
    // 只有显示时间到了的时候，才会复位
    widgetInfoBar.time = get_ticks();

    widgetInfoBar.content = (char *)_content;

    widgetInfoBar.span = _span;
    widgetInfoBar.is_running = false; // 每次进入该函数都代表有新的消息涌入，所以需要重置is_running

    // 展开弹窗 收回弹窗和同步时间戳需要在循环中进行 所以移到了drawer中
    if (!widgetInfoBar.is_running)
    {
        widgetInfoBar.time_start = get_ticks();
        widgetInfoBar.y_info_bar_trg = 0;
        widgetInfoBar.is_running = true;
    }

    // oled_set_font(u8g2_font_wqy13_t_gb2312a);
    widgetInfoBar.w_info_bar_trg = oled_get_UTF8_width(widgetInfoBar.content) + INFO_BAR_OFFSET;
}

// widgetInfoBar的画面函数渲染
void HugoUI::WidgetDrawInfoBar(void)
{
    if (!widgetInfoBar.is_running)
        return;

    // 计算动画插值
    Animation_Linear(&widgetInfoBar.y_info_bar, &widgetInfoBar.y_info_bar_trg, 94);
    Animation_Linear(&widgetInfoBar.w_info_bar, &widgetInfoBar.w_info_bar_trg, 95);

    // 弹窗到位后才开始计算时间
    if (widgetInfoBar.y_info_bar == widgetInfoBar.y_info_bar_trg)
        widgetInfoBar.time = get_ticks();

    // 时间到了就收回
    if (widgetInfoBar.time - widgetInfoBar.time_start >= widgetInfoBar.span)
    {
        widgetInfoBar.y_info_bar_trg = 0 - 2 * INFO_BAR_HEIGHT; // 收回
        if (widgetInfoBar.y_info_bar == widgetInfoBar.y_info_bar_trg)
            widgetInfoBar.is_running = false; // 等归位后结束生命周期
    }

    int16_t _x_info_bar = SCREEN_WIDTH / 2 - widgetInfoBar.w_info_bar / 2;
    int16_t _y_info_bar_1 = widgetInfoBar.y_info_bar - 4;
    int16_t _y_info_bar_2 = widgetInfoBar.y_info_bar + INFO_BAR_HEIGHT;

    // oled_set_font(u8g2_font_wqy13_t_gb2312a);
    oled_set_draw_color(1);
    oled_draw_R_box(_x_info_bar + 3, _y_info_bar_1 + 3,
                    (int16_t)widgetInfoBar.w_info_bar, INFO_BAR_HEIGHT + 4, 4);

    oled_set_draw_color(0); // 黑遮罩打底
    oled_draw_R_box((int16_t)(SCREEN_WIDTH / 2 - (widgetInfoBar.w_info_bar + 4) / 2), _y_info_bar_1,
                    (int16_t)(widgetInfoBar.w_info_bar + 4), INFO_BAR_HEIGHT + 6, 4);

    oled_set_draw_color(1);
    oled_draw_R_box(_x_info_bar, _y_info_bar_1,
                    (int16_t)widgetInfoBar.w_info_bar, INFO_BAR_HEIGHT + 4, 3);
    // 向上移动四个像素 同时向下多画四个像素 只用下半部分圆角

    oled_set_draw_color(2);
    oled_draw_H_line(_x_info_bar + 2, _y_info_bar_2 - 2, (int16_t)(widgetInfoBar.w_info_bar - 4));
    oled_draw_pixel(_x_info_bar + 1, _y_info_bar_2 - 3);
    oled_draw_pixel(_x_info_bar - 2, _y_info_bar_2 - 3);

    oled_set_draw_color(0);
    oled_draw_UTF8(_x_info_bar + 6,
                   (int16_t)(widgetInfoBar.y_info_bar + oled_get_str_height() - 2),
                   widgetInfoBar.content);
    oled_set_draw_color(1);

}

// widgetPopUp的用户函数调用
void HugoUI::WidgetPushPopUp(const char *_content, const uint16_t _span)
{
    widgetPopUp.time = get_ticks();
    widgetPopUp.content = (char *)_content;
    widgetPopUp.span = _span;
    widgetPopUp.is_running = false;

    // 弹出
    if (!widgetPopUp.is_running)
    {
        widgetPopUp.time_start = get_ticks();
        widgetPopUp.y_pop_up_trg = 20;
        widgetPopUp.is_running = true;
    }

    // oled_set_font(u8g2_font_wqy13_t_gb2312a);
    widgetPopUp.w_pop_up_trg = oled_get_UTF8_width(widgetPopUp.content) + POP_UP_OFFSET;
}

// widgetPopUp的画面函数渲染
void HugoUI::WidgetDrawPopUp(void)
{
    if (!widgetPopUp.is_running)
        return;

    Animation_Linear(&widgetPopUp.y_pop_up, &widgetPopUp.y_pop_up_trg, 94);
    Animation_Linear(&widgetPopUp.w_pop_up, &widgetPopUp.w_pop_up_trg, 96);

    // 弹窗到位后才开始计算时间
    if (widgetPopUp.y_pop_up == widgetPopUp.y_pop_up_trg)
        widgetPopUp.time = get_ticks();

    // 时间到了就收回
    if (widgetPopUp.time - widgetPopUp.time_start >= widgetPopUp.span)
    {
        widgetPopUp.y_pop_up_trg = 0 - 2 * INFO_BAR_HEIGHT; // 收回
        if (widgetPopUp.y_pop_up == widgetPopUp.y_pop_up_trg)
            widgetPopUp.is_running = false; // 等归位后结束生命周期
    }

    int16_t _x_pop_up = SCREEN_WIDTH / 2 - widgetPopUp.w_pop_up / 2;
    int16_t _y_pop_up = widgetPopUp.y_pop_up + POP_UP_HEIGHT;

    // oled_set_font(u8g2_font_wqy13_t_gb2312a);
    oled_set_draw_color(1); // 阴影打底
    oled_draw_R_box(_x_pop_up + 1, (int16_t)widgetPopUp.y_pop_up + 3,
                    (int16_t)(widgetPopUp.w_pop_up + 4),
                    POP_UP_HEIGHT, 4);

    oled_set_draw_color(0); // 黑遮罩
    oled_draw_R_box((int16_t)(SCREEN_WIDTH / 2 - (widgetPopUp.w_pop_up + 4) / 2 - 2), (int16_t)(widgetPopUp.y_pop_up - 2),
                    (int16_t)(widgetPopUp.w_pop_up + 8), POP_UP_HEIGHT + 4, 5);

    oled_set_draw_color(1);
    oled_draw_R_box(_x_pop_up - 2, (int16_t)widgetPopUp.y_pop_up,
                    (int16_t)(widgetPopUp.w_pop_up + 4),
                    POP_UP_HEIGHT, 3);

    oled_set_draw_color(2);
    oled_draw_H_line(_x_pop_up, _y_pop_up - 2, (int16_t)widgetPopUp.w_pop_up);
    oled_draw_pixel(_x_pop_up - 1, _y_pop_up - 3);
    oled_draw_pixel((int16_t)(SCREEN_WIDTH / 2 + widgetPopUp.w_pop_up / 2), _y_pop_up - 3);

    oled_set_draw_color(0);
    oled_draw_UTF8(_x_pop_up + 3,
                   (int16_t)(widgetPopUp.y_pop_up + oled_get_str_height() + 1),
                   widgetPopUp.content);
    oled_set_draw_color(1);
}

// widgetDrawMessageBox绘制msg的用户函数调用
void HugoUI::WidgetDrawMessageBox(const char *msg, bool isRefreshImme)
{
    if (isRefreshImme)
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

void HugoUI::WidgetDrawProgressBar(const char *msg, float num, bool isRefreshImme)
{
    if (isRefreshImme)
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

    // Loop
    // 反色擦除需要绘制的区域
    oled_set_draw_color(0);
    oled_draw_box(6, 18, 118, 30);
    oled_set_draw_color(1);

    // 绘制反色框
    oled_draw_frame(6, 18, 118, 30);

    // title
    oled_draw_UTF8(10, 20 + FONT_HEIGHT, msg);

    // desc

    // 进度条
    oled_draw_frame(10, 24 + FONT_HEIGHT, 70, 6);
    if (num < 0.0f)
        num = 0.0f;
    else if (num > 100.0f)
        num = 100.0f;
    int progressWidth = (int)(num * 67.0f / 100.0f + 0.5f);
    if (progressWidth > 67)
        progressWidth = 67;
    oled_draw_box(12, 26 + FONT_HEIGHT, progressWidth, 2); // width: 0 ~ 67

    // float num
    char numForShow[16] = {0};
    sprintf(numForShow, "%.2f", num);
    oled_draw_str(85, 30 + FONT_HEIGHT, numForShow);

    if (isRefreshImme)
        oled_send_buffer();
}