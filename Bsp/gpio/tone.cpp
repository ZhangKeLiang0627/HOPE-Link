#include "tone.hpp"
#include <cmath>

// C4-B8的音调对应的频率大小
const uint16_t frequency[] = {
    // rest_note
    0,
    // C    C#   D   Eb    E    F    F#   G    G#   A   Bb    B
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
    523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 988,
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2218, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
    4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,
    // check_note
    0,
};

Tone::Tone(TIM_HandleTypeDef *_htim) : htim(_htim), config(Config_t{})
{
    config.enable = true;
    config.isContinue = false;
    config.volume = 80;
    config.count = 0;
    config.schedule = 0;
    config.size = 0;
}

// 计算对应的预重装值ARR（psc = 10 - 1）/ 用1000kHz 
uint16_t Tone::setNote(uint16_t frq)
{
    // 防止休止符时蜂鸣器怪叫
    if (frq == 0)
        return 0;
    float temp = 0.0f;
    temp = 1000000.0f / (float)frq;
    return (uint16_t)temp;
}

// 应用函数
void Tone::Play(const MusicSheet* music)
{
    // 该变量用于计算结构体数组的长度
    uint16_t noteLength;
    Stop();

    // 让全局结构体指针指向传入的乐曲
    _music = music;

    // 通过寻找检查位CHECK_NOTE来计算传入的结构体长度
    for (noteLength = 0; _music[noteLength].note != CHECK_NOTE; noteLength++)
        ;

    // 赋予长度大小
    config.size = noteLength;
    // 把音符表清零
    config.schedule = 0;

    // 开启蜂鸣器继续标志位
    config.isContinue = true;
    config.count = 0;
}

void Tone::Stop(void)
{
    htim->Instance->CCR2 = 0;               // 占空比设为0
    HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_2);  // 停止输出
}

// 用于10ms定时器中断进行循环 
void Tone::Update()
{
    // 判断是否继续
    if (config.isContinue && config.enable)
    {
        // 判断音符表走完没
        if (config.schedule <= config.size)
        {
            // 时间减短10ms
            config.count--;
            // 这个操作的意思是如果count = 65535时，意思就是延时结束了，这个音符演完了
            if (!(config.count < 65535))
            {
                // 给预重装载值赋值，改变音调
                htim->Instance->ARR = (uint16_t)setNote(frequency[_music[config.schedule].note]);
                // 给PWM占空比赋值，改变音量
                htim->Instance->CCR2 = (uint16_t)htim->Instance->ARR / (100 - config.volume);
                // 赋值新的延时长度给count
                config.count = _music[config.schedule].delay;
                // 音符表走到下一个音符
                config.schedule++;
                HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
            }
        }
        // 失能蜂鸣器，清空标志位
        else
            Stop();
    }
    else
        Stop();
}

// 乐曲
const Tone::MusicSheet BEEPER_KEYPRESS[] = {
    {NOTE_C6, 7},
    {CHECK_NOTE, 0}, // 检查位
};

const Tone::MusicSheet BEEPER_TRITONE[] = {
    {NOTE_B5, 6},
    {REST_NOTE, 2},
    {NOTE_D6, 6},
    {REST_NOTE, 2},
    {NOTE_F6, 6},
    {CHECK_NOTE, 0}, // 检查位
};

const Tone::MusicSheet BEEPER_WARNING[] = {
    {NOTE_F4, 5},
    {REST_NOTE, 2},
    {NOTE_F4, 5},
    {CHECK_NOTE, 0}, // 检查位
};
