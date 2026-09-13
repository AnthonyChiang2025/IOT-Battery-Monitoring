#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <time.h>

#define BATTERY_PIN 34

// TFT
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// Device
const char* DEVICE_ID = "Work-Battery";

// WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// API
const char* serverUrl =
  "https://anthonystudio.net/api/battery.php";

// Malaysia GMT+8
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;

bool uploadedToday = false;
int lastDay = -1;

// -------------------------------------------------
void sendToServer(
  float voltage,
  int soc,
  int soh,
  float resistance)
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi offline");
    return;
  }

  HTTPClient http;

  http.begin(serverUrl);

  http.addHeader(
    "Content-Type",
    "application/json"
  );

  String payload = "{";
  payload += "\"device_id\":\"";
  payload += DEVICE_ID;
  payload += "\",";
  payload += "\"voltage\":";
  payload += String(voltage, 2);
  payload += ",";
  payload += "\"soc\":";
  payload += String(soc);
  payload += ",";
  payload += "\"soh\":";
  payload += String(soh);
  payload += ",";
  payload += "\"resistance\":";
  payload += String(resistance, 3);
  payload += "}";

  Serial.println("Sending JSON:");
  Serial.println(payload);

  int httpCode = http.POST(payload);

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println(response);
  }

  http.end();
}

// -------------------------------------------------
void drawScreen(
  float voltage,
  int soc,
  int soh,
  float resistance)
{
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextSize(2);
  tft.setCursor(10,10);
  tft.setTextColor(ILI9341_CYAN);
  tft.print("Battery Monitor");

  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(10,50);
  tft.print("ID: ");
  tft.print(DEVICE_ID);

  tft.setCursor(10,80);
  tft.print("V: ");
  tft.print(voltage,2);
  tft.print("V");

  tft.setCursor(10,110);
  tft.print("SOC: ");
  tft.print(soc);
  tft.print("%");

  tft.setCursor(10,140);
  tft.print("SOH: ");
  tft.print(soh);
  tft.print("%");

  tft.setCursor(10,170);
  tft.print("R: ");
  tft.print(resistance,3);
  tft.print(" Ohm");

  tft.setCursor(10,200);

  if(WiFi.status()==WL_CONNECTED){
    tft.setTextColor(ILI9341_GREEN);
    tft.print("WiFi OK");
  }
  else{
    tft.setTextColor(ILI9341_RED);
    tft.print("WiFi OFF");
  }

  int barWidth = map(soc, 0, 100, 0, 200);

  tft.drawRect(
    10,
    230,
    200,
    20,
    ILI9341_WHITE
  );

  uint16_t colour;

  if(soc > 50)
    colour = ILI9341_GREEN;
  else if(soc > 20)
    colour = ILI9341_YELLOW;
  else
    colour = ILI9341_RED;

  tft.fillRect(
    10,
    230,
    barWidth,
    20,
    colour
  );
}

// -------------------------------------------------
void setup()
{
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1);

  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(10,50);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.println("Connecting WiFi");

  Serial.print("Connecting WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  configTime(
    gmtOffset_sec,
    daylightOffset_sec,
    ntpServer
  );
}

// -------------------------------------------------
void loop()
{
  int adc = analogRead(BATTERY_PIN);

  // Simulate 11.5V - 12.7V battery
  float batteryVoltage =
      11.5 +
      ((float)adc / 4095.0) *
      (12.7 - 11.5);

  int soc =
      ((batteryVoltage - 11.5) /
      (12.7 - 11.5)) * 100;

  soc = constrain(soc, 0, 100);

  // simulated
  int soh = 98;
  float resistance = 0.050;

  Serial.print("Voltage=");
  Serial.print(batteryVoltage,2);

  Serial.print(" SOC=");
  Serial.print(soc);

  Serial.print("% SOH=");
  Serial.print(soh);

  Serial.print("% R=");
  Serial.println(resistance);

  drawScreen(
      batteryVoltage,
      soc,
      soh,
      resistance
  );

  struct tm timeinfo;

  if (getLocalTime(&timeinfo))
  {
      int currentHour =
          timeinfo.tm_hour;

      int currentMinute =
          timeinfo.tm_min;

      int currentDay =
          timeinfo.tm_mday;

      if(currentDay != lastDay)
      {
          uploadedToday = false;
          lastDay = currentDay;
      }

      if(
          !uploadedToday &&
          currentHour >= 12
      )
      {
          sendToServer(
              batteryVoltage,
              soc,
              soh,
              resistance
          );

          uploadedToday = true;

          Serial.println(
            "Daily upload completed"
          );
      }
  }

  delay(1000);
}