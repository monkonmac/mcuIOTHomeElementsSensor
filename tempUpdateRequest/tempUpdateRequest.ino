#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Adafruit_ADS1X15.h>

#define DHTTYPE DHT11
#define DHT11_PIN 2
DHT_Unified dht(DHT11_PIN, DHTTYPE);

Adafruit_ADS1115 ads(0x48);


const char* ssid = "ssid";   // Enter SSID here
const char* password = "password"; // Enter Password here
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
String endpoint = "http://192.168.1.37:3000/update-temp-humidity-rain"; //Replace with your endpoint
void setup() {
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());
  ads.begin();
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      sensors_event_t event;
      dht.temperature().getEvent(&event);
      float temp = event.temperature;
      if (isnan(event.temperature)) {
        Serial.println(F("Error reading temperature!"));
      } else {
        Serial.print(F("Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
      }
      // Get humidity event and print its value.
      dht.humidity().getEvent(&event);
      float humidity = event.relative_humidity;
      if (isnan(event.relative_humidity)) {
        Serial.println(F("Error reading humidity!"));
      } else {
        Serial.print(F("Humidity: "));
        Serial.print(event.relative_humidity);
        Serial.println(F("%"));
      }

      int16_t rainSensorValue = ads.readADC_SingleEnded(0);
      Serial.print("AIN0: ");
      Serial.println(adc0);




      WiFiClient client;
      HTTPClient http;
      http.begin(client, endpoint);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "temperature=" + String(temp) + "&humidity=" + String(humidity) + "&rainSensorValue=" + String(rainSensorValue);
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);

      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // Free resources
      http.end();
    }
    lastTime = millis();
  }
}
