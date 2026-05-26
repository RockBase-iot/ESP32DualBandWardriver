#include "display.h"

//Display::Display() {
//
//}

Display::Display(SPIClass* spi, int cs, int dc, int rst)
  : _spi(spi), _cs(cs), _dc(dc), _rst(rst) {
  #ifdef NM_CYD_C5
    tft = new Adafruit_ST7789(_spi, _cs, _dc, _rst);
  #else
    tft = new Adafruit_ST7735(_spi, _cs, _dc, _rst);
  #endif
}

void Display::begin() {
  pinMode(TFT_BL, OUTPUT);
  
  this->ctrlBacklight(false);

  #ifdef NM_CYD_C5
    tft->init(TFT_WIDTH, TFT_HEIGHT);
  #elif !defined(JCMK_HOST_BOARD)
    tft->initR(INITR_MINI160x80_PLUGIN);
  #else
    tft->initR(INITR_MINI160x80);
  #endif

  tft->setSPISpeed(TFT_SPI_SPEED);

  this->clearScreen();
  
  tft->setTextWrap(false);
  tft->setRotation(TFT_ROTATION);

  this->drawMonochromeImageCentered(logo2, 160, 80);
  this->ctrlBacklight(true);
}

void Display::drawCenteredText(String text, bool centerVertically, uint8_t textSize) {
  tft->setRotation(TFT_ROTATION);
  tft->setTextSize(textSize);
  tft->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft->setTextWrap(false);

  uint16_t charWidth = 6 * textSize;
  uint16_t charHeight = 8 * textSize;

  uint16_t textWidth = text.length() * charWidth;
  uint16_t textHeight = charHeight;

  uint16_t screenWidth = tft->width();
  uint16_t screenHeight = tft->height();
  uint16_t x = textWidth < screenWidth ? (screenWidth - textWidth) / 2 : 0;
  uint16_t y = centerVertically && textHeight < screenHeight ? (screenHeight - textHeight) / 2 : tft->getCursorY();

  tft->setCursor(x, y);
  tft->print(text);
}

// https://javl.github.io/image2cpp/
void Display::drawMonochromeImageCentered(const uint8_t* imageData, int width, int height) {
  int screenWidth = tft->width();
  int screenHeight = tft->height();
  int targetWidth = min(screenWidth - 16, width * 2);
  int targetHeight = (targetWidth * height) / width;

  if (targetHeight > screenHeight - 16) {
    targetHeight = screenHeight - 16;
    targetWidth = (targetHeight * width) / height;
  }

  if (targetWidth < width || targetHeight < height) {
    targetWidth = width;
    targetHeight = height;
  }

  int startX = (screenWidth - targetWidth) / 2;
  int startY = (screenHeight - targetHeight) / 2;

  tft->startWrite();

  for (int y = 0; y < targetHeight; y++) {
    int srcY = (y * height) / targetHeight;
    for (int x = 0; x < targetWidth; x++) {
      int srcX = (x * width) / targetWidth;
      int byteIndex = (srcY * (width / 8)) + (srcX / 8);
      uint8_t byteVal = pgm_read_byte(&imageData[byteIndex]);

      // MSB first (bit 7 is leftmost pixel)
      bool pixelOn = (byteVal >> (7 - (srcX % 8))) & 0x01;
      uint16_t color = pixelOn ? ST77XX_WHITE : ST77XX_BLACK;

      tft->writePixel(startX + x, startY + y, color);
    }
  }

  tft->endWrite();
}

void Display::ctrlBacklight(bool on) {
  if (on)
    digitalWrite(TFT_BL, ON);
  else
    digitalWrite(TFT_BL, OFF);
}

void Display::clearScreen() {
  tft->fillScreen(ST77XX_BLACK);
}

void Display::main(uint32_t currentTime) {

}