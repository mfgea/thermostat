#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DEBUG true
#define SCREENSAVER 0
#define CAYENNE 1

#define temperatureLowerLimit 10
#define temperatureHigherLimit 30

#define OLED_I2C_ADDRESS 0x3C
#define TEMP_SENSOR 4
#define ROTARY_A 2
#define ROTARY_B 3
#define SWITCH 8


#if defined(ESP8266)
  #include "credentials.h"

  #if CAYENNE
    #define TEMPERATURE_VPIN V0
    #define DESIRED_TEMPERATURE_VPIN V1
    #define SWITCH_VPIN V2
  
    #if DEBUG
      #define CAYENNE_DEBUG         // Uncomment to show debug messages
      #define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
    #endif
    //#include <CayenneWiFi.h>
    #include <CayenneDefines.h>
    #include <BlynkSimpleEsp8266.h>
    #include <CayenneWiFiClient.h>
  #else
    #include <ESP8266WiFi.h>
    #include <WiFiClient.h>
  #endif

  #include <ESP8266WebServer.h>
#endif

#include <rotary.h>

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include "glyphs.h"
#include "wifi_icons.h"
#include "screensaver_toasters_bitmaps.h" // Toaster graphics data is in this header file

#if DEBUG
  #define debugln(a) (Serial.println(a))
  #define debug(a) (Serial.print(a))
  #define debugBegin(a) (Serial.begin(a))
#else
  #define debugln(a)
  #define debug(a)
  #define debugBegin(a)
#endif

#define N_FLYERS   5 // Number of flying things
struct Flyer {       // Array of flying things
  int16_t x, y;      // Top-left position * 16 (for subpixel pos updates)
  int8_t  depth;     // Stacking order is also speed, 12-24 subpixels/frame
  uint8_t frame;     // Animation frame; Toasters cycle 0-3, Toast=255
} flyer[N_FLYERS];

#define UNUSED_PIN -1

#define ON 1
#define OFF 0

#if defined(ESP8266)
  char token[] = CAYENNE_TOKEN;
  char ssid[] = WIFI_SSID;
  char password[] = WIFI_PASSWORD;

  WiFiEventHandler gotIpEventHandler, disconnectedEventHandler, connectedEventHandler;

  ESP8266WebServer server(80);
#endif

Adafruit_SSD1306 display(UNUSED_PIN);
Rotary r = Rotary(ROTARY_A, ROTARY_B);
uint8_t switchState = ON;
uint8_t screensaverOn = false;
uint32_t idleStartTime;
uint32_t tempLastReadTime = 0;

uint8_t enableServer = false;

float temperature = 0;
float desiredTemperature = 21.0;

bool updateDisplay = true;

OneWire oneWirePin(TEMP_SENSOR);
DallasTemperature sensors(&oneWirePin);

#if !defined(ESP8266)
  void pciSetup(byte pin) {
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
  }
#endif

void setup()   {
  debugBegin(74880);
  debugln("\ninit");

  #if defined(ESP8266)
    attachInterrupt(digitalPinToInterrupt(ROTARY_A), rotaryChanged, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ROTARY_B), rotaryChanged, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWITCH), switchPressed, CHANGE);
  #else
  // Program interrupts for Rotary Encoder and switch
    pciSetup(ROTARY_A);
    pciSetup(ROTARY_B);
    pciSetup(SWITCH);
  #endif

  digitalWrite(SWITCH, HIGH); // Set switch pint to PULLUP mode. pinMode(4, INPUT); is default

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);  // initialize with the I2C addr 0x3D (for the 128x64)

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.clearDisplay();
  display.drawXBitmap(0, 0, batman_bits, batman_width, batman_height, WHITE);
  display.display();
  delay(200);

  screensaverSetup();
#if OWSENSOR
  sensors.begin();
#endif

  idleStartTime = millis();

#if defined(ESP8266)
  setupWebServer();
#endif

  randomSeed(analogRead(0));
}

#if defined(ESP8266)

