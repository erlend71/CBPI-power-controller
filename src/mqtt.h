#pragma once

//MQTT
#include <Ethernet.h>
#include <Wifi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
// https://pubsubclient.knolleary.net/api#PubSubClient1


extern PubSubClient client;
extern bool mqtt_dontListen; //
extern bool mqtt_updateReceived;
extern String str_actorID;
extern char  actorID[30];

// Callback function header
void mqtt_rx_callback(char *topic, byte *payload, unsigned int length);

void init_mqtt(void);
void setup_wifi(void);
void reconnect(void);
int decodeJson(char *payload);
int parseJson(void);


