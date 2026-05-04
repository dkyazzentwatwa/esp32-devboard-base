#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <Adafruit_GFX.h>

#include "GamerConfig.h"

#if GAMER_BOARD_PROFILE == GAMER_BOARD_DEVKIT_SSD1306
#include <Adafruit_SSD1306.h>
#elif GAMER_BOARD_PROFILE == GAMER_BOARD_WAVESHARE_AMOLED_18
#include <Adafruit_XCA9554.h>
#include <Arduino_DriveBus_Library.h>
#include <Arduino_GFX_Library.h>
#define XPOWERS_CHIP_AXP2101
#include <XPowersLib.h>
#include <memory>
#endif

#if GAMER_BOARD_PROFILE == GAMER_BOARD_WAVESHARE_AMOLED_18
class ArduinoGfxBridge : public Adafruit_GFX {
public:
  ArduinoGfxBridge(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {}

  void attach(Arduino_GFX* driver) { target = driver; }
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void startWrite() override;
  void writePixel(int16_t x, int16_t y, uint16_t color) override;
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;
  void endWrite() override;
  void setRotation(uint8_t rotation) override;
  void invertDisplay(bool inverted) override;
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void fillScreen(uint16_t color) override;
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

private:
  Arduino_GFX* target = nullptr;
};
#endif

enum GamerButton : uint8_t {
  BTN_LEFT = 0,
  BTN_RIGHT = 1,
  BTN_SELECT = 2
};

struct ButtonRuntime {
  int pin;
  bool lastRawPressed;
  bool stablePressed;
  uint32_t lastRawChange;
  uint32_t pressedAt;
  uint16_t releaseDuration;
  bool pressEvent;
  bool releaseEvent;
  bool longEvent;
  bool longFired;
};

class GamerEngine {
public:
  GamerEngine();

  void begin();
  void tick();
  bool displayReady() const;

  bool isHeld(GamerButton button) const;
  bool wasPressed(GamerButton button) const;
  bool wasReleased(GamerButton button) const;
  bool wasLongPressed(GamerButton button) const;
  uint16_t releasedDuration(GamerButton button) const;
  bool shouldExitGame() const;
  void waitForRelease();

  Adafruit_GFX& screen();
  uint16_t width() const;
  uint16_t height() const;
  uint8_t textScale() const;
  String powerLabel();

  void clear();
  void show();
  void centerText(const char* text, int16_t y, uint8_t size = 1, uint16_t color = GAMER_WHITE);
  void rightText(const char* text, int16_t y = 0, uint16_t color = GAMER_WHITE);
  void drawTitle(const char* title, const char* line1, const char* line2);
  bool waitForSelectOrExit(const char* title, const char* line1, const char* line2);
  bool showResult(const char* title, const char* detail);
  void drawHeader(const char* title, int16_t value);

  void ledOff();
  void ledSet(const CRGB& color);
  void ledPulse(const CRGB& color, uint16_t durationMs);

private:
  void initDisplay();
  void initButtons();
  void initLed();
#if GAMER_BOARD_PROFILE == GAMER_BOARD_WAVESHARE_AMOLED_18
  void initPower();
#endif
  void updateButtons();
  void updateButton(ButtonRuntime& button, bool rawPressed, bool allowShortRelease = true);
#if GAMER_HAS_TOUCH
  void initTouch();
  bool readTouchPressed(uint16_t& x, uint16_t& y);
  void updateTouchButtons();
#endif
  void updateLed();

#if GAMER_DISPLAY_IS_SSD1306
  Adafruit_SSD1306 display;
#elif GAMER_DISPLAY_IS_SH8601
  Arduino_DataBus* amoledBus;
  Arduino_SH8601* amoled;
  ArduinoGfxBridge display;
  Adafruit_XCA9554 expander;
  XPowersPMU pmu;
  std::shared_ptr<Arduino_IIC_DriveBus> touchBus;
  std::unique_ptr<Arduino_FT3x68> touchDevice;
  ButtonRuntime bootButton;
#endif
  ButtonRuntime buttons[3];
  CRGB leds[RGB_LED_COUNT];
  bool displayOk;
  bool touchOk;
  bool powerOk;
  uint32_t ledUntil;
  uint32_t ambientLastUpdate;
  uint8_t ambientHue;
};
