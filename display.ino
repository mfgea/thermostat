Display::Display() {
  updateDisplay = true;
  display = new Adafruit_SSD1306(-1);
}

void Display::init(Values dataObj) {
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display->begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);  // initialize with the I2C addr 0x3D (for the 128x64)

  screensaverSetup();

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display->clearDisplay();
  display->drawXBitmap(0, 0, batman_bits, batman_width, batman_height, WHITE);
  display->display();
  data = dataObj;
}

void Display::loop(bool isIdle) {
  if(isIdle) {
    if(!isScreensaverPlaying()) {
      turnScreensaver(ON);
    }
    screensaverRender();
  } else {
    render();
  }
}

void Display::turnScreensaver(bool status) {
  screensaverStatus = status == ON;
  display->clearDisplay(); // start new buffer

  touch(); // We set the update flag, so when the screensaver stops it immediately renders the UI
}

bool Display::isScreensaverPlaying() {
  return screensaverStatus;
}

void Display::draw_thermometer (int temp) {
  //-- Draw the thermometer filled
  display->setTextColor(WHITE);
  display->drawCircle(10, 6, 4, WHITE);
  display->drawRect(6, 6, 9, 46, WHITE);
  display->drawCircle(10, 56, 6, WHITE);
  //-- Remove the contents
  display->fillCircle(10, 6, 3, BLACK);
  display->fillRect(7, 6, 7, 46, BLACK);
  display->fillCircle(10, 56, 5, BLACK);

  //-- Draw the graduation
  display->drawFastHLine(17, 10, 4,WHITE);
  display->drawFastHLine(17, 20, 4,WHITE);
  display->drawFastHLine(17, 30, 4,WHITE);
  display->drawFastHLine(17, 40, 4,WHITE);
  display->drawFastHLine(17, 50, 4,WHITE);
  //-- Draw the contents
  display->fillCircle(10, 56, 4, WHITE);

  //-- Draw the temperature dependant part
  int y = 40 - temp;
  int h = 15 + temp;
  display->fillRect(9,y,3,h,WHITE);
}

void Display::draw_temp(float temp) {
    String str;
    char buf[10];
    int tempInt = (int)temp;
    int tempDec = (int) (temp * 10) - tempInt * 10;

    draw_thermometer(tempInt);

    //-- Write the temperature
    display->setFont(&FreeSansBold18pt7b);
    int16_t  x1, y1;
    uint16_t w1, h1;
    int16_t top = 26;
    int16_t left = 32;
    str = String(tempInt);
    str.toCharArray(buf, 10);
    display->getTextBounds(buf, top, left, &x1, &y1, &w1, &h1);
    display->fillRect(x1,y1,w1,h1,BLACK);
    display->setCursor(left, top);
    display->print(str);

    left = x1 + w1 + 6;
    top = y1 + h1 - 7;

    str = String(".") + String(tempDec);
    str.toCharArray(buf, 10);
    display->setFont(&FreeSansBold9pt7b);
    display->getTextBounds(buf, left, top, &x1, &y1, &w1, &h1);
    display->fillRect(x1,y1,w1,h1,BLACK);
    display->setCursor(left,top);
    display->print(str);
}

void Display::draw_desired_temp(float temp) {
    char buf[10];
    String str;

    str = String(temp);
    str.toCharArray(buf, 10);
    int16_t  x1, y1;
    uint16_t w1, h1;
    display->setFont(&FreeSans9pt7b);
    display->getTextBounds(buf, 0, 0, &x1, &y1, &w1, &h1);

    uint16_t left = 128 - w1;
    uint16_t top = 64 - h1;
    display->fillRect(left, top, w1, h1, BLACK);
    display->setCursor(left-10, 58);
    display->print(str);
}

void Display::draw_wifi_icon() {
  int strength;
  // render a wifi icon, either connected or not
  if(enableServer) {
    float raw = data.getSignalStrength();
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
  display->fillRect(x, y, wifi_width, wifi_height, BLACK);
  display->drawXBitmap(x, y, wifi_icon[index], wifi_width, wifi_height, WHITE);
}

void Display::render() {
  static uint32_t lastUpdate = 0;
  uint32_t t = millis();
  if (updateDisplay || t - lastUpdate > 5000L) {
    debugln("UPDATE");
    display->clearDisplay(); // start new buffer
    // text display tests
    display->setTextColor(WHITE);

    draw_temp(data.getTemperature());
    draw_desired_temp(data.getDesiredTemperature());
    #ifdef ESP8266
    draw_wifi_icon();
    #endif
    updateDisplay = false;
    lastUpdate = t;

    display->display();
  }
}

void Display::touch() {
  updateDisplay = true;
}

/* -------------------- */

void Display::screensaverSetup(){
  randomSeed(analogRead(2));           // Seed random from unused analog input
  for(uint8_t i=0; i<N_FLYERS; i++) {  // Randomize initial flyer states
    flyer[i].x     = (-32 + random(160)) * 16;
    flyer[i].y     = (-32 + random( 96)) * 16;
    flyer[i].frame = random(3) ? random(4) : 255; // 66% toaster, else toast
    flyer[i].depth = 10 + random(16);             // Speed / stacking order
  }
  sort_flyers(flyer, N_FLYERS);
}

void Display::screensaverRender() {
  uint8_t i, f;
  int16_t x, y;
  boolean resort = false;     // By default, don't re-sort depths

  for(i=0; i<N_FLYERS; i++) { // For each flyer...

    // First draw each item...
    f = (flyer[i].frame == 255) ? 4 : (flyer[i].frame++ & 3); // Frame #
    x = flyer[i].x / 16;
    y = flyer[i].y / 16;
    #ifdef ESP8266
    display->drawBitmap(x, y, (const uint8_t *)pgm_read_dword(&mask[f]), 32, 32, BLACK);
    display->drawBitmap(x, y, (const uint8_t *)pgm_read_dword(&img[f]), 32, 32, WHITE);
    #else
    display->drawBitmap(x, y, (const uint8_t *)pgm_read_word(&mask[f]), 32, 32, BLACK);
    display->drawBitmap(x, y, (const uint8_t *)pgm_read_word(&img[f]), 32, 32, WHITE);
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

void Display::sort_flyers(Flyer a[], uint8_t al) {
  uint16_t i, j;
  for (i = 1; i < al; i++) {
    Flyer tmp = a[i];
    for (j = i; j >= 1 && compare(tmp, a[j-1]) < 0; j--)
      a[j] = a[j-1];
    a[j] = tmp;
  }
}

// Flyer depth comparison function for qsort()
int Display::compare(Flyer a, Flyer b) {
  return a.depth - b.depth;
}

