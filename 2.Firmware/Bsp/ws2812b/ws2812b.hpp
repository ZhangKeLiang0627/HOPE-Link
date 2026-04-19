#ifndef __WS2812B_HPP
#define __WS2812B_HPP

#include "tim.h"

#define WS2812B_ARR 105 // TIM的自动重装值 / 使得PWM输出频率在800kHz
#define WS2812B_T0H 35  // 0编码高电平时间占1/3
#define WS2812B_T1H 70  // 1编码高电平时间占2/3

#define WS2812B_NUM 1   // 使用灯珠的个数
#define WS2812B_DATA_SIZE 24 // WS2812B传输一个数据的大小是3个字节（24bit）

class WS2812B
{
private:
    TIM_HandleTypeDef *htim;
    uint32_t buffer[WS2812B_DATA_SIZE * WS2812B_NUM + 50];

public:
    explicit WS2812B(TIM_HandleTypeDef *_htim);

    void SetPixels(uint8_t _num, uint32_t _color);
    void SetPixels(uint8_t _num, uint32_t *_color);

    void UpdatePixels(void);

};

#endif // __WS2812B_HPP
