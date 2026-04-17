#ifndef __HUGO_UI_CONFIG_H
#define __HUGO_UI_CONFIG_H

#include "hugo_ui.h"

/* define ---------------------------------------------------------- */

namespace HugoUI
{
    /* class ------------------------------------------------------- */

    /* API-Function ------------------------------------------------ */
    bool LoadConfig(const std::string &file_path = "0:/Config/config.json");
    bool SaveConfig(const std::string &file_path = "0:/Config/config.json");

} /* namespace HugoUI */

/* 声明变量 -------------------------------------------------------- */

#endif // __HUGO_UI_CONFIG_H