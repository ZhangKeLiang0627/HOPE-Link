#include "encoder.hpp"

Encoder::Encoder(TIM_HandleTypeDef *_htim, bool _inverse, uint16_t _threshold) :
    htim(_htim), config(Config_t{}), lastCount(0), diffThreshold(_threshold), dir(NONE)
{
    config.inverse = _inverse;
}

int64_t Encoder::GetCount()
{
    int64_t count = GetCntLoop(htim->Instance) * 65536 + htim->Instance->CNT;

    return config.inverse ? -count : count;
}

void Encoder::Start(uint8_t *_userData)
{
    userData = _userData;
    ClearCntLoop(htim->Instance);
    htim->Instance->CNT = 0;
    HAL_TIM_Encoder_Start_IT(htim, TIM_CHANNEL_ALL);
}

void Encoder::Update()
{
    if(!userData)
        return;

    int64_t curCount = GetCount();
    int64_t diff = curCount - lastCount;

    if(!diff)
        return;

    dir = EncoDirect::NONE;

    if (diff >= diffThreshold)
    {
        dir = EncoDirect::CW; // 正转
        *userData = 2;
    }
    else if (diff <= -diffThreshold)
    {
        dir = EncoDirect::CCW; // 反转
        *userData = 1;
    }
    // 更新上次计数值
    if (dir != EncoDirect::NONE)
    {
        lastCount = curCount;
    }
}

// 获取当前方向
EncoDirect Encoder::GetDirection()
{
    Update();
    return dir;
}
