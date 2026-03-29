#ifndef __HUGO_UI_WIDGET_H
#define __HUGO_UI_WIDGET_H

#include "hugo_ui.h"
#include <string>

/* define ------------------------------------------------------- */
#define INFO_BAR_HEIGHT 15
#define INFO_BAR_OFFSET 10

#define POP_UP_HEIGHT 20
#define POP_UP_OFFSET 8

namespace HugoUI
{
    /* class ------------------------------------------------------- */

    /* struct ------------------------------------------------------- */
    typedef struct widget_info_bar_t
    {
        char *content;
        uint16_t span;
        float y_info_bar, y_info_bar_trg, w_info_bar, w_info_bar_trg;
        bool is_running;
        uint32_t time_start;
        uint32_t time;
    } widget_info_bar_t;

    typedef struct widget_pop_up_t
    {
        char *content;
        uint16_t span;
        float y_pop_up, y_pop_up_trg, w_pop_up, w_pop_up_trg;
        bool is_running;
        uint32_t time_start;
        uint32_t time;
    } widget_pop_up_t;

    /* API-Function --------------------------------------------------- */
    void WidgetShow(void);

    void WidgetPushInfoBar(const char *_content, const uint16_t _span);
    void WidgetDrawInfoBar(void);

    void WidgetPushPopUp(const char *_content, const uint16_t _span);
    void WidgetDrawPopUp(void);

    void WidgetDrawMessageBox(const char *msg, bool isRefreshImme = false);

} /* namespace HugoUI */

/* 声明变量 ------------------------------------------------------- */

#endif // __HUGO_UI_WIDGET_H
