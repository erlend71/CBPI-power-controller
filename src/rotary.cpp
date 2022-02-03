
#include "rotary.h"
#include "config.h"



ESP32Encoder encoder;

void initEncoder(void)
{
    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder.attachSingleEdge(ENC_A_PIN, ENC_B_PIN);
    encoder.setFilter(1023);
    encoder.setCount(0);
    //button
    pinMode(ROTARY_BUTTON, INPUT_PULLUP); // inverted input
    pinMode(LED_BUILTIN, OUTPUT);         // inverted output
}

/**
 * @brief checks or updates encoder value 
 *
 * @param percent pointer to the percentage variable to be updated
 * @return  true if changed, otherwise false
 */
bool rotaryHousekeeping(uint *percent)
{
    static int32_t oldRotary = 0;
    int32_t rotary = encoder.getCount();

    if (rotary != oldRotary)    // the wheels are turning....
    {
        if (rotary > 100)
        {
            rotary = 100;
            encoder.setCount(100);
            return false;
        }
        if (rotary < 0)
        {
            rotary = 0;
            encoder.setCount(0);
            return false;
        }
        oldRotary = rotary;
        *percent = rotary;
        return true;
    }
    if (rotary != *percent) // percent has changed externally, update encoder
    {
        encoder.setCount(*percent);
        Serial.printf("Updated encoder from %i to %u", oldRotary, *percent);
        Serial.println();
        oldRotary = *percent;
    }
    return false;
}



byte checkEncoderButton(void)
{
    static u_long lastTime;
    static byte buttonState = BTN_OFF;
    static bool oldButton;
    u_long now = millis();
    if (now < lastTime + 50) // debounce
        return buttonState;        // return without checking
    bool encBtn = !digitalRead(ROTARY_BUTTON);
    if (encBtn != oldButton) // Button state changed
    {
        lastTime = now; // start debounce clock
        oldButton = encBtn;
        if (encBtn) // button just went down
        {
            buttonState = BTN_ON;     // for next iteration...
            return BTN_PRESSED; // now
        }
        else
        {
            buttonState = BTN_OFF;     // for next iteration...
            return BTN_RELEASED; // now
        }
    }
    return buttonState;
}