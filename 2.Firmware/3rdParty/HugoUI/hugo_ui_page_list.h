#ifndef __HUGO_UI_PAGE_LIST_H
#define __HUGO_UI_PAGE_LIST_H

#include "hugo_ui.h"

/* define ------------------------------------------------------- */

namespace HugoUI
{

/* class ------------------------------------------------------- */
class PageList : public Page {
public:
    using Ptr = std::shared_ptr<PageList>;
    
    // 列表页面专属参数
    float list_y, list_y_trg;
    float frame_y, frame_y_trg;
    float frame_width, frame_width_trg;
    float slidbar_y, slidbar_y_trg;

    // 长文本滚动状态，仅选中项滚动
    int scroll_x;
    int scroll_dir;
    uint16_t scroll_delay;
    int selected_line_last;

    // 缓存的计算值，避免重复计算
    int cached_selection_text_max_pixel;
    int cached_selection_title_w;
    int cached_sel_frame_width;
    uint8_t cached_bar_h;

    // 构造函数
    PageList(const std::string& pageTitle = "") 
        : Page(PageType::List, pageTitle),
          list_y(0), list_y_trg(0),
          frame_y(0), frame_y_trg(0),
          frame_width(36), frame_width_trg(36),
          slidbar_y(0), slidbar_y_trg(0),
          scroll_x(0), scroll_dir(1), scroll_delay(0), selected_line_last(-1),
          cached_selection_text_max_pixel(-1), cached_selection_title_w(-1), 
          cached_sel_frame_width(-1), cached_bar_h(0) {}

    // 实现基类的纯虚函数
    void Show(Item* thisitem) override;
};

/* API-Function --------------------------------------------------- */

} /* namespace HugoUI */

/* 声明变量 ------------------------------------------------------- */


#endif // __HUGO_UI_PAGE_LIST_H