#ifndef __HUGO_UI_EVENT_DAP_H
#define __HUGO_UI_EVENT_DAP_H

#include "hugo_ui.h"

/* define ------------------------------------------------------- */

namespace HugoUI
{
    /* class ------------------------------------------------------- */


    /* API-Function --------------------------------------------------- */
    void AddItemsFromFirmwareFolder(Page::Ptr page, const char *folderPath);
    void AddItemsFromFlashAlgo(Page::Ptr page);
    void EventBurnDapUI(void);
    void EventEraseChipUI(void);
    void EventAutoTriggerUI(void);
    void EventSetFlashAddressUI(void);
    void EventSelectFirmware(void);
    void EventSelectFlashAlgo(void);

} /* namespace HugoUI */

/* 声明变量 ------------------------------------------------------- */

#endif // __HUGO_UI_EVENT_DAP_H
