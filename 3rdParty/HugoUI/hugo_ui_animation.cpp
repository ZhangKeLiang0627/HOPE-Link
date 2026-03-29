#include "hugo_ui_animation.h"

#include <cmath>

using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */


/* 动画函数 ----------------------------------------------------------- */

/**
 * @brief 线性动画插值
 * @param a: 当前值
 * @param a_trg: 目标值
 * @param n: 缓动系数
 * @retval 1-动画进行中，0-动画完成
 */
uint8_t HugoUI::Animation_Linear(float *a, float *a_trg, uint8_t n)
{
    if (fabs(*a - *a_trg) < 0.001f)
    {
        *a = *a_trg;
        return 0;
    }

    if (fabs(*a - *a_trg) < 0.15f)
        *a = *a_trg;
    else
        *a += (*a_trg - *a) / (n / 10.0f);

    return 1;
}

/**
 * @brief 缓出动画插值
 */
uint8_t HugoUI::Animation_EasyOut(float *a, float *a_trg, uint16_t n)
{
    if (fabs(*a - *a_trg) < 0.001f)
    {
        *a = *a_trg;
        return 0;
    }

    float cz = fabs(*a - *a_trg);

    if (cz <= 1)
        *a = *a_trg;
    else
    {
        if (cz < 10)
            n = static_cast<uint16_t>(n * cz * 0.1f);
        if (n < 10)
            n = 10;
        *a += (*a_trg - *a) / (n * 0.1f);
    }
    return 1;
}

/**
 * @brief 缓入动画插值
 */
uint8_t HugoUI::Animation_EasyIn(float *a, float *a_trg, uint16_t n)
{
    if (fabs(*a - *a_trg) < 0.001f)
    {
        *a = *a_trg;
        return 0;
    }

    float cz = fabs(*a - *a_trg);
    if (cz <= 1)
        *a = *a_trg;
    else if (cz > 20)
        n = n * 3;
    else if (cz > 15)
        n = n * 2;
    else if (cz > 5)
        n = n * 1;

    *a += (*a_trg - *a) / (n * 0.1f);

    return 1;
}

/**
 * @brief 过冲回弹动画插值
 * @param n: 弹性系数，越大回弹越慢、幅度越大
 */
uint8_t HugoUI::Animation_Elastic(float *a, float *a_trg, uint8_t n)
{
    static float vel = 0.0f;

    float delta = *a_trg - *a;

    // 结束判断
    if (fabs(delta) < 0.001f && fabs(vel) < 0.01f)
    {
        *a = *a_trg;
        vel = 0;
        return 0;
    }

    // 弹性系数 & 阻尼（可微调）
    float stiffness = 12.0f / (n + 1);
    float damping = 0.77f;

    // 弹簧物理公式（胡克定律）
    vel += delta * stiffness;
    vel *= damping;
    *a += vel;

    return 1;
}

/**
 * @brief 模糊转场效果
 */
uint8_t HugoUI::Animation_Blur(void)
{
    int len = 8 * oled_get_buffer_tile_height() * oled_get_buffer_tile_width();
    uint8_t *p = oled_get_buffer_ptr();
    uint8_t return_flag = 0;

    static uint8_t blur_effect_temp = 0; // UI模糊转场动效

    if (blur_effect_temp >= 6)
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 == 0)
                p[i] = p[i] & (0x55);
        }
    }
    if (blur_effect_temp >= 13)
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 != 0)
                p[i] = p[i] & (0xaa);
        }
    }
    if (blur_effect_temp >= 17)
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 == 0)
                p[i] = p[i] & (0x00);
        }
    }
    if (blur_effect_temp >= 20)
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 != 0)
                p[i] = p[i] & (0x00);
        }
    }

    blur_effect_temp += 1;
    if (blur_effect_temp > 21)
    {
        return_flag = 1;
        blur_effect_temp = 0;
    }

    return return_flag ? 0 : 1;
}
