#include "ws2812b.hpp"

WS2812B::WS2812B(TIM_HandleTypeDef *_htim) : htim(_htim)
{
    // 初始化缓冲区为 0
    uint32_t bufSize = WS2812B_DATA_SIZE * WS2812B_NUM + 50;
    for (uint32_t i = 0; i < bufSize; i++)
    {
        buffer[i] = 0;
    }
}

void WS2812B::SetPixels(uint8_t _num, uint32_t _color)
{
    uint8_t i, j;
    for (j = 0; j < _num; j++)
    {
        for (i = 0; i < WS2812B_DATA_SIZE; i++)
        {
            // 因为数据发送的顺序是GRB，高位先发，所以从高位开始判断，判断后比较值先放入缓存数组 
            buffer[i + j * WS2812B_DATA_SIZE] = ((_color << i) & 0x800000) ? WS2812B_T1H : WS2812B_T0H;
        }
    }
}

void WS2812B::SetPixels(uint8_t _num, uint32_t *_color)
{
    uint8_t i, j;
    for (j = 0; j < _num; j++)
    {
        for (i = 0; i < WS2812B_DATA_SIZE; i++)
        {
            // 因为数据发送的顺序是GRB，高位先发，所以从高位开始判断，判断后比较值先放入缓存数组 
            buffer[i + j * WS2812B_DATA_SIZE] = ((_color[j] << i) & 0x800000) ? WS2812B_T1H : WS2812B_T0H;
        }
    }
}

void WS2812B::UpdatePixels(void)
{
    // 发送数据到 WS2812B
    HAL_TIM_PWM_Start_DMA(htim, TIM_CHANNEL_3, (uint32_t *)buffer, WS2812B_NUM * WS2812B_DATA_SIZE + 50);
}