void setupWebServer() {
  debugln("Using credentials:");
  debug("Cayenne token: "); debugln(token);
  debug("Wifi SSID: "); debugln(ssid);
  debug("Wifi Password: "); debugln(password);
  debug("Cayenne?: "); debugln(CAYENNE);

  connectedEventHandler = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected& event) {
    debugln("Station connected");
  });

  disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event) {
    debugln("Station disconnected");
    enableServer = false;
  });

  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event) {
    debug("Wifi connected, IP: ");
    debugln(WiFi.localIP());
    enableServer = true;
  });
  
  #if CAYENNE
  Cayenne.begin(token, ssid, password);
  #else
  debugln("Disconnecting...");
  WiFi.disconnect(true);

  delay(1000);
  debugln("OK. Reconnecting...");

  WiFi.begin(ssid, password);
  #endif

  server.on("/", sendStatus);
  server.on("/set", setData);
  server.begin();
}

void sendStatus() {
  char tmp[400];

  debugln("sending status...");

  char temp[10];
  ftoa(temp, getTemperature(), 2);
  
  char desiredTemp[10];
  ftoa(desiredTemp, desiredTemperature, 2);

  snprintf(tmp, 400, "{ \"temperature\": \"%s\", \"desiredTemperature\": \"%s\", \"switchState\": \"%d\" }", temp, desiredTemp, switchState);
  debugln(tmp);
  server.send(200, "  application/json", tmp);
}

void setData() {
  debugln("setting vars...");
  if(server.arg("desiredTemperature")) {
    debug("setting desiredTemp: ");
    desiredTemperature = server.arg("desiredTemperature").toFloat();
    debugln(desiredTemperature);
  }
  if(server.arg("switchState")) {
    debug("setting switchStatus: ");
    switchState = server.arg("switchState").toInt();
    debugln(switchState);
  }
  sendStatus();
  updateDisplay = true;
}
#endif

void loop() {
  uint32_t t;

#if defined(ESP8266)
  if(enableServer) {
   server.handleClient();
  }
#endif

  // Update screen to show the current buffer
  display.display();

  if(SCREENSAVER && screensaverOn) {
    display.clearDisplay(); // start new buffer
    screensaverRender();
  } else {
    renderUI();
    t = millis();
    if((t - idleStartTime) >= 17000L) {
      turnScreensaver(ON);
    }
  }

  #if CAYENNE
  Cayenne.run();
  #endif
}

void draw_temp(float temp, int tempInt, int tempDec) {
    //-- Draw the thermometer filled
    display.setTextColor(WHITE);
    display.drawCircle(10, 6, 4, WHITE);
    display.drawRect(6, 6, 9, 46, WHITE);
    display.drawCircle(10, 56, 6, WHITE);
    //-- Remove the contents
    display.fillCircle(10, 6, 3, BLACK);
    display.fillRect(7, 6, 7, 46, BLACK);
    display.fillCircle(10, 56, 5, BLACK);

    //-- Draw the graduation
    display.drawFastHLine(17, 10, 4,WHITE);
    display.drawFastHLine(17, 20, 4,WHITE);
    display.drawFastHLine(17, 30, 4,WHITE);
    display.drawFastHLine(17, 40, 4,WHITE);
    display.drawFastHLine(17, 50, 4,WHITE);
    //-- Draw the contents
    display.fillCircle(10, 56, 4, WHITE);

    //-- Draw the temperature dependant part
    int y = 40 - tempInt;
    int h = 15 + tempInt;
    display.fillRect(9,y,3,h,WHITE);

    char buf[5];
    //-- Write the temperature
    display.setFont(&FreeSansBold18pt7b);
    int16_t  x1, y1;
    uint16_t w1, h1;
    int16_t top = 26;
    int16_t left = 32;
    itoa(tempInt, buf, 10);
    display.getTextBounds(buf, top, left, &x1, &y1, &w1, &h1);
    display.fillRect(x1,y1,w1,h1,BLACK);
    display.setCursor(left, top);
    display.print(buf);

    left = x1 + w1 + 6;
    top = y1 + h1 - 7;

    char str[10];
    itoa(tempDec, buf, 10);
    strcpy(str, ".");
    strcat(str, buf);

    display.setFont(&FreeSansBold9pt7b);
    display.getTextBounds(str, left, top, &x1, &y1, &w1, &h1);
    display.fillRect(x1,y1,w1,h1,BLACK);
    display.setCursor(left,top);
    display.print(str);
}

