
#include "config.h"
#include "rotary.h"
#include "mqtt.h"
#include "display.h"
#include <PubSubClient.h>

// Global variables...
bool powerIsOn;
uint percent = 0; // current setting of power

void setup()
{
    Serial.begin(115200);
    Serial.println(F("We're connected"));

    initDisplay();
    delay(300);
    initEncoder();
    updateScreen(-1, 0, "Connecting WiFi...");
    setup_wifi();
    updateScreen(-1, 0, "Connecting MQTT...");
    init_mqtt();
    delay(300);

    if (!client.subscribe("cbpi/actorupdate/#"))
        Serial.println(F("sub failed"));
    else
        Serial.println(F("sub OK"));
}

void loop()
{
    static u_long lastTransmission = 0, mqDelay = 0;
    u_long now = millis();

    switch (checkEncoderButton())
    {
    case BTN_PRESSED:
        powerIsOn = !powerIsOn; // toggle power state
        digitalWrite(LED_BUILTIN, powerIsOn);
        if (powerIsOn)
        {
            // String _atopic = String("cbpi/actor/") + actorID + "/on";
            // client.publish(_atopic.c_)
            String crazy = String("cbpi/actor/") + actorID + "/on";
            // client.publish(_topic.c_str(), "");
            client.publish(crazy.c_str(), "");
            // client.publish("cbpi/actor/QrfdzP7hdUjYEqm2ELCpsU/on", "");
        }
        else
        {
            // String _sendTopic = "cbpi/actor/QrfdzP7hdUjYEqm2ELCpsU/off";
            // String _header = "cbpi/actor/";
            //  char _string[] = header.c_str() ;//sendTopic.c_string();
            //  client.publish(sendTopic.c_string(), "");
            String _topic = String("cbpi/actor/") + actorID + "/off";
            client.publish(_topic.c_str(), "");
            // client.publish("cbpi/actor/" + actorID + "/off", "");
        }
        updateScreen(powerIsOn, percent, "Sent");
        lastTransmission = now;
        mqtt_dontListen = true;
        Serial.println(powerIsOn ? "Sent on cmd" : "Sent off cmd"); /* code */
        break;
    default:
        break;
    }

    // return to Idle text on display...
    if (now > lastTransmission + 2000 && mqDelay == 0)
    {
        updateScreen(powerIsOn, percent, "Idle");
    }

    // returns true if encoder has changed
    if (rotaryHousekeeping(&percent))
    {
        Serial.print("Power value: ");
        Serial.println(percent);
        updateScreen(powerIsOn, percent, "Sending...");
        mqDelay = now; // trigger sending in a few moments
    }

    // publish MQTT data
    if (mqDelay && now > mqDelay + 100) // 200 ms since we stopped rotating,
    {
        // this must be fixed.....
        String header = "cbpi/actor/";
        // String guid = "QrfdzP7hdUjYEqm2ELCpsU";
        String item = "/power";
        String topic = header + actorID + item;
        // const char *topic_c = added.c_str();

        char payload[5];
        sprintf(payload, "%d", percent);

        client.publish(topic.c_str(), payload);
        updateScreen(powerIsOn, percent, "Sent");
        Serial.print("sent MQTT ");
        Serial.println(topic);
        Serial.println(payload);
        mqDelay = 0;
        lastTransmission = now;
        mqtt_dontListen = true; // Silence! I'll kill you!!!!
    }

    // ignore received updates just after sending, just in case it's fake or old news
    if (now > lastTransmission + 100)
        mqtt_dontListen = false; // ready to receive again..

    if (mqtt_updateReceived)
    {
        mqtt_updateReceived = false;
        lastTransmission = now;
        updateScreen(powerIsOn, percent, "Rx update");
        digitalWrite(LED_BUILTIN, powerIsOn);
    }

    if (!client.connected())
    {
        updateScreen(-1, percent, "Reconnecting MQTT...");
        reconnect();
    }

    // let the MQTT client do its stuff
    client.loop();
}
