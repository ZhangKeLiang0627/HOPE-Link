#include "oled_wrapper.h"
#include <stdio.h>
#include <math.h>

void oledInit(void)
{
    u8g2_init(&u8g2);
    oledSetFont(u8g2_font_wqy13_t_gb2312a);
}

void oledDrawStr(uint16_t x, uint16_t y, const char *str)
{
    u8g2_DrawStr(&u8g2, x, y, str);
}

void oledDrawUTF8(uint16_t x, uint16_t y, const char *str)
{
    u8g2_DrawUTF8(&u8g2, x, y, str);
}

/**
 * @brief   drawFloat/显示浮点数
 * @param   x        横坐标位置
 * @param   y        纵坐标位置
 * @param   dat      传入需要显示的数据
 * @param   num      小数点前有几位数
 * @param   pointNum 小数点后有几位数
 * @return  void
 */
static char formatControl[10];
static char numForShow[10];
void oledDrawFloat(uint16_t x, uint16_t y, float dat, uint8_t num, uint8_t pointNum)
{
    num += pointNum + 1;                                                //%m.nf中的m指该浮点数所有的打印空间（包括小数点和小数点后几位）
    sprintf(formatControl, "%s%d%c%d%c", "%", num, '.', pointNum, 'f'); // 控制小数点前几位和后几位

    sprintf(numForShow, formatControl, dat);
    sprintf(numForShow, "%.1f", dat);

    u8g2_DrawStr(&u8g2, x, y, numForShow); 
}

/**
 * @brief   drawNum/显示正数
 * @param   x        横坐标位置
 * @param   y        纵坐标位置
 * @param   dat      传入需要显示的数据
 * @return  void
 */
void oledDrawNum(uint16_t x, uint16_t y, uint16_t dat)
{
    sprintf(numForShow, "%d", dat);

    u8g2_DrawStr(&u8g2, x, y, numForShow);
}

/**
 * @brief   drawPixel/画点函数
 * @param   x        横坐标位置
 * @param   y        纵坐标位置
 * @return  void
 */
void oledDrawPoint(uint16_t x, uint16_t y)
{
    // no need to draw at all if we're offscreen
    if (x > OLED_SCREEN_WIDTH * 2 || y > OLED_SCREEN_HEIGHT * 2)
        return;
    u8g2_DrawPixel(&u8g2, x, y);
}

/**
 * @brief   drawBox/画实心矩形
 * @param   x        横坐标位置
 * @param   y        纵坐标位置
 * @param   width    矩形的宽度
 * @param   height   矩形的长度
 * @return  void
 */
void oledDrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    u8g2_DrawBox(&u8g2, x, y, width, height);
}

/**
 * @brief   drawFrame/画空心矩形
 * @param   x        横坐标位置
 * @param   y        纵坐标位置
 * @param   width    矩形的宽度
 * @param   height   矩形的长度
 * @return  void
 */
void oledDrawFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    u8g2_DrawFrame(&u8g2, x, y, width, height);
}

/**
 * @brief   DrawRFrame/画空心圆角矩形
 * @param   x        横坐标位置
 * @param   y        纵坐标位置
 * @param   width    矩形的宽度
 * @param   height   矩形的长度
 * @param   r        圆角半径
 * @return  void
 */
void oledDrawRFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r)
{
    u8g2_DrawRFrame(&u8g2, x, y, width, height, r);
}

/**
 * @brief   DrawRBox/画实心圆角矩形
 * @param   x        横坐标位置
 * @param   y        纵坐标位置
 * @param   width    矩形的宽度
 * @param   height   矩形的长度
 * @param   r        圆角半径
 * @return  void
 */
void oledDrawRBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r)
{
    u8g2_DrawRBox(&u8g2, x, y, width, height, r);
}

/**
 * @brief   清空发送缓冲区
 * @return  void
 */
void oledClearBuffer(void)
{
    u8g2_ClearBuffer(&u8g2);
}

/**
 * @brief   刷新发送缓冲区
 * @return  void
 */
uint8_t screenshotPrintFlag = 0; // 是否开启画面投屏标志位
void oledSendBuffer(void)
{
    u8g2_SendBuffer(&u8g2);
    if (screenshotPrintFlag)
        oledScreenshotPrint();
}

/**
 * @brief   反色函数/基于u8g2
 * @param   mode     颜色模式（0->透显/黑色 1->实显/白色 2->XOR模式/反色）如果对某物体进行反色处理记得设置回1模式
 * @return  void
 */
void oledSetDrawColor(uint8_t mode)
{
    u8g2_SetDrawColor(&u8g2, mode);
}

/**
 * @brief   DrawBMP/画图
 * @param   x        横坐标位置
 * @param   y        纵坐标位置
 * @param   width    矩形的宽度
 * @param   height   矩形的长度
 * @param   pic      图片
 * @return  void
 */
void oledDrawBMP(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pic)
{
    u8g2_DrawXBMP(&u8g2, x, y, width, height, pic);
}

// /**
//  * @brief   全屏反色函数/基于寄存器的
//  * @param   mode     颜色模式0->正常显示 1->反色显示
//  * @return  void
//  */
// void oledModifyColor(uint8_t mode)
// {
//     // TODO ...
// }

uint32_t oledGetUTF8Width(char *s)
{
    return u8g2_GetUTF8Width(&u8g2, s);
}

void oledDrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    u8g2_DrawLine(&u8g2, x1, y1, x2, y2);
}

void oledDrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    u8g2_DrawTriangle(&u8g2, x0, y0, x1, y1, x2, y2);
}

uint16_t oledGetBufferTilHeight(void)
{
    return u8g2_GetBufferTileHeight(&u8g2);
}

uint16_t oledGetBufferTileWidth(void)
{
    return u8g2_GetBufferTileWidth(&u8g2);
}

uint8_t* oledGetBufferPtr(void)
{
    return u8g2_GetBufferPtr(&u8g2);
}

void oledSetBitmapMode(uint8_t mode)
{
    u8g2_SetBitmapMode(&u8g2, mode);
}

void oledSetFlipMode(uint8_t mode)
{
    u8g2_SetFlipMode(&u8g2, mode);
}

// 位图缩放 代码片段改自arduboy2
void oledDrawSlowBitmapResize(int x, int y, const uint8_t *bitmap, int w1, int h1, int w2, int h2)
{
    uint8_t color = u8g2_GetDrawColor(&u8g2);
    float mw = (float)w2 / w1;
    float mh = (float)h2 / h1;
    uint8_t cmw = ceil(mw);
    uint8_t cmh = ceil(mh);
    int xi, yi, byteWidth = (w1 + 7) / 8;
    for (yi = 0; yi < h1; yi++)
    {
        for (xi = 0; xi < w1; xi++)
        {
            // if (*(uint8_t *)(bitmap + yi * byteWidth + xi / 8) & (1 << (7 - (xi & 7))))
            if (*(uint8_t *)(bitmap + yi * byteWidth + xi / 8) & (1 << (xi & 7))) // 修改后
            {
                u8g2_DrawBox(&u8g2, x + xi * mw, y + yi * mh, cmw, cmh);
            }
            else if (color != 2)
            {
                u8g2_SetDrawColor(&u8g2, 0);
                u8g2_DrawBox(&u8g2, x + xi * mw, y + yi * mh, cmw, cmh);
                u8g2_SetDrawColor(&u8g2, color);
            }
        }
    }
}

void oledDrawSlowBitmapResize2(int x, int y, const uint8_t *bitmap, int w1, int h1, int w2, int h2)
{
    uint8_t color = u8g2_GetDrawColor(&u8g2);
    float mw = (float)w2 / w1;
    float mh = (float)h2 / h1;
    uint8_t cmw = ceil(mw);
    uint8_t cmh = ceil(mh);
    int xi, yi, byteWidth = (w1 + 7) / 8;
    for (yi = 0; yi < h1; yi++)
    {
        for (xi = 0; xi < w1; xi++)
        {
            // if (*(uint8_t *)(bitmap + yi * byteWidth + xi / 8) & (1 << (7 - (xi & 7))))
            if (*(uint8_t *)(bitmap + yi * byteWidth + xi / 8) & (1 << (7 - (xi & 7))))
            {
                u8g2_DrawBox(&u8g2, x + xi * mw, y + yi * mh, cmw, cmh);
            }
            else if (color != 2)
            {
                u8g2_SetDrawColor(&u8g2, 0);
                u8g2_DrawBox(&u8g2, x + xi * mw, y + yi * mh, cmw, cmh);
                u8g2_SetDrawColor(&u8g2, color);
            }
        }
    }
    // uint8_t color = u8g2_GetDrawColor(&u8g2);
    // int xi, yi, intWidth = (w1 + 7) / 8;
    // for (yi = 0; yi < h1; yi++)
    // {
    //     for (xi = 0; xi < w1; xi++)
    //     {
    //         if (*(uint8_t *)(bitmap + yi * intWidth + xi / 8) & (128 >> (xi & 7)))
    //         {
    //             u8g2_DrawPixel(&u8g2, x + xi, y + yi);
    //         }
    //         else if (color != 2)
    //         {
    //             u8g2_SetDrawColor(&u8g2, 0);
    //             u8g2_DrawPixel(&u8g2, x + xi, y + yi);
    //             u8g2_SetDrawColor(&u8g2, color);
    //         }
    //     }
    // }
}

/* Oled手动设置反色函数 */
void oledSetInverseColor(uint8_t flag)
{
    if (flag)
    {
        u8g2_SendF(&u8g2, "c", 0xA7); // 反白
    }
    else
    {
        u8g2_SendF(&u8g2, "c", 0xA6); // 反黑
    }
}

/* 对于所有的Oled通用的调节屏幕亮度的函数 */
void oledSetScreenBacklight(uint8_t val)
{
    u8g2_SendF(&u8g2, "c", 0x81);                                                    
    u8g2_SendF(&u8g2, "c", val); 
}

/* 选择字库 */
void oledSetFont(const uint8_t *font)
{
    u8g2_SetFont(&u8g2, font);
}

/* Oled投屏 */
#include "interface_uart.h"
void oledScreenshotPrint(void)
{
    uint8_t head[] = {0xA5, 0xA5};
    HAL_UART_Transmit(&huart1, head, 2, 1000); // 固定请求头

    uint8_t *p = (uint8_t *)u8g2_GetBufferPtr(&u8g2);
    HAL_UART_Transmit(&huart1, p, 1024, 1000);

    uint8_t tail[] = {0x5A, 0x5A};
    HAL_UART_Transmit(&huart1, tail, 2, 1000); // 固定请求尾
}
