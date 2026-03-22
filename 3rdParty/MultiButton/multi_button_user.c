#include "multi_button_user.h"
#include "gpio.h"

#define USE_ENCODER 1

// static Button key0;
// static Button key1;
static Button key2;

void keyInit(uint8_t *user_key)
{
#if !USE_ENCODER

#else
	button_init(&key2, read_key_gpio, 0, 0);
	button_attach(&key2, BTN_LONG_PRESS_START, key2_long_press_start_handler, user_key);
	button_attach(&key2, BTN_SINGLE_CLICK, key2_single_click_handler, user_key);
	button_start(&key2);
#endif
}

uint8_t read_key_gpio(uint8_t button_id)
{   
    switch (button_id) {
        case 0:
            return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
        default:
            return 0;
    }
}

void key2_single_click_handler(Button* btn, void* user_data)
{
	user_data = 1;
}

void key2_long_press_start_handler(Button* btn, void* user_data)
{
	user_data = 2;
}
