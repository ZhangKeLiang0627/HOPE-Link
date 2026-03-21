#ifndef __OLED_WRAPPER_H
#define __OLED_WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "u8g2_init.h"

/* u8g2_SetDrawColor param -----------------------------------------------------*/
#define OLED_NORMAL        1
#define OLED_XOR           2
#define OLED_SCREEN_WIDTH  128
#define OLED_SCREEN_HEIGHT 64

/* API -----------------------------------------------------*/
void oledInit(void);
void oledDrawStr(uint16_t x, uint16_t y, const char *str);
void oledDrawUTF8(uint16_t x, uint16_t y, const char *str);
void oledDrawFloat(uint16_t x, uint16_t y, float dat, uint8_t num, uint8_t pointNum);
void oledDrawNum(uint16_t x, uint16_t y, uint16_t dat);
void oledDrawPoint(uint16_t x, uint16_t y);
void oledDrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void oledDrawFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void oledDrawRFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r);
void oledDrawRBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r);
void oledDrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void oledClearBuffer(void);
void oledSendBuffer(void);
void oledSetDrawColor(uint8_t mode);
void oledDrawBMP(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pic);
uint32_t oledGetUTF8Width(char *s);
void oledDrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
uint16_t oledGetBufferTilHeight(void);
uint16_t oledGetBufferTileWidth(void);
uint8_t *oledGetBufferPtr(void);
void oledSetBitmapMode(uint8_t mode);
void oledSetFlipMode(uint8_t mode);
void oledDrawSlowBitmapResize(int x, int y, const uint8_t *bitmap, int w1, int h1, int w2, int h2);
void oledDrawSlowBitmapResize2(int x, int y, const uint8_t *bitmap, int w1, int h1, int w2, int h2);
void oledSetInverseColor(uint8_t flag);
void oledSetScreenBacklight(uint8_t val);
void oledSetFont(const uint8_t *font);
void oledScreenshotPrint(void);

#ifdef __cplusplus
}
#endif

#endif