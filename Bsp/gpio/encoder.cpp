#include "encoder.hpp"

Encoder::Encoder(TIM_HandleTypeDef *_htim, bool _inverse) :
    htim(_htim), config(Config_t{})
{
    config.inverse = _inverse;
}

void Encoder::Start(uint8_t *_userData)
{
    userData = _userData;
    htim->Instance->CNT = 32768;
    HAL_TIM_Encoder_Start_IT(htim, TIM_CHANNEL_ALL);
}

void Encoder::Update()
{
    int8_t Encoder_Num = 0;
    static uint8_t Encoder_ReturnNum1, Encoder_ReturnNum2;

    /* 读取输入值 */
    Encoder_Num = GetEncoderCount(htim->Instance);
    /* 没有输入就返回0 */
    if (Encoder_Num == 0)
        return ;

    if (Encoder_Num > 0)
    {
        Encoder_ReturnNum1 += Encoder_Num;
    }
    else
    {
        Encoder_ReturnNum2 -= Encoder_Num;
    }

    if (Encoder_ReturnNum1 >= 2)
    {
        Encoder_ReturnNum1 = 0;
        Encoder_ReturnNum2 = 0;
        *userData = config.inverse ? 1 : 2;
    }
    else if (Encoder_ReturnNum2 >= 2 || (Encoder_ReturnNum1 == 1 && Encoder_ReturnNum2 == 1))
    {
        Encoder_ReturnNum1 = 0;
        Encoder_ReturnNum2 = 0;
        *userData = config.inverse ? 2 : 1;
    }
}
