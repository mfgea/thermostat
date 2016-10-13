#define DEBUG true
#define SCREENSAVER 0
#define CAYENNE 1

#define SERIAL_SPEED 74880

#define OLED_I2C_ADDRESS 0x3C
#define TEMP_SENSOR 4
#define ROTARY_A 2
#define ROTARY_B 3
#define SWITCH 8

#if DEBUG
  #define debugln(a) (Serial.println(a))
  #define debug(a) (Serial.print(a))
  #define debugBegin(a) (Serial.begin(a))
#else
  #define debugln(a)
  #define debug(a)
  #define debugBegin(a)
#endif

#define ON 1
#define OFF 0

#if CAYENNE
  #define TEMPERATURE_VPIN V0
  #define DESIRED_TEMPERATURE_VPIN V1
  #define SWITCH_VPIN V2

  #if DEBUG
    #define CAYENNE_DEBUG         // Uncomment to show debug messages
    #define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
  #endif
#endif
