#ifndef _TONE_H_
#define _TONE_H_

#include <cstdint>
#include <tim.h>

/*音调频率表对应的每一个音符名称*/
enum MusicalNote
{
    // 休止符
    REST_NOTE,

    NOTE_C4, NOTE_D4b, NOTE_D4, NOTE_E4b, NOTE_E4, NOTE_F4,
    NOTE_G4b, NOTE_G4, NOTE_A4b, NOTE_A4, NOTE_B4b, NOTE_B4,

    NOTE_C5, NOTE_D5b, NOTE_D5, NOTE_E5b, NOTE_E5, NOTE_F5,
    NOTE_G5b, NOTE_G5, NOTE_A5b, NOTE_A5, NOTE_B5b, NOTE_B5,

    NOTE_C6, NOTE_D6b, NOTE_D6, NOTE_E6b, NOTE_E6, NOTE_F6,
    NOTE_G6b, NOTE_G6, NOTE_A6b, NOTE_A6, NOTE_B6b, NOTE_B6,

    NOTE_C7, NOTE_D7b, NOTE_D7, NOTE_E7b, NOTE_E7, NOTE_F7,
    NOTE_G7b, NOTE_G7, NOTE_A7b, NOTE_A7, NOTE_B7b, NOTE_B7,

    NOTE_C8, NOTE_D8b, NOTE_D8, NOTE_E8b, NOTE_E8, NOTE_F8,
    NOTE_G8b, NOTE_G8, NOTE_A8b, NOTE_A8, NOTE_B8b, NOTE_B8,

    // 检查位
    CHECK_NOTE,
};

class Tone
{
private:
    TIM_HandleTypeDef *htim;

public:
    struct Config_t
    {
        bool enable;
        bool isContinue;
        uint16_t count;
        uint16_t size;
        uint16_t schedule;
        uint16_t volume;
    };

    Config_t config;

    struct MusicSheet
    {
        MusicalNote note;
        uint16_t delay;
    };
    const MusicSheet *_music;

public:
    explicit Tone(TIM_HandleTypeDef *_htim);

    void Start();
    void Stop();
    
    void Play(const MusicSheet *music);
    void Update();

    uint16_t setNote(uint16_t frq);

};

// 乐曲
extern const Tone::MusicSheet BEEPER_KEYPRESS[];
extern const Tone::MusicSheet BEEPER_TRITONE[];
extern const Tone::MusicSheet BEEPER_WARNING[];

#endif //_TONE_H_
