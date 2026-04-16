#ifndef __HUGO_UI_ANIMATION_H
#define __HUGO_UI_ANIMATION_H

#include "hugo_ui.h"

/* define ------------------------------------------------------- */

namespace HugoUI
{
    /* class ------------------------------------------------------- */

    /* API-Function ------------------------------------------------ */
    uint8_t Animation_Linear(float *a, float *a_trg, uint8_t n);
    uint8_t Animation_EasyIn(float *a, float *a_trg, uint16_t n);
    uint8_t Animation_EasyOut(float *a, float *a_trg, uint16_t n);
    uint8_t Animation_Elastic(float *a, float *a_trg, uint8_t n);

    uint8_t Transition_Blur(void);
    uint8_t Transition_Hourglass(void);

} /* namespace HugoUI */

/* 声明变量 ------------------------------------------------------- */

#endif // __HUGO_UI_ANIMATION_H
