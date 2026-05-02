#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "GamerConfig.h"

enum GamerButton : uint8_t {
  BTN_LEFT = 0,
  BTN_RIGHT = 1,
  BTN_SELECT = 2
};

struct ButtonRuntime {
  uint8_t pin;
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

  Adafruit_SSD1306& screen();
  void clear();
  void show();
  void centerText(const char* text, int16_t y, uint8_t size = 1, uint16_t color = SSD1306_WHITE);
  void rightText(const char* text, int16_t y = 0, uint16_t color = SSD1306_WHITE);
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
  void updateButtons();
  void updateLed();

  Adafruit_SSD1306 display;
  ButtonRuntime buttons[3];
  CRGB leds[RGB_LED_COUNT];
  bool displayOk;
  uint32_t ledUntil;
  uint32_t ambientLastUpdate;
  uint8_t ambientHue;
};
