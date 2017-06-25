#include <OneWire.h>
#include <DallasTemperature.h>
#include <rotary.h>

#include "config.h"
#include "display.h"
#include "values.h"

#ifdef ESP8266
  #include "credentials.h"

  #if CAYENNE
    #include <CayenneDefines.h>
    #include <BlynkSimpleEsp8266.h>
    #include <CayenneWiFiClient.h>
  #else
    #include <ESP8266WiFi.h>
    #include <WiFiClient.h>
  #endif

  #include <ESP8266WebServer.h>

  char token[] = CAYENNE_TOKEN;
  char ssid[] = WIFI_SSID;
  char password[] = WIFI_PASSWORD;

  WiFiEventHandler gotIpEventHandler, disconnectedEventHandler, connectedEventHandler;

  ESP8266WebServer server(80);
#endif

Display display;
Rotary r = Rotary(ROTARY_A, ROTARY_B);
bool isIdle = false;
uint32_t idleStartTime;
uint8_t enableServer = false;

OneWire oneWirePin(TEMP_SENSOR);
DallasTemperature sensors(&oneWirePin);
Values data;

void setup()   {
  debugBegin(SERIAL_SPEED);
  debugln("\ninit");
  randomSeed(analogRead(2));

  sensors.begin();

  #ifdef ESP8266
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

  display.init(data);
  delay(200);

  #ifdef ESP8266
  setupWebServer();
  #endif

  idleStartTime = millis();
}

void loop() {
  uint32_t t;

  #ifdef ESP8266
  if(enableServer) {
    server.handleClient();
    #if CAYENNE
      Cayenne.run();
    #endif
  }
  #endif

  display.loop(isIdle);

  if(!isIdle) {
    t = millis();
    if((t - idleStartTime) >= 17000L) {
      isIdle = true;
    }
  }
}

void touch() {
  idleStartTime = millis();
  isIdle = false;
  display.touch();
}

void switchPressed() {// handle pin change interrupt for D8 to D13 here
  int button = digitalRead(SWITCH);
  if(button == LOW) {
    if(!isIdle) {
      data.toggleSwitchState();
    }
    touch();
  }
}

void rotaryChanged() {
  unsigned char result = r.process();
  float desiredTemp;
  if (result) {
    if(!isIdle) {
      desiredTemp = data.getDesiredTemperature();
      desiredTemp = (result == DIR_CW ? desiredTemp+.5 : desiredTemp-.5);
      if(desiredTemp < temperatureLowerLimit) {
        desiredTemp = temperatureLowerLimit;
      }
      if(desiredTemp > temperatureHigherLimit) {
        desiredTemp = temperatureHigherLimit;
      }
      data.setDesiredTemperature(desiredTemp);
    }
    touch();
  }
}


#ifdef ESP8266
//----- Http service functions -----//

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
  ftoa(temp, data.getTemperature(), 2);
  
  char desiredTemp[10];
  ftoa(desiredTemp, data.getDesiredTemperature(), 2);

  snprintf(tmp, 400, "{ \"temperature\": \"%s\", \"desiredTemperature\": \"%s\", \"switchState\": \"%d\" }", temp, desiredTemp, data.getSwitchState());
  debugln(tmp);
  server.send(200, "  application/json", tmp);
}

void setData() {
  debugln("setting vars...");
  if(server.arg("desiredTemperature")) {
    debug("setting desiredTemp: ");
    data.setDesiredTemperature(server.arg("desiredTemperature").toFloat());
    debugln(data.getDesiredTemperature());
  }
  if(server.arg("switchState")) {
    debug("setting switchStatus: ");
    data.setSwitchState(server.arg("switchState").toInt()?1:0);
    debugln(data.getSwitchState());
  }
  sendStatus();
  display.touch();
}
#endif

#if CAYENNE
CAYENNE_OUT(DESIRED_TEMPERATURE_VPIN) {
  Cayenne.celsiusWrite(DESIRED_TEMPERATURE_VPIN, 21.0); //data.getDesiredTemperature());
}

CAYENNE_OUT(SWITCH_VPIN) {
  Cayenne.virtualWrite(SWITCH_VPIN, data.getSwitchState());
}

CAYENNE_OUT(TEMPERATURE_VPIN) {
  Cayenne.celsiusWrite(TEMPERATURE_VPIN, 21.0); //data.getTemperature());
}
#endif

#ifndef ESP8266
void pciSetup(byte pin) {
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}
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
