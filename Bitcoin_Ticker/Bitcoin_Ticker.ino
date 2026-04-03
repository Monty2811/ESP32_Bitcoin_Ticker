#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <GxEPD2_3C.h>
#include <Adafruit_GFX.h>
#include <time.h>

#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

//Hier bitte die WLAN Daten eingeben
const char* ssid = "SSID";
const char* password = "Password";

// --- DISPLAY PINS ---
#define EPD_BUSY 21
#define EPD_CS   10
#define EPD_RST  6
#define EPD_DC   7
#define EPD_SCK  20
#define EPD_MOSI 5

GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(GxEPD2_290_C90c(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

String finalPrice = "0";
String priceDiff = "0.00";
String percentDiff = "0.00";
bool isPositive = true;
char timeString[10];

void getBitcoinData() {
  WiFiClientSecure client;
  client.setInsecure();
  if (client.connect("api.binance.com", 443)) {
    client.print("GET /api/v3/ticker/24hr?symbol=BTCEUR HTTP/1.1\r\nHost: api.binance.com\r\nConnection: close\r\n\r\n");
    unsigned long start = millis();
    while (client.available() == 0 && (millis() - start < 8000));
    String payload = client.readString();
    
    int pPos = payload.indexOf("\"lastPrice\":\"");
    if (pPos != -1) {
      String raw = payload.substring(pPos + 13);
      raw = raw.substring(0, raw.indexOf("."));
      if (raw.length() > 3) finalPrice = raw.substring(0, raw.length()-3) + "." + raw.substring(raw.length()-3);
      else finalPrice = raw;
    }

    int dPos = payload.indexOf("\"priceChange\":\"");
    if (dPos != -1) {
      float diff = payload.substring(dPos + 15).toFloat();
      priceDiff = String(abs(diff), 2);
    }

    int cPos = payload.indexOf("\"priceChangePercent\":\"");
    if (cPos != -1) {
      String percRaw = payload.substring(cPos + 22);
      isPositive = !percRaw.startsWith("-");
      percRaw.replace("-", "");
      percentDiff = percRaw.substring(0, percRaw.indexOf("\"")).substring(0, 4);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.begin(ssid, password);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 40) { delay(500); count++; }
  
  if (WiFi.status() == WL_CONNECTED) {
    configTime(0, 0, "pool.ntp.org");
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0", 1);
    tzset();
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)) strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);
    else sprintf(timeString, "--:--");
    getBitcoinData();
  }

  display.init(115200, true, 50, false);
  SPI.end();
  SPI.begin(EPD_SCK, -1, EPD_MOSI, EPD_CS);
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(15, 33);
  display.print("Bitcoin");
  display.setFont(&FreeSans9pt7b);
  display.setCursor(105, 30); 
  display.print("(24h)");
  display.setFont(); 
  display.setCursor(195, 10);
  display.print("Update: ");
  display.print(timeString);
  display.drawFastHLine(10, 42, 275, GxEPD_BLACK);

  uint16_t themeColor = isPositive ? GxEPD_BLACK : GxEPD_RED;
  display.setTextColor(themeColor);
  display.setFont(&FreeSansBold18pt7b);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(finalPrice, 0, 0, &x1, &y1, &w, &h);
  int startX = (296 - w - 50) / 2; 
  display.setCursor(startX, 82); 
  display.print(finalPrice);
  
  int euroX = display.getCursorX() + 15;
  int euroY = 82;
  display.setCursor(euroX, euroY);
  display.print("C"); 
  for(int i=0; i<3; i++) {
    display.drawFastHLine(euroX-5, euroY-11+i, 18, themeColor); 
    display.drawFastHLine(euroX-5, euroY-18+i, 18, themeColor);
  }

  display.setFont(&FreeSansBold9pt7b);
  String sign = isPositive ? "+" : "-";
  String diffValue = sign + priceDiff + " ";
  String percPart = " (" + sign + percentDiff + "%)";
  
  int16_t tx1, ty1; uint16_t tw, th;
  display.getTextBounds(diffValue + "C" + percPart, 0, 0, &tx1, &ty1, &tw, &th);
  int diffStartX = (296 - tw) / 2;
  
  display.setCursor(diffStartX, 112);
  display.print(diffValue);
  
  int sEuroX = display.getCursorX() + 2;
  int sEuroY = 112;
  display.print("C");
  display.drawFastHLine(sEuroX + 1, sEuroY - 4, 6, themeColor);
  display.drawFastHLine(sEuroX + 1, sEuroY - 7, 6, themeColor);
  
  display.setCursor(display.getCursorX() + 2, 112);
  display.print(percPart);

  int aX = 250; int aY = isPositive ? 75 : 95;
  if (isPositive) {
    display.fillTriangle(aX, aY, aX+18, aY-18, aX+5, aY, themeColor);
    display.fillTriangle(aX+18, aY-18, aX+5, aY, aX+23, aY-18, themeColor);
    display.fillTriangle(aX+10, aY-23, aX+28, aY-23, aX+28, aY-5, themeColor);
  } else {
    display.fillTriangle(aX, aY-18, aX+18, aY, aX+5, aY-18, themeColor);
    display.fillTriangle(aX+18, aY, aX+5, aY-18, aX+23, aY, themeColor);
    display.fillTriangle(aX+28, aY+5, aX+10, aY+5, aX+28, aY-13, themeColor);
  }
  
  display.display(false);
  display.hibernate();
  
  esp_sleep_enable_timer_wakeup(900ULL * 1000000ULL);
  esp_deep_sleep_start();
}

void loop() {}
