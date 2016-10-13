#define ON 1
#define OFF 0

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include "values.h"
#include "glyphs.h"
#include "wifi_icons.h"
#include "screensaver_toasters_bitmaps.h"

#define N_FLYERS   5 // Number of flying things
struct Flyer {       // Array of flying things
  int16_t x, y;      // Top-left position * 16 (for subpixel pos updates)
  int8_t  depth;     // Stacking order is also speed, 12-24 subpixels/frame
  uint8_t frame;     // Animation frame; Toasters cycle 0-3, Toast=255
} flyer[N_FLYERS];

class Display {
  public:
    Display();
    void init(Values data);
    void refresh();
    void touch();
    void loop(bool screensaver);
    void render();

    void turnScreensaver(bool status);
    bool isScreensaverPlaying();


  private:
    Values data;
    bool updateDisplay;
    Adafruit_SSD1306* display;
    bool screensaverStatus;

    void draw_wifi_icon();
    void draw_temp(float temp);
    void draw_desired_temp(float temp);
    void draw_thermometer(int temp);

    void screensaverSetup();
    void screensaverRender();
    int compare(Flyer a, Flyer b);
    void sort_flyers(Flyer a[], uint8_t al);
};

