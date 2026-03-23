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
    int64_t lastCount;          // 上一次的计数值，用于计算差值
    uint16_t diffThreshold;     // 差值阈值，过滤抖动
    EncoDirect dir;             // 当前旋转方向

    uint8_t *userData;

public:
    explicit Encoder(TIM_HandleTypeDef *_htim, bool _inverse = false, uint16_t _threshold = 2);

    void Start(uint8_t *_userData);

    void Update();

    int64_t GetCount();

    EncoDirect GetDirection();

    struct Config_t
    {
        bool inverse;
    };

    Config_t config;
};

#endif //REF_STM32F4_ENCODER_HPP
