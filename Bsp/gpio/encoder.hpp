#ifndef REF_STM32F4_ENCODER_HPP
#define REF_STM32F4_ENCODER_HPP

#include "tim.h"

class Encoder
{
private:
    const float RAD_TO_DEG = 57.295777754771045f;

    TIM_HandleTypeDef *htim;

public:
    explicit Encoder(TIM_HandleTypeDef *_htim, uint16_t _cpr = 4096, bool _inverse = false);

    void Start();

    int64_t GetCount();

    float GetAngle(bool _useRAD = false);

    struct Config_t
    {
        uint16_t cpr;

        bool inverse;
    };

    Config_t config;
};

#endif //REF_STM32F4_ENCODER_HPP
