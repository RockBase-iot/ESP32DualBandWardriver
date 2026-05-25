#pragma once

#ifndef BatteryInterface_h
#define BatteryInterface_h

#include <Arduino.h>

#include "configs.h"
#include "utils.h"
#include "logger.h"

#ifdef HAS_BATTERY
  #include "Adafruit_MAX1704X.h"
  #include <Wire.h>
#endif

#define IP5306_ADDR 0x75
#define MAX17048_ADDR 0x36

class BatteryInterface {
  private:
    uint32_t initTime = 0;
    #ifdef HAS_BATTERY
      Adafruit_MAX17048 maxlipo;
    #endif

  public:
    int8_t battery_level = 0;
    int8_t old_level = 0;
    bool i2c_supported = false;
    bool has_max17048 = false;
    bool has_ip5306 = false;

    BatteryInterface();

    void RunSetup();
    void main(uint32_t currentTime);
    int8_t getBatteryLevel();
};

#endif
