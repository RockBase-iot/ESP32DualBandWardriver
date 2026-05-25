#ifndef display_h
#define display_h

#include <FS.h>
#include <LinkedList.h>
#include <Adafruit_GFX.h>

#include "configs.h"

#ifdef NM_CYD_C5
  #include <Adafruit_ST7789.h>
#else
  #include <Adafruit_ST7735.h>
#endif
#include <functional>

#include "assets.h"

#include "BatteryInterface.h"

extern BatteryInterface battery;

#ifdef JCMK_HOST_BOARD
  #define CYAN 0xFFE0
#else
  #define CYAN ST77XX_CYAN
#endif

class Display {
  public:
    int _cs, _dc, _rst;
    Display(SPIClass* spi, int cs, int dc, int rst);
    #ifdef NM_CYD_C5
      Adafruit_ST7789* tft;
    #else
      Adafruit_ST7735* tft;
    #endif

    void begin();
    void main(uint32_t currentTime);
    void clearScreen();
    void ctrlBacklight(bool on = true);
    void drawCenteredText(String text, bool centerVertically = false);

  private:
    SPIClass* _spi;

    void drawMonochromeImage160x80(const uint8_t* imageData, int width, int height);

};

#endif