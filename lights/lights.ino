/*
  Devices - Lights

  created 2015
  by Intelliroads, LLC.
 */
#include "MQTT/MQTT.h"

#define TOPIC_BASE "lights/1/"

/* MQTT settings */
char* clientId = "lights_module_1";

/* Access settings */
byte server[] = { 172, 20, 10, 2 }; // MQTT server address

/* MQTT communication */
void callback(char* topic, byte* payload, unsigned int length); // subscription callback
MQTT client(server, 1883, callback); // mqtt client

/* Pins configuration */
int outPins[] = {D0, D1, D2, D3}; // led modules

/* Led logics */
int state = LOW;

/* MQTT topics */
char* topics[] = {"lights/1/0", "lights/1/1", "lights/1/2", "lights/1/3"};

/* Spark Core setup */
void setup() {
    Serial.begin(9600);
    delay(5000);

    Serial.println("Starting connection to server...");

    mqttConnect(); // MQTT server connection

    // led pins setup
    for (int i = 0; i < arraySize(outPins); i++) {
        pinMode(outPins[i], OUTPUT);
    }
}

/* Spark Core loop */
void loop() {
    mqttConnect();
}

/* MQTT server connection */
void mqttConnect() {
    // Reconnection logic
    if (!client.isConnected()) {
        // Connection to MQTT server
        if (client.connect(clientId)) {
            Serial.println("Successfully connected with MQTT");
            mqttSubscribe();
        }
    }
    client.loop();
}

/* MQTT subscribe */
void mqttSubscribe() {
    // topics subscription
    for (int i = 0; i < arraySize(topics); i++) {
        client.subscribe(topics[i]);
    }
}

/* Receive server message and confirm the physical change */
void callback(char* topic, byte* payload, unsigned int length) {
    // Copy the payload content into a char*
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);

    // Update the physical status
    if (message.equals("on")) {
        Serial.println("Led turned on");
        state = HIGH;
    } else {
        Serial.println("Led turned off");
        state = LOW;
    }

    digitalWrite(outPins[0], state);
}