void draw_desired_temp(float temp) {
    char buf[5];
    char str[20];

    int tempInt = (int) temp;
    int tempDec = (int) (temp * 10) - tempInt * 10;

    strcpy(str, "");
    itoa(tempInt, buf, 10);
    strcat(str, buf);
    strcat(str, ".");
    itoa(tempDec, buf, 10);
    strcat(str, buf);

    int16_t  x1, y1;
    uint16_t w1, h1;
    display.setFont(&FreeSans9pt7b);
    display.getTextBounds(str, 0, 0, &x1, &y1, &w1, &h1);

    uint16_t left = 128 - w1;
    uint16_t top = 64 - h1;
    display.fillRect(left, top, w1, h1, BLACK);
    display.setCursor(left-10, 58);
    display.print(str);
}

void drawWifiIcon() {
  int strength;
  // render a wifi icon, either connected or not
  if(enableServer) {
    float raw = getSignalStrength();
    strength = (int) (2 * (100 + raw));
    strength = constrain(strength, 0, 100);
    debug("Raw Signal Strength: "); debug(raw); debugln("dbm");
  } else {
    strength = 0;
  }
  
  int index = ceil(strength / 25);
  debug("Signal Strength: "); debug(strength); debugln("%");
  debug("Icon index: "); debugln(index);
  int x = 128 - wifi_width;
  int y = 0;
  display.fillRect(x, y, wifi_width, wifi_height, BLACK);
  display.drawXBitmap(x, y, wifi_icon[index], wifi_width, wifi_height, WHITE);
}

void renderUI() {
  static uint32_t lastUpdate = 0;
  uint32_t t = millis();
  if (updateDisplay || t - lastUpdate > 5000L) {
    debugln("UPDATE");
    display.clearDisplay(); // start new buffer
    // text display tests
    display.setTextColor(WHITE);
    float temp = getTemperature();
    int tempInt = (int)temp;
    int tempDec = (int) (temp * 10) - tempInt * 10;
    draw_temp(temp, tempInt, tempDec);

    draw_desired_temp(desiredTemperature);

    #if defined(ESP8266)
    drawWifiIcon();
    #endif
    updateDisplay = false;
    lastUpdate = t;
  }
}

float getSignalStrength() {
  return WiFi.RSSI();
}

float getTemperature() {
  uint32_t t = millis();
  if(tempLastReadTime == 0 || (t - tempLastReadTime) >= 5000L) { // Read temp once every 1 min
    //sensors.requestTemperatures();
    //temperature = sensors.getTempCByIndex(0);
    temperature = random(20,30);
    #if CAYENNE
    Cayenne.celsiusWrite(TEMPERATURE_VPIN, temperature);
    #endif

    tempLastReadTime = t;
  }
  return temperature;
}

void touch() {
  idleStartTime = millis();
  updateDisplay = true;
}

void turnScreensaver(boolean status) {
  screensaverOn = status == ON;
}

void switchPressed() {// handle pin change interrupt for D8 to D13 here
  int button = digitalRead(SWITCH);
  if(button == LOW) {
    if(screensaverOn) {
      turnScreensaver(OFF);
    } else {
      switchState = !switchState;
    }
    touch();
  }
}

void rotaryChanged() {
  unsigned char result = r.process();
  if (result) {
    if(screensaverOn) {
      turnScreensaver(OFF);
    } else {
      desiredTemperature = (result == DIR_CW ? desiredTemperature+.5 : desiredTemperature-.5);
      if(desiredTemperature < temperatureLowerLimit) {
        desiredTemperature = temperatureLowerLimit;
      }
      if(desiredTemperature > temperatureHigherLimit) {
        desiredTemperature = temperatureHigherLimit;
      }
    }
    touch();
  }
}

