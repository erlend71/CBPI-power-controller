#pragma once
#include <ESP32Encoder.h>

enum buttonState
{
    BTN_OFF = 0,
    BTN_PRESSED = 1,
    BTN_ON = 2,
    BTN_RELEASED = 3
};

bool rotaryHousekeeping(uint *percent);
void initEncoder(void);

/**
 * @brief Maintains all reading of encoder and reports states  
 * Note: states pressed and released will be reported only once
 * so store state if needed.
 * 
 * @return Enum describing state and events (off, pressed, on, released)
 */
byte checkEncoderButton(void);
