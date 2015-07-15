/*
  Devices - Sensors

  created 2015
  by Intelliroads, LLC.
*/
#include "HttpClient/HttpClient.h"

#define HOSTNAME "192.168.1.101"
#define PORT 3000
#define URL "/readings"

#define SENSOR_ID "55799f279dca8a7f55abc5e8"
#define DISTANCE 0.015

/* HTTP client configuration */
HttpClient http;
http_request_t request;
http_response_t response;

// Set headers
http_header_t headers[] = {
    { "Content-Type", "application/json" },
    { NULL, NULL }
};

/* Pins configuration */
int inPinsA[] = {D0, D2, D4, D6}; // sensors A
int inPinsB[] = {D1, D3, D5, D7}; // sensors B

/* Sensors logics */
int readingA;
int readingB;
int previousA[] = {LOW, LOW, LOW, LOW};
int previousB[] = {LOW, LOW, LOW, LOW};
long tA[] = {0, 0, 0, 0};
long tB = 0;

// /* Sensor settings */
float distance = 0.015;
float speed;
char speedStr[64];
float period;
char periodStr[64];

/* Spark Core setup */
void setup() {
    // sensors A pin setup
    for (int i = 0; i < arraySize(inPinsA); i++) {
        pinMode(inPinsA[i], INPUT);
    }

    // sensors B pin setup
    for (int i = 0; i < arraySize(inPinsB); i++) {
        pinMode(inPinsB[i], INPUT);
    }
}

/* Spark Core loop */
void loop() {
    for (int i = 0; i < arraySize(inPinsA); i++) {
        readingA = digitalRead(inPinsA[i]); // read the sensor A state
        if (readingA == HIGH && previousA[i] == LOW) {
            tA[i] = millis();
        }
        previousA[i] = readingA;

        readingB = digitalRead(inPinsB[i]); // read the sensor B state
        if (readingB == HIGH && previousB[i] == LOW) {
            tB = millis();

            // Calculate speed
            speed = distance / (((float) tB - (float) tA[i]) / 1000);
            period = distance / speed;

            // Post reading
            postReading(speed, period);
        }
        previousB[i] = readingB;
    }
}

int postReading(float speed, float period) {
    request.hostname = HOSTNAME;
    request.port = PORT;
    request.path = URL;

    // Build JSON to send as request body
    sprintf(speedStr, "%f", speed);
    sprintf(periodStr, "%f", period);
    String json = readingJson(speedStr, periodStr);
    request.body = json;

    http.post(request, response, headers);
    if (response.status >= 200 && response.status < 400) {
        return 1;
    }
    return -1;
}

String readingJson(String speed, String period) {
    return "{ \"speed\": \"" + speed + "\", " +
            "\"period\": " + period + ", " +
            "\"sensorId\": " + SENSOR_ID + " }";
}
