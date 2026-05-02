#include "GamerEngine.h"

GamerEngine::GamerEngine()
  : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN),
    buttons{
      {BUTTON_LEFT_PIN, false, false, 0, 0, 0, false, false, false, false},
      {BUTTON_RIGHT_PIN, false, false, 0, 0, 0, false, false, false, false},
      {BUTTON_SELECT_PIN, false, false, 0, 0, 0, false, false, false, false}
    },
    displayOk(false),
    ledUntil(0),
    ambientLastUpdate(0),
    ambientHue(0) {
}

void GamerEngine::begin() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("=== ESP32 Pico Gamer ===");

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  initDisplay();
  initButtons();
  initLed();

  if (displayOk) {
    drawTitle("ESP32 GAMER", "Pico ports", "SEL: PLAY");
    delay(900);
  }
  Serial.println("READY");
}

void GamerEngine::tick() {
  updateButtons();
  updateLed();
}

bool GamerEngine::displayReady() const {
  return displayOk;
}

bool GamerEngine::isHeld(GamerButton button) const {
  return buttons[button].stablePressed;
}

bool GamerEngine::wasPressed(GamerButton button) const {
  return buttons[button].pressEvent;
}

bool GamerEngine::wasReleased(GamerButton button) const {
  return buttons[button].releaseEvent;
}

bool GamerEngine::wasLongPressed(GamerButton button) const {
  return buttons[button].longEvent;
}

uint16_t GamerEngine::releasedDuration(GamerButton button) const {
  return buttons[button].releaseDuration;
}

bool GamerEngine::shouldExitGame() const {
  return wasLongPressed(BTN_SELECT);
}

void GamerEngine::waitForRelease() {
  while (isHeld(BTN_LEFT) || isHeld(BTN_RIGHT) || isHeld(BTN_SELECT)) {
    tick();
    delay(10);
  }
}

Adafruit_SSD1306& GamerEngine::screen() {
  return display;
}

void GamerEngine::clear() {
  if (!displayOk) return;
  display.clearDisplay();
}

void GamerEngine::show() {
  if (!displayOk) return;
  display.display();
}

void GamerEngine::centerText(const char* text, int16_t y, uint8_t size, uint16_t color) {
  if (!displayOk) return;
  display.setTextSize(size);
  display.setTextColor(color);
  int16_t x = (SCREEN_WIDTH - static_cast<int16_t>(strlen(text)) * 6 * size) / 2;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.print(text);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void GamerEngine::rightText(const char* text, int16_t y, uint16_t color) {
  if (!displayOk) return;
  display.setTextSize(1);
  display.setTextColor(color);
  int16_t x = SCREEN_WIDTH - static_cast<int16_t>(strlen(text)) * 6;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.print(text);
  display.setTextColor(SSD1306_WHITE);
}

void GamerEngine::drawTitle(const char* title, const char* line1, const char* line2) {
  clear();
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  centerText(title, 14, 1);
  centerText(line1, 34, 1);
  centerText(line2, 50, 1);
  show();
}

bool GamerEngine::waitForSelectOrExit(const char* title, const char* line1, const char* line2) {
  waitForRelease();
  drawTitle(title, line1, line2);

  while (true) {
    tick();
    if (wasLongPressed(BTN_SELECT)) {
      waitForRelease();
      ledPulse(CRGB::Red, 120);
      return false;
    }
    if (wasReleased(BTN_SELECT) && releasedDuration(BTN_SELECT) < BUTTON_LONGPRESS_MS) {
      ledPulse(CRGB::Green, 90);
      return true;
    }
    delay(10);
  }
}

bool GamerEngine::showResult(const char* title, const char* detail) {
  waitForRelease();
  clear();
  display.fillRect(0, 18, SCREEN_WIDTH, 28, SSD1306_WHITE);
  centerText(title, 27, 1, SSD1306_BLACK);
  centerText(detail, 52, 1);
  show();

  while (true) {
    tick();
    if (wasLongPressed(BTN_SELECT)) {
      waitForRelease();
      ledPulse(CRGB::Red, 160);
      return false;
    }
    if (wasReleased(BTN_SELECT) && releasedDuration(BTN_SELECT) < BUTTON_LONGPRESS_MS) {
      ledPulse(CRGB::Green, 90);
      return true;
    }
    delay(10);
  }
}

void GamerEngine::drawHeader(const char* title, int16_t value) {
  if (!displayOk) return;
  display.setCursor(0, 0);
  display.print(title);
  char valueText[10];
  snprintf(valueText, sizeof(valueText), "%d", value);
  rightText(valueText, 0);
}

void GamerEngine::ledOff() {
  leds[0] = CRGB::Black;
  FastLED.show();
  ledUntil = 0;
}

void GamerEngine::ledSet(const CRGB& color) {
  leds[0] = color;
  FastLED.show();
  ledUntil = 0;
}

void GamerEngine::ledPulse(const CRGB& color, uint16_t durationMs) {
  leds[0] = color;
  FastLED.show();
  ledUntil = millis() + durationMs;
}

void GamerEngine::initDisplay() {
  displayOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR_PRIMARY);
  if (!displayOk) {
    displayOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR_SECONDARY);
  }

  if (!displayOk) {
    Serial.println("ERROR: SSD1306 display not found");
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
  Serial.println("OK: SSD1306 display ready");
}

