#ifndef REF_STM32F4_ENCODER_HPP
#define REF_STM32F4_ENCODER_HPP

#include "tim.h"

// 编码器方向枚举
typedef enum
{
    NONE = 0, // 无转动
    CW = 1,   // 正转（顺时针）
    CCW = 2   // 反转（逆时针）
} EncoDirect;

class Encoder
{
private:

    TIM_HandleTypeDef *htim;
    uint8_t *userData;

public:
    explicit Encoder(TIM_HandleTypeDef *_htim, bool _inverse = false);

    void Start(uint8_t *_userData);

    void Update();

    struct Config_t
    {
        bool inverse;
    };

    Config_t config;
};

#endif //REF_STM32F4_ENCODER_HPP
