#ifndef __HUGO_UI_PAGE_ICON_H
#define __HUGO_UI_PAGE_ICON_H

#include "hugo_ui.h"

/* define ------------------------------------------------------- */

namespace HugoUI
{

/* class ------------------------------------------------------- */
class PageIcon : public Page {
public:
    using Ptr = std::shared_ptr<PageIcon>;
    
    // Icon页面专属参数
    float frame_x, frame_x_trg;
    float icon_move_x, icon_move_x_trg;
    float icon_desc_y, icon_desc_y_trg;
    float icon_rectangle_x, icon_rectangle_x_trg;

    // 构造函数
    PageIcon(const std::string& pageTitle = "") 
        : Page(PageType::Icon, pageTitle),
          frame_x(SCREEN_WIDTH), frame_x_trg(0),
          icon_move_x(0), icon_move_x_trg(48),
          icon_desc_y(0), icon_desc_y_trg(24),
          icon_rectangle_x(0), icon_rectangle_x_trg(13) {}

    // 实现基类的纯虚函数
    void Show(Item* thisitem) override;
};

/* API-Function --------------------------------------------------- */

} /* namespace HugoUI */

/* 声明变量 ------------------------------------------------------- */


#endif // __HUGO_UI_PAGE_ICON_H