#include "mqtt.h"
#include "config.h"

/**
 * https://openbrewing.gitbook.io/craftbeerpi4_support/readme/craftbeerpi-4-server/mqtt-connectivity
 *
 */

// local variables
bool mqtt_dontListen; // Used to disable MQTT reception
bool mqtt_updateReceived;
char actorID[30];
String str_actorID;

WiFiClient espClient;
PubSubClient client(MQTT_SERVER, MQTT_PORT, mqtt_rx_callback, espClient);

// for json decoding
//#define MSG_BUFFER_SIZE (300)
// char msg[MSG_BUFFER_SIZE];
StaticJsonDocument<300> doc; // used for all occations

// global varialbes defined in main
extern bool powerIsOn;
extern u_short percent;

void init_mqtt(void)
{
    reconnect();
}

void setup_wifi()
{
    // config can be omitted if you wat to use DHCP
    WiFi.config(IPAddress(ESP_IP), IPAddress(ESP_GW), IPAddress(ESP_SUBNET)); //  ip, gw, subnet, dns);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        digitalWrite(LED_BUILTIN, true);
        Serial.print(".");
        delay(100);
        digitalWrite(LED_BUILTIN, false);
    }
    // randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
}

/**
 * @brief Called to (re)connect to MQTT broker.
 *
 */
void reconnect()
{
    randomSeed(micros());
    uint16_t loop = 0; // restart counter

    // Loop until we're reconnected
    while (!client.connected())
    {
        digitalWrite(LED_BUILTIN, true);
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESPClient-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("cbpi/fancyPowerController/", "I'm alive!");
        }
        else // no luck
        {
            digitalWrite(LED_BUILTIN, false);
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
            loop++;
        }

        // restart wifi if we're not able to connect to MQTT server
        if (loop > 5)
        {
            Serial.println("Restarting WiFi...");
            loop = 0;
            WiFi.disconnect();
            delay(1000);
            setup_wifi();
        }
    }
    digitalWrite(LED_BUILTIN, false);
}

void mqtt_rx_callback(char *topic, byte *payload, unsigned int length)
{
    char rcvdPayload[300];
    Serial.println();
    // Copy message to rx string
    if (mqtt_dontListen) // do noting now
        return;
    int n = memcmp(topic, "cbpi/actorupdate", 16);
    if (n == 0)
    {
        Serial.print("received correct topic: ");
        Serial.println(topic);
    }
    else
    {
        Serial.print("received another topic: ");
        Serial.println(topic);
    }
    // store received json data
    memcpy(rcvdPayload, payload, length);
    rcvdPayload[length] = 0; // add null termination
    // Serial.println();
    Serial.print("Rx: ");
    Serial.println(rcvdPayload);
    decodeJson(rcvdPayload);
    parseJson();
}

// StaticJsonDocument<250> doc;
int decodeJson(char *payload)
{

    //  Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload);

    // Test if parsing succeeds.
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return -1;
    }
    return 1;
}

int parseJson(void)
{
    static u_short oldPct = 0;
    String actName = doc["name"];
    if (memcmp(actName.c_str(), ACTOR_NAME, strlen(ACTOR_NAME)) == 0)
    {
        String _actorID = doc["id"];
        //actorID = doc["id"].as< char *>();
        strlcpy(actorID, _actorID.c_str(), _actorID.length()+1);
        Serial.print("Name OK, id ");
        Serial.println(actorID);
  

        // Check if state has changed
        if (doc["state"] == true)
        {
            if (!powerIsOn)
                mqtt_updateReceived = true;
            powerIsOn = true;
        }
        else
        {
            if (powerIsOn)
                mqtt_updateReceived = true;
            powerIsOn = false;
        }
        percent = doc["power"];
        Serial.print("Power on: ");
        Serial.print(doc["state"] == true ? "Yes" : "No");
        Serial.print(", power: ");
        Serial.println(percent);
        // check if percent has changed
        if (oldPct != percent)
            mqtt_updateReceived = true;
        oldPct = percent;
        return 1;
    }
    else // something else arrived
    {

        Serial.print("Failure, ");
        Serial.print("received actor name '");
        Serial.print(actName);
        Serial.println("'");
        return 0;
    }
}
