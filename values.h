#define startDesiredTemperature 21.0

#define temperatureLowerLimit 10
#define temperatureHigherLimit 30

#ifndef _VALUES_H_
#define _VALUES_H_

class Values {
  public:
    Values();
    float getTemperature();
    float getDesiredTemperature();
    void setDesiredTemperature(float temp);
    bool getSwitchState();
    void setSwitchState(bool state);
    void toggleSwitchState();
    int getSignalStrength();

  protected:
    float temperature;
    float desiredTemperature;
    bool switchState;
    int signalStrength;
};

#endif
