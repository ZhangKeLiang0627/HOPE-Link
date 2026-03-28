#ifndef __HUGO_UI_EVENT_DAP_H
#define __HUGO_UI_EVENT_DAP_H

#include "hugo_ui.h"

/* define ------------------------------------------------------- */

namespace HugoUI
{
    /* class ------------------------------------------------------- */

    /* API-Function --------------------------------------------------- */
    void AddItemsFromFirmwareFolder(Page::Ptr page, const char *folderPath);
    void EventTestDapUI(void);

} /* namespace HugoUI */

/* 声明变量 ------------------------------------------------------- */

#endif // __HUGO_UI_EVENT_DAP_H