#if CAYENNE
CAYENNE_OUT(DESIRED_TEMPERATURE_VPIN) {
  Cayenne.celsiusWrite(DESIRED_TEMPERATURE_VPIN, 21.0);
}

CAYENNE_OUT(SWITCH_VPIN) {
  Cayenne.virtualWrite(SWITCH_VPIN, switchState);
}
#endif

#if !defined(ESP8266)
  ISR(PCINT0_vect){
    switchPressed();
  }
  ISR(PCINT2_vect){
    rotaryChanged();
  }
#endif

char* ftoa(char *a, double f, int precision) {
  char *ret = a;
  long integer = (long)f;
  itoa(integer, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  int mult = 10 ^ precision;
  if (precision == 0){
    mult = 0;
  }
  long decimal = abs((long)((f - integer) * mult));
  itoa(decimal, a, 10);
  return ret;
}

/* -------------------- */

void screensaverSetup(){
  randomSeed(analogRead(2));           // Seed random from unused analog input
  for(uint8_t i=0; i<N_FLYERS; i++) {  // Randomize initial flyer states
    flyer[i].x     = (-32 + random(160)) * 16;
    flyer[i].y     = (-32 + random( 96)) * 16;
    flyer[i].frame = random(3) ? random(4) : 255; // 66% toaster, else toast
    flyer[i].depth = 10 + random(16);             // Speed / stacking order
  }
  sort_flyers(flyer, N_FLYERS);
}

void screensaverRender() {
  uint8_t i, f;
  int16_t x, y;
  boolean resort = false;     // By default, don't re-sort depths

  for(i=0; i<N_FLYERS; i++) { // For each flyer...

    // First draw each item...
    f = (flyer[i].frame == 255) ? 4 : (flyer[i].frame++ & 3); // Frame #
    x = flyer[i].x / 16;
    y = flyer[i].y / 16;
    #if defined(ESP8266)
    display.drawBitmap(x, y, (const uint8_t *)pgm_read_dword(&mask[f]), 32, 32, BLACK);
    display.drawBitmap(x, y, (const uint8_t *)pgm_read_dword(&img[f]), 32, 32, WHITE);
    #else
    display.drawBitmap(x, y, (const uint8_t *)pgm_read_word(&mask[f]), 32, 32, BLACK);
    display.drawBitmap(x, y, (const uint8_t *)pgm_read_word(&img[f]), 32, 32, WHITE);
    #endif

    // Then update position, checking if item moved off screen...
    flyer[i].x -= flyer[i].depth * 2; // Update position based on depth,
    flyer[i].y += flyer[i].depth;     // for a sort of pseudo-parallax effect.
    if((flyer[i].y >= (64*16)) || (flyer[i].x <= (-32*16))) { // Off screen?
      if(random(7) < 5) {         // Pick random edge; 0-4 = top
        flyer[i].x = random(160) * 16;
        flyer[i].y = -32         * 16;
      } else {                    // 5-6 = right
        flyer[i].x = 128         * 16;
        flyer[i].y = random(64)  * 16;
      }
      flyer[i].frame = random(3) ? random(4) : 255; // 66% toaster, else toast
      flyer[i].depth = 10 + random(16);
      resort = true;
    }
  }
  // If any items were 'rebooted' to new position, re-sort all depths
  if(resort) sort_flyers(flyer, N_FLYERS);
}

void sort_flyers(Flyer a[], uint8_t al) {
  uint16_t i, j;
  for (i = 1; i < al; i++) {
    Flyer tmp = a[i];
    for (j = i; j >= 1 && compare(tmp, a[j-1]) < 0; j--)
      a[j] = a[j-1];
    a[j] = tmp;
  }
}

// Flyer depth comparison function for qsort()
static int compare(Flyer a, Flyer b) {
  return a.depth - b.depth;
}

