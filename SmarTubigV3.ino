// SmarTubig V3 - Web-based
// now runs on web browsers instead of being limited to an app
// developed by GrenadeSpamr

#include <ESP8266WiFi.h>
#include <FS.h>
#include <DHT.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#define dhtPin 4
#define dhtType DHT11

const char* ssid = "SmarTubig ni Topher";
const char* pass = "smartubig";

float temperature;
float humidity;
float humidex;

DHT dht(dhtPin, dhtType);

String readTemperatureDHT() {
  if(isnan(temperature)) {
    Serial.println("DHT returned NaN for temperature, check sensor!");
    return "--";
  }
  else {
    Serial.println(temperature);
    return String(temperature);
  }
}

String readHumidityDHT() {
  if(isnan(humidity)) {
    Serial.println("DHT returned NaN for humidity, check sensor!");
    return "--";
  }
  else {
    Serial.println(humidity);
    return String(humidity);
  }
}

String readHumidexDHT() {
  if(isnan(humidex)) {
    Serial.println("DHT returned NaN for humidex, check sensor!");
    return "--";
  }
  else {
    Serial.println(humidex);
    return String(humidex);
  }
}

String statusHumidex() {
  if(humidex <= 29) {
    Serial.println("Normal");
    return "Your environment is normal.";
  }
  else if (humidex >= 31) {
    Serial.println("Warm");
    return "Your environment may feel warm, drink water every two hours.";
  }
  else if (humidex >= 35) {
    Serial.println("Hot");
    return "Your environment is a bit hot, drink water every hour.";
  }
  else if (humidex >= 40) {
    Serial.println("HotSpicy");
    return "Your environment is hotter than usual, drink water every 30 minutes.";
  }
}

String processor(const String& var){
  Serial.println(var);
  if(var == "TEMPERATURE") {
    return readTemperatureDHT();
  }
  else if(var == "HUMIDITY") {
    return readHumidityDHT();
  }
  else if(var == "HUMIDEX") {
    return readHumidexDHT();
  }
  else if(var == "STATUS") {
    return statusHumidex();
  }
  return String();
}

AsyncWebServer server(80);

void setup() {
  delay(1000);
  Serial.begin(9600);

  dht.begin();

  if(SPIFFS.begin() != true) {
    Serial.println("Error initialising SPIFFS");
    return;
  }

  WiFi.softAP(ssid, pass);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/bulma.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/bulma.min.css", "text/css");
  });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.js", "application/javascript");
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readTemperatureDHT().c_str());
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readHumidityDHT().c_str());
  });

  server.on("/humidex", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readHumidexDHT().c_str());
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", statusHumidex().c_str());
  });

  server.begin();
  Serial.print("Initialised HTTP server at: ");
  Serial.println(WiFi.softAPIP()); 
}

void loop() {
  delay(2000);

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  humidex = dht.computeHeatIndex(temperature, humidity, false);
  
  Serial.println(temperature);
  Serial.println(humidity);
  Serial.println(humidex);
}
