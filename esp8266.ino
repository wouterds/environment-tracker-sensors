#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include <TSL2561.h>
#include <Adafruit_BME280.h>
#include <Adafruit_HTU21DF.h>

// Configuration
#define WIFI_SSID ""
#define WIFI_PASS ""
#define WEB_SERVER_PORT 80
#define I2C_ADDR_TSL2561 0x39
#define I2C_ADDR_BME280 0x76
#define RGB_LED_RED_PIN D7
#define RGB_LED_GREEN_PIN D6
#define RGB_LED_BLUE_PIN D5

// Web server
ESP8266WebServer webServer(WEB_SERVER_PORT);

// Sensors
TSL2561 tsl2561(I2C_ADDR_TSL2561);
Adafruit_BME280 bme280;
Adafruit_HTU21DF htu21df;

void setup()
{
  setupI2C();
  setupSerial();
  setupWiFi();
  setupWebServer();
  setupSensors();
  setupLed();
}

void loop()
{
  webServer.handleClient();
  checkLed();
  delay(25);
}

void handle404()
{
  webServer.send(404, "text/plain", "404 Not Found");
  flashLed();
}

void handlePressure()
{
  float pressure = bme280.readPressure() / 100;

  webServer.send(200, "text/plain", String(pressure));
  flashLed();
}

void handleHumidity()
{
  float humidity = readHumidity();

  webServer.send(200, "text/plain", String(humidity));
  flashLed();
}

void handleTemperature()
{
  float temperature = readTemperature();

  webServer.send(200, "text/plain", String(temperature));
  flashLed();
}

void handleIlluminanceIr()
{
  unsigned int illuminance = tsl2561.getLuminosity(TSL2561_INFRARED);

  webServer.send(200, "text/plain", String(illuminance));
  flashLed();
}

void handleIlluminanceVisible()
{
  unsigned int illuminance = tsl2561.getLuminosity(TSL2561_VISIBLE);

  webServer.send(200, "text/plain", String(illuminance));
  flashLed();
}

void handleIlluminanceFull()
{
  unsigned int illuminance = tsl2561.getLuminosity(TSL2561_FULLSPECTRUM);

  webServer.send(200, "text/plain", String(illuminance));
  flashLed();
}

bool isLedEnabled = true;
void handleLed()
{
  if (webServer.hasArg("enabled")) {
    isLedEnabled = webServer.arg("enabled").equals("1");
  }

  webServer.send(204);
  flashLed();
}

void handleRoot()
{
  Serial.println("[WebServer] Request: /");

  // Read sensor values
  unsigned int visible = tsl2561.getLuminosity(TSL2561_VISIBLE);
  unsigned int full = tsl2561.getLuminosity(TSL2561_FULLSPECTRUM);
  unsigned int ir = tsl2561.getLuminosity(TSL2561_INFRARED);
  float temperature = readTemperature();
  float humidity = readHumidity();
  float pressure = bme280.readPressure() / 100;

  String response = "";
  response += "{";
  response += "\"illuminance\":{";
  response += "\"visible\":";
  response += visible;
  response += ",\"full\":";
  response += full;
  response += ",\"ir\":";
  response += ir;
  response += "}";
  response += ",\"temperature\":";
  response += temperature;
  response += ",\"humidity\":";
  response += humidity;
  response += ",\"pressure\":";
  response += pressure;
  response += "}";

  webServer.send(200, "application/json", response);
  flashLed();
}

float readHumidity()
{
  return (bme280.readHumidity() + htu21df.readHumidity()) / 2;
}

float readTemperature()
{
  return (bme280.readTemperature() + htu21df.readTemperature()) / 2;
}

void setupI2C()
{
  Wire.begin(D2, D1);
}

void setupSerial()
{
  Serial.begin(9600);
  Serial.println();
}

void setupWiFi()
{
  Serial.println("[WiFi] Setup");
  Serial.print("[WiFi] Connecting to: ");
  Serial.print(WIFI_SSID);
  Serial.print("..");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("[WiFi] Connected!");
  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());
}

void setupWebServer()
{
  Serial.println("[WebServer] Setup");
  webServer.on("/", HTTP_GET, handleRoot);
  webServer.on("/illuminance/full", HTTP_GET, handleIlluminanceFull);
  webServer.on("/illuminance/visible", HTTP_GET, handleIlluminanceVisible);
  webServer.on("/illuminance/ir", HTTP_GET, handleIlluminanceIr);
  webServer.on("/temperature", HTTP_GET, handleTemperature);
  webServer.on("/humidity", HTTP_GET, handleHumidity);
  webServer.on("/pressure", HTTP_GET, handlePressure);
  webServer.on("/led", HTTP_POST, handleLed);
  webServer.onNotFound(handle404);

  Serial.println("[WebServer] Starting..");
  webServer.begin();
  Serial.println("[WebServer] Running!");
}

void setupLed()
{
  pinMode(RGB_LED_RED_PIN, OUTPUT);
  pinMode(RGB_LED_GREEN_PIN, OUTPUT);
  pinMode(RGB_LED_BLUE_PIN, OUTPUT);
  setLedColor(1023, 1023, 1023);
}

void setupSensors()
{
  setupSensorTSL2561();
  setupSensorBME280();
  setupSensorHTU21DF();
}

void setupSensorTSL2561()
{
  Serial.println("[TSL2561] Setup");
  Serial.print("[TSL2561] Connecting..");

  while (!tsl2561.begin())
  {
    delay(50);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("[TSL2561] Connected!");
}

void setupSensorBME280()
{
  Serial.println("[BME280] Setup");
  Serial.print("[BME280] Connecting..");

  while (!bme280.begin(I2C_ADDR_BME280))
  {
    delay(50);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("[BME280] Connected!");
}

void setupSensorHTU21DF()
{
  Serial.println("[HTU21DF] Setup");
  Serial.print("[HTU21DF] Connecting..");

  while (!htu21df.begin())
  {
    delay(50);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("[HTU21DF] Connected!");
}

void checkLed()
{
  // Enabled, do nothing
  if (isLedEnabled) {
    return;
  }

  // Turn led off
  setLedColor(0, 0, 0);
}

void flashLed()
{
  // Led disabled, stop here
  if (!isLedEnabled) {
    return;
  }

  int rgbColor[3] = { 1023, 0, 0 };

  // Choose the colors to increment and decrement
  for (int decColour = 0; decColour < 3; decColour += 1) {
    int incColour = decColour == 2 ? 0 : decColour + 1;

    // Cross-fade colors
    for(int i = 0; i < 1023; i += 1) {
      rgbColor[decColour] -= 1;
      rgbColor[incColour] += 1;

      setLedColor(rgbColor[0], rgbColor[1], rgbColor[2]);
      delayMicroseconds(250); // 0.25ms
    }
  }
}

void setLedColor(unsigned int red, unsigned int green, unsigned int blue)
{
  analogWrite(RGB_LED_RED_PIN, 1023 - red);
  analogWrite(RGB_LED_GREEN_PIN, 1023 - green);
  analogWrite(RGB_LED_BLUE_PIN, 1023 - blue);
}
