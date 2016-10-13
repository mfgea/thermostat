#include "values.h"

Values::Values() {
  temperature = 0;
  desiredTemperature = startDesiredTemperature;
}

float Values::getTemperature() {
  static uint8_t lastUpdate = 0;
  uint8_t t = millis();
  if(lastUpdate == 0 || (t - lastUpdate) >= 5000L) { // Read temp once every 1 min
    //sensors.requestTemperatures();
    //temperature = sensors.getTempCByIndex(0);
    temperature = random(20,30);
    lastUpdate = t;
  }
  return temperature;
}

float Values::getDesiredTemperature() {
  return desiredTemperature;
}
void Values::setDesiredTemperature(float temp) {
  desiredTemperature = temp;
}

bool Values::getSwitchState() {
  return switchState;
}
void Values::setSwitchState(bool state) {
  switchState = state;
}
void Values::toggleSwitchState() {
  switchState = !switchState;
}

int Values::getSignalStrength() {
  #ifdef ESP8266
  static uint8_t lastUpdate = 0;
  uint8_t t = millis();
  if(lastUpdate == 0 || (t - lastUpdate) >= 5000L) { // Read temp once every 1 min
    signalStrength = WiFi.RSSI();
    lastUpdate = t;
  }
  #endif
  return signalStrength;
}

