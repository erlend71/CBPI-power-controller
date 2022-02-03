#include "display.h"
#include "config.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

extern bool powerIsOn;

/**
 * @brief Updates screen with new values for power setting
 *
 * @param power power in %
 * @param statusline shown at bottom of screen
 * @param display Reference to display object to be used
 */

void initDisplay(void)
{
    Wire1.begin(SDA_OLED, SCL_OLED);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.clearDisplay();
    updateScreen(-1, 0, "Startup...");
}

void updateScreen(int pwrState, float percent, String statusline)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0); // Start at top-left corner
    display.print(F("Power "));
    if (pwrState == 0)
        display.print(F("off"));
    else if (pwrState == 1)
        display.print(F("on"));
    else
        display.print(F("-"));
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 27);
    display.setTextSize(3);
    display.printf("%3.f", percent);
    display.print(F(" %"));
    display.setTextSize(1);
    display.setCursor(0, 56);
    display.print(statusline);
    display.display();
}