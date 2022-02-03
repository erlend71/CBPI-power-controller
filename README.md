# CBPI power controller

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://github.com/erlend71/mqtt_power/blob/master/LICENSE)

The purpose for this system is to enable a ESP32 connected to an Oled display and a rotary encoder to control and display power level and state of a [CBPI](http://www.craftbeerpi.com/) actor using MQTT.

Instead of struggling with a small touch screen and sliders on the CBPI frontend, the power level can be adjsted by rotating the knob or pushing to toggle the on/off state. This enables quick and precise control of i.e. an electrical heater element when boiling wort.

I've tested this on [Alexander Vollkopf's fork](https://github.com/avollkopf/craftbeerpi4) of craftbeerpi4, but it should be possible to make this run on CBPI 3 as well.

## Hardware

* ESP32
* I2C Oled display (currently optimized for for 64*128 resolution)
* Rotary encoder switch
* Wires, breadboard or other prototyping equipment
* Some kind of enclosure if you're using this on your brew day.

## Prerequisites

* CBPI installation up and running with [MQTT configured](https://openbrewing.gitbook.io/craftbeerpi4_support/readme/craftbeerpi-4-server/mqtt-connectivity)
* MQTT server
* Basic knowledge of electronics and programming
* [PlatformIO IDE](https://platformio.org/) installed (I'm using VS code)
  
If you are still using Arduino IDE, this is when you want to step up the game and get started with VS Code and PlatformIO. It's much more sexy than Arduino :-)
  
## Functional description

After booting the ESP tries to establish connection to your configured WiFi, and then connects to the MQTT server.

When rotating the encoder, the display updates continuously showing the percentage setting, and after a short idle time a MQTT message will be published to update the CBPI actor power level. If the button is pressed the actor state (on/off) will be toggled and a corresponding MQTT message is published.

The ESP subscribes for actor update messages and will update the display with power level and state accordingly if a MQTT message is received due to actor update on the CBPI frontend.

## Configuration

I've tried to collect all config parameters  in the file [`config.h`](https://github.com/erlend71/mqtt_power/blob/master/src/config.h) and configuration should be quite straightforward when looking at the file.

Personal information is kept in a file called `secrets.h` which you have to make yourself since it's on the .gitignore list. This is where I keep my SSID, passwords etc. For personal use you can comment out `#include "secrets.h"`, and add the missing #defines directly in `config.h`.

## NOTE

MQTT configuration is still hardcoded in the program, and needs to be changed to make anything work since CBPI generates random ID's for each actor, and names will also change. If you want to try the code as it is now, this needs to be changed everywhere the `client.publish()` or `client.subscribe()` method is called

```CPP
if (!client.subscribe("cbpi/actorupdate/QrfdzP7hdUjYEqm2ELCpsU"))
        Serial.println(F("sub failed"));
if (powerIsOn)
    client.publish("cbpi/actor/QrfdzP7hdUjYEqm2ELCpsU/on", "");
    ...
```

The plan is to extract this ID automatically when a actor update message is published from CBPI based on the configured actor name we want to control.

## References and further reading

* [Facebook CBPI user forum](https://www.facebook.com/groups/craftbeerpi/)
* Alexander Vollkopf's [CBPI4 Manual](https://openbrewing.gitbook.io/craftbeerpi4_support/)
