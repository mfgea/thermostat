#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#if defined(ESP8266)
  #include "credentials.h"

  //#include <ESP8266WiFi.h>
  //#include <WiFiClient.h>
  #include <ESP8266WebServer.h>

  //#define CAYENNE_DEBUG         // Uncomment to show debug messages
  #define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
  //#include <CayenneWiFi.h>
  #include <CayenneDefines.h>
  #include <BlynkSimpleEsp8266.h>
  #include <CayenneWiFiClient.h>
#endif

#include <rotary.h>

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include "glyphs.h"
#include "screensaver_toasters_bitmaps.h" // Toaster graphics data is in this header file

#define DEBUG true

#define N_FLYERS   5 // Number of flying things
struct Flyer {       // Array of flying things
  int16_t x, y;      // Top-left position * 16 (for subpixel pos updates)
  int8_t  depth;     // Stacking order is also speed, 12-24 subpixels/frame
  uint8_t frame;     // Animation frame; Toasters cycle 0-3, Toast=255
} flyer[N_FLYERS];

#define OLED_I2C_ADDRESS 0x3C
#define TEMP_SENSOR 4
#define ROTARY_A 2
#define ROTARY_B 3
#define SWITCH 8

#define UNUSED_PIN -1

#define ON true
#define OFF false

#if defined(ESP8266)
  char token[] = CAYENNE_TOKEN;
  char ssid[] = WIFI_SSID;
  char password[] = WIFI_PASSWORD;

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

#define temperatureLowerLimit 10
#define temperatureHigherLimit 30

OneWire oneWirePin(TEMP_SENSOR);
DallasTemperature sensors(&oneWirePin);

#if !defined(ESP8266)
  void pciSetup(byte pin) {
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
  }
#endif

void debug(String str){
#if DEBUG
  Serial.println(str);
#endif
}

void setup()   {
 #if DEBUG
  Serial.begin(9600);
 #endif

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

  screensaverSetup();
#if OWSENSOR
  sensors.begin();
#endif

  // init done
  debug("\ninit");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.clearDisplay();
  display.drawXBitmap(0, 0, batman_bits, batman_width, batman_height, WHITE);
  display.display();
  delay(200);

  idleStartTime = millis();

#if defined(ESP8266)
  setupWebServer();
#endif
}

#if defined(ESP8266)
/*
void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            debug("WiFi connected");
            debug("IP address: ");
            debug((String)WiFi.localIP());
            enableServer = true;
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            debug("WiFi lost connection");
            enableServer = false;
            break;
    }
}
*/
void setupWebServer() {
  //WiFi.disconnect(true);

  //delay(1000);

  Cayenne.begin(token, ssid, password);

  //WiFi.onEvent(WiFiEvent);
  //WiFi.begin(ssid, password);

  server.on ( "/", sendStatus);
  server.on ( "/set", setData);
  server.begin();
}

void sendStatus() {
  char tmp[400];

  snprintf(tmp, 400, "{ \"temperature\": \"%f\", \"desiredTemperature\": \"%f\", \"switchState\": \"%d\" }", getTemperature(), desiredTemperature, switchState);
  server.send(200, "  application/json", tmp);
}

void setData() {
  if(server.arg("desiredTemperature")) {
    desiredTemperature = server.arg("desiredTemperature").toFloat();
  }
  if(server.arg("switchState")) {
    switchState = server.arg("switchState").toInt();
  }
  sendStatus();
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
  display.clearDisplay(); // start new buffer

  if(screensaverOn) {
    screensaverRender();
  } else {
    renderUI();
    t = millis();
    if((t - idleStartTime) >= 17000L) {
      turnScreensaver(ON);
    }
  }
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

void renderUI() {
  // text display tests
  display.setTextColor(WHITE);
  float temp = getTemperature();
  int tempInt = (int)temp;
  int tempDec = (int) (temp * 10) - tempInt * 10;
  draw_temp(temp, tempInt, tempDec);

  draw_desired_temp(desiredTemperature);
}

float getTemperature() {
  uint32_t t = millis();
  if(tempLastReadTime == 0 || (t - tempLastReadTime) >= 60000L) { // Read temp once every 1 min
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    tempLastReadTime = t;
  }
  return temperature;
}

void turnScreensaver(boolean status) {
  screensaverOn = status == ON;
}

void switchPressed() {// handle pin change interrupt for D8 to D13 here
  int button = digitalRead(SWITCH);
  if(button) {
    if(screensaverOn) {
      turnScreensaver(OFF);
    } else {
      switchState = !switchState;
    }
    idleStartTime = millis();
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
    idleStartTime = millis();
  }
}

#if !defined(ESP8266)
  ISR(PCINT0_vect){
    switchPressed();
  }
  ISR(PCINT2_vect){
    rotaryChanged();
  }
#endif

/* -------------------- */

void screensaverSetup(){
  randomSeed(analogRead(2));           // Seed random from unused analog input
  for(uint8_t i=0; i<N_FLYERS; i++) {  // Randomize initial flyer states
    flyer[i].x     = (-32 + random(160)) * 16;
    flyer[i].y     = (-32 + random( 96)) * 16;
    flyer[i].frame = random(3) ? random(4) : 255; // 66% toaster, else toast
    flyer[i].depth = 10 + random(16);             // Speed / stacking order
  }
  //qsort(flyer, N_FLYERS, sizeof(struct Flyer), compare); // Sort depths
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
    display.drawBitmap(x, y, (const uint8_t *)pgm_read_word(&mask[f]), 32, 32, BLACK);
    display.drawBitmap(x, y, (const uint8_t *)pgm_read_word(&img[ f]), 32, 32, WHITE);

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

