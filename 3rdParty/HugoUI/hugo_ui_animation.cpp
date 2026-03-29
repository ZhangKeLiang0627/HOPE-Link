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
uint8_t HugoUI::Transition_Blur(void)
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

// void astra_draw_exit_animation()
// {
//     // 完成完整的退场动画 astra_exit_animation_status的取值依次如下
//     // 0 触发退场动画 遮罩开始落下
//     // 1 遮罩落下完成 此时屏幕被遮罩填满 开始变更背景内容
//     // 2 遮罩开始抬升
//     // 0 遮罩抬升完成 退场动画完成
//     static float _temp_h = -8;
//     static float _temp_h_trg = OLED_HEIGHT + 8;

//     oled_set_draw_color(0);
//     oled_draw_box(0, 0, OLED_WIDTH, _temp_h); // 遮罩
//     oled_set_draw_color(1);

//     // 沙漏
//     uint8_t _x_hourglass_offset = OLED_WIDTH / 2 - 8;
//     int8_t _y_hourglass = _temp_h - OLED_HEIGHT / 2 - 18;
//     if (_y_hourglass + 20 >= 0)
//     {
//         // 绘制顶部和底部矩形及中间擦除
//         oled_draw_box(_x_hourglass_offset, _y_hourglass + 2, 13, 3);
//         oled_set_draw_color(0);
//         oled_draw_H_line(_x_hourglass_offset + 2, _y_hourglass + 3, 9);
//         oled_set_draw_color(1);

//         // 主体结构
//         oled_draw_V_line(_x_hourglass_offset + 1, _y_hourglass + 4, 5);
//         oled_draw_V_line(_x_hourglass_offset + 11, _y_hourglass + 4, 5);

//         // 斜线部分循环绘制
//         for (uint8_t i = 0; i < 5; ++i)
//         {
//             int8_t _current_y = _y_hourglass + 8 + i;
//             int8_t _left_x = (i < 3) ? (_x_hourglass_offset + 1 + i) : (_x_hourglass_offset + 4);
//             int8_t _right_x = (i < 3) ? (_x_hourglass_offset + 10 - i) : (_x_hourglass_offset + 7);
//             oled_draw_H_line(_left_x, _current_y, 2);
//             oled_draw_H_line(_right_x, _current_y, 2);
//         }

//         // 中间收口部分
//         for (uint8_t i = 0; i < 3; ++i)
//         {
//             int8_t _current_y = _y_hourglass + 13 + i;
//             oled_draw_H_line(_x_hourglass_offset + 3 - i, _current_y, 2);
//             oled_draw_H_line(_x_hourglass_offset + 8 + i, _current_y, 2);
//         }

//         // 底部竖线
//         oled_draw_V_line(_x_hourglass_offset + 1, _y_hourglass + 16, 3);
//         oled_draw_V_line(_x_hourglass_offset + 11, _y_hourglass + 16, 3);

//         // 底部矩形
//         oled_draw_box(_x_hourglass_offset, _y_hourglass + 19, 13, 3);
//         oled_set_draw_color(0);
//         oled_draw_H_line(_x_hourglass_offset + 2, _y_hourglass + 20, 9);
//         oled_set_draw_color(1);

//         // 散点像素数组化绘制
//         const uint8_t _points[][2] = {
//             {5, 7}, {7, 7}, {6, 8}, {6, 10}, {6, 14}, {6, 16}, {5, 17}, {7, 17}, {4, 18}, {6, 18}, {8, 18}};
//         for (uint8_t i = 0; i < sizeof(_points) / sizeof(_points[0]); ++i)
//             oled_draw_pixel(_x_hourglass_offset + _points[i][0], _y_hourglass + _points[i][1]);
//     }

//     if (_temp_h + 3 >= 0)
//         // 下面是遮罩下方横线
//         for (uint8_t i = 0; i <= 3; ++i)
//             oled_draw_H_line(0, _temp_h + i, OLED_WIDTH);

//     // 棋盘格过渡
//     for (int16_t i = 0; i <= OLED_WIDTH; i += 2)
//         for (int16_t j = _temp_h - 5; j <= _temp_h - 1; j++)
//         {
//             if (j % 2 == 0)
//                 oled_draw_pixel(i + 1, j);
//             if (j % 2 == 1)
//                 oled_draw_pixel(i, j);
//         }

//     astra_exit_animation(&_temp_h, _temp_h_trg, 94);

//     // 下落过程
//     if (astra_exit_animation_status == 0 && _temp_h == _temp_h_trg && _temp_h == OLED_HEIGHT + 8)
//     {
//         astra_exit_animation_status = 1; // 落下来了
//         return;
//     }