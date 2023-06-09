#include <Wire.h>

#include <ESP8266WiFi.h>

#include <MPU6050.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>


MPU6050 mpu;

String ifttt_url = "https://maker.ifttt.com/trigger/fall_detected/with/key/dNwHOXwK7yV236JwtlJXSOiUbMfZYEI-4-P2Pv1rFDU";

const int THRESHOLD = 64000; // adjust as needed

const int BUTTON_PIN = D3; // the pin number of the push button

const int LED_PIN = D4; // the pin number of the LED

void setup() {

  Serial.println("Hi");

  Serial.begin(115200);

  Wire.begin();

  mpu.initialize();

  mpu.setFullScaleAccelRange(2);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // set the push button pin as input

  pinMode(LED_PIN, OUTPUT); // set the LED pin as output

  WiFi.begin("Vish", "12345678");

  // while(WiFi.status() != WL_CONNECTED) {

  //   Serial.println("Connecting to WiFi...");

  // }

  // Serial.println("Connected to WiFi");

}

void loop() {

  // check if the push button is pressed

  if (digitalRead(BUTTON_PIN) == LOW) {

    digitalWrite(LED_PIN, HIGH); // turn on the LED

  } else {

    digitalWrite(LED_PIN, LOW); // turn off the LED

  }

  WiFiClientSecure client;

  int16_t ax, ay, az;

  mpu.getAcceleration( & ax, & ay, & az);

  int sum = abs(ax) + abs(ay) + abs(az);

  if (sum > THRESHOLD) {

    digitalWrite(LED_PIN, HIGH); // turn on the LED

    tone(D5, 5, 500); // turn on the buzzer for 500 milliseconds

    delay(500); // wait for 500 milliseconds

    digitalWrite(LED_PIN, LOW); // turn off the LED

    noTone(D5); // turn off the buzzer

    Serial.println("Fall detected!");

    Serial.println(sum);

    Serial.println("Fall detected! Sending webhook to IFTTT...");

    Serial.print("Connecting to ");

    // Serial.println(host);

    // Use WiFiClient class to create TCP connections

    WiFiClient client;

    const int httpPort = 80;

    const char * host = "maker.ifttt.com";

    if (!client.connect(host, httpPort)) {

      Serial.println("Connection failed");

      return;

    }

    // We now create a URI for the request

    String url = "https://maker.ifttt.com/trigger/fall_detected/with/key/Zj6G0I9cr7CcqJe8h0sdL";

    Serial.print("Requesting URL: ");

    Serial.println(url);

    // This will send the request to the server

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +

      "Host: " + host + "\r\n" +

      "Connection: close\r\n\r\n");

    while (client.connected())

    {

      if (client.available())

      {

        String line = client.readStringUntil('\r');

        Serial.print(line);

      } else {

        // No data yet, wait a bit

        delay(50);

      };

    }

    Serial.println();

    Serial.println("closing connection");

    client.stop();

    HTTPClient http;

    // Send the webhook request to trigger the IFTTT applet

    http.begin(client, ifttt_url.c_str());

    int http_code = http.GET();

    if (http_code > 0) {

      Serial.printf("Webhook request sent. Response code: %d\n", http_code);

    } else {

      Serial.printf("Webhook request failed. Error code: %d\n", http_code);

    }

    // Clean up the HTTP client

    http.end();

  }

  delay(100);

}