void GamerEngine::initButtons() {
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(buttons[i].pin, INPUT);
    bool pressed = digitalRead(buttons[i].pin) == BUTTON_ACTIVE_STATE;
    buttons[i].lastRawPressed = pressed;
    buttons[i].stablePressed = pressed;
    buttons[i].lastRawChange = millis();
  }
  Serial.println("OK: Buttons ready");
}

void GamerEngine::initLed() {
  FastLED.addLeds<WS2812B, RGB_LED_PIN, GRB>(leds, RGB_LED_COUNT);
  FastLED.setBrightness(RGB_LED_BRIGHTNESS);
  ledOff();
  Serial.println("OK: RGB LED ready");
}

void GamerEngine::updateButtons() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < 3; i++) {
    ButtonRuntime& button = buttons[i];
    button.pressEvent = false;
    button.releaseEvent = false;
    button.longEvent = false;

    bool rawPressed = digitalRead(button.pin) == BUTTON_ACTIVE_STATE;
    if (rawPressed != button.lastRawPressed) {
      button.lastRawPressed = rawPressed;
      button.lastRawChange = now;
    }

    if ((now - button.lastRawChange) < BUTTON_DEBOUNCE_MS) {
      continue;
    }

    if (rawPressed != button.stablePressed) {
      button.stablePressed = rawPressed;
      if (button.stablePressed) {
        button.pressEvent = true;
        button.longFired = false;
        button.pressedAt = now;
        button.releaseDuration = 0;
      } else {
        button.releaseEvent = true;
        uint32_t heldFor = now - button.pressedAt;
        if (heldFor > 65535) heldFor = 65535;
        button.releaseDuration = static_cast<uint16_t>(heldFor);
      }
    }

    if (button.stablePressed && !button.longFired && (now - button.pressedAt) >= BUTTON_LONGPRESS_MS) {
      button.longFired = true;
      button.longEvent = true;
    }
  }
}

void GamerEngine::updateLed() {
  if (ledUntil != 0 && static_cast<int32_t>(millis() - ledUntil) >= 0) {
    ledUntil = 0;
  }

  if (ledUntil != 0) {
    return;
  }

  uint32_t now = millis();
  if (now - ambientLastUpdate < 35) {
    return;
  }

  ambientLastUpdate = now;
  ambientHue++;
  uint8_t wave = sin8(now / 14);
  uint8_t brightness = 8 + scale8(wave, RGB_LED_BRIGHTNESS - 8);
  leds[0] = CHSV(ambientHue, 220, brightness);
  FastLED.show();
}
