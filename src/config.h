#pragma once

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH    128     // OLED display width, in pixels
#define SCREEN_HEIGHT   64      // OLED display height, in pixels
#define SDA_OLED        4       // Separate i2c-bus #1 for oled
#define SCL_OLED        15
#define OLED_RESET      16      // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS  0x3C    // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// Pins used for Rotary swinch
#define ENC_A_PIN       21
#define ENC_B_PIN       13
#define ROTARY_BUTTON   35 

// ESP led
#define LEDPIN 12 // for onboard LED

// IP and server config 
#define MQTT_SERVER     "192.168.0.222"
#define MQTT_PORT       1883

#define ESP_IP          10, 0, 0, 98
#define ESP_GW          10, 0, 0, 1
#define ESP_SUBNET      255, 255, 255, 0

#include "secrets.h"     // remember to .gitignore this. Contains the lines below with real passwords
//#define WIFI_SSID "wifi_ssid"
//#define WIFI_PASSWORD "wifi_password"
//#define MQTT_USER ""
//#define MQTT_PASS ""

//actor we want to control...
#define ACTOR_NAME "Boiler Heater "