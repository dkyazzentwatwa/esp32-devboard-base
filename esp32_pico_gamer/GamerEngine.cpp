#include "GamerEngine.h"

#include <string.h>

#if GAMER_BOARD_PROFILE == GAMER_BOARD_WAVESHARE_AMOLED_18
void ArduinoGfxBridge::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (target) target->drawPixel(x, y, color);
}

void ArduinoGfxBridge::startWrite() {
  if (target) target->startWrite();
}

void ArduinoGfxBridge::writePixel(int16_t x, int16_t y, uint16_t color) {
  if (target) target->writePixel(x, y, color);
}

void ArduinoGfxBridge::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                     uint16_t color) {
  if (target) target->writeFillRect(x, y, w, h, color);
}

void ArduinoGfxBridge::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  if (target) target->writeFastVLine(x, y, h, color);
}

void ArduinoGfxBridge::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  if (target) target->writeFastHLine(x, y, w, color);
}

void ArduinoGfxBridge::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                                 uint16_t color) {
  if (target) target->writeLine(x0, y0, x1, y1, color);
}

void ArduinoGfxBridge::endWrite() {
  if (target) target->endWrite();
}

void ArduinoGfxBridge::setRotation(uint8_t rotation) {
  Adafruit_GFX::setRotation(rotation);
  if (target) target->setRotation(rotation);
}

void ArduinoGfxBridge::invertDisplay(bool inverted) {
  if (target) target->invertDisplay(inverted);
}

void ArduinoGfxBridge::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  if (target) target->drawFastVLine(x, y, h, color);
}

void ArduinoGfxBridge::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  if (target) target->drawFastHLine(x, y, w, color);
}

void ArduinoGfxBridge::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                uint16_t color) {
  if (target) target->fillRect(x, y, w, h, color);
}

void ArduinoGfxBridge::fillScreen(uint16_t color) {
  if (target) target->fillScreen(color);
}

void ArduinoGfxBridge::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                                uint16_t color) {
  if (target) target->drawLine(x0, y0, x1, y1, color);
}

void ArduinoGfxBridge::drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                uint16_t color) {
  if (target) target->drawRect(x, y, w, h, color);
}

namespace {
void remapTouchForRotation(int32_t rawX, int32_t rawY, uint16_t& outX, uint16_t& outY) {
  int32_t x = rawY;
  int32_t y = (SCREEN_HEIGHT - 1) - rawX;
  x = constrain(x, 0, static_cast<int32_t>(SCREEN_WIDTH - 1));
  y = constrain(y, 0, static_cast<int32_t>(SCREEN_HEIGHT - 1));
  outX = static_cast<uint16_t>(x);
  outY = static_cast<uint16_t>(y);
}
}
#endif

GamerEngine::GamerEngine()
#if GAMER_DISPLAY_IS_SSD1306
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN),
#elif GAMER_DISPLAY_IS_SH8601
    : amoledBus(nullptr),
      amoled(nullptr),
      display(SCREEN_WIDTH, SCREEN_HEIGHT),
      bootButton{PIN_BOOT_BUTTON, false, false, 0, 0, 0, false, false, false, false},
#endif
      buttons{
#if GAMER_DISPLAY_IS_SSD1306
          {BUTTON_LEFT_PIN, false, false, 0, 0, 0, false, false, false, false},
          {BUTTON_RIGHT_PIN, false, false, 0, 0, 0, false, false, false, false},
          {BUTTON_SELECT_PIN, false, false, 0, 0, 0, false, false, false, false}
#else
          {-1, false, false, 0, 0, 0, false, false, false, false},
          {-1, false, false, 0, 0, 0, false, false, false, false},
          {-1, false, false, 0, 0, 0, false, false, false, false}
#endif
      },
      displayOk(false),
      touchOk(false),
      powerOk(false),
      ledUntil(0),
      ambientLastUpdate(0),
      ambientHue(0) {}

void GamerEngine::begin() {
  Serial.begin(SERIAL_BAUD);
  delay(300);
  Serial.println();
  Serial.println("=== ESP32 Pico Gamer ===");
  Serial.printf("Board: %s\n", GAMER_BOARD_NAME);

#if GAMER_DISPLAY_IS_SSD1306
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
#elif GAMER_DISPLAY_IS_SH8601
  Wire.begin(PIN_TOUCH_SDA, PIN_TOUCH_SCL);
#endif

  initDisplay();
#if GAMER_BOARD_PROFILE == GAMER_BOARD_WAVESHARE_AMOLED_18
  initPower();
#endif
  initButtons();
#if GAMER_HAS_TOUCH
  initTouch();
#endif
  initLed();

  if (displayOk) {
    drawTitle("ESP32 GAMER", GAMER_DISPLAY_IS_SH8601 ? "AMOLED native" : "Pico ports",
              GAMER_HAS_TOUCH ? "TAP: PLAY" : "SEL: PLAY");
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
  while (isHeld(BTN_LEFT) || isHeld(BTN_RIGHT) || isHeld(BTN_SELECT)
#if GAMER_HAS_BOOT_BUTTON
         || bootButton.stablePressed
#endif
  ) {
    tick();
    delay(10);
  }
}

Adafruit_GFX& GamerEngine::screen() {
  return display;
}

uint16_t GamerEngine::width() const {
  return SCREEN_WIDTH;
}

uint16_t GamerEngine::height() const {
  return SCREEN_HEIGHT;
}

uint8_t GamerEngine::textScale() const {
  return GAMER_DISPLAY_IS_SH8601 ? 2 : 1;
}

String GamerEngine::powerLabel() {
#if GAMER_BOARD_PROFILE == GAMER_BOARD_WAVESHARE_AMOLED_18
  if (!powerOk) return String();
  if (pmu.isBatteryConnect()) {
    String label;
    label += pmu.getBatteryPercent();
    label += "%";
    if (pmu.isCharging()) label += " CHG";
    return label;
  }
  if (pmu.isVbusIn()) return "USB";
#endif
  return String();
}

void GamerEngine::clear() {
  if (!displayOk) return;
#if GAMER_DISPLAY_IS_SSD1306
  display.clearDisplay();
#else
  display.fillScreen(GAMER_BLACK);
#endif
}

void GamerEngine::show() {
  if (!displayOk) return;
#if GAMER_DISPLAY_IS_SSD1306
  display.display();
#endif
}

void GamerEngine::centerText(const char* text, int16_t y, uint8_t size, uint16_t color) {
  if (!displayOk) return;
  display.setTextSize(size);
  display.setTextColor(color, GAMER_BLACK);
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t textW = 0;
  uint16_t textH = 0;
  display.getTextBounds(text, 0, 0, &x1, &y1, &textW, &textH);
  int16_t x = (static_cast<int16_t>(width()) - static_cast<int16_t>(textW)) / 2 - x1;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.print(text);
  display.setTextSize(textScale());
  display.setTextColor(GAMER_WHITE, GAMER_BLACK);
}

void GamerEngine::rightText(const char* text, int16_t y, uint16_t color) {
  if (!displayOk) return;
  display.setTextSize(textScale());
  display.setTextColor(color, GAMER_BLACK);
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t textW = 0;
  uint16_t textH = 0;
  display.getTextBounds(text, 0, 0, &x1, &y1, &textW, &textH);
  int16_t x = static_cast<int16_t>(width()) - static_cast<int16_t>(textW) - 2 - x1;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.print(text);
  display.setTextColor(GAMER_WHITE, GAMER_BLACK);
}

void GamerEngine::drawTitle(const char* title, const char* line1, const char* line2) {
  clear();
  display.drawRect(0, 0, width(), height(), GAMER_DIM);
  const uint8_t titleSize = GAMER_DISPLAY_IS_SH8601 ? 3 : 1;
  const uint8_t bodySize = textScale();
  centerText(title, GAMER_DISPLAY_IS_SH8601 ? 112 : 14, titleSize, GAMER_WHITE);
  centerText(line1, GAMER_DISPLAY_IS_SH8601 ? 210 : 34, bodySize, GAMER_ACCENT);
  centerText(line2, GAMER_DISPLAY_IS_SH8601 ? 292 : 50, bodySize, GAMER_DIM);
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
  const int16_t bandH = GAMER_DISPLAY_IS_SH8601 ? 96 : 28;
  const int16_t bandY = (height() - bandH) / 2;
  display.fillRect(0, bandY, width(), bandH, GAMER_ACCENT);
  centerText(title, bandY + bandH / 2 - (GAMER_DISPLAY_IS_SH8601 ? 12 : 3), textScale(),
             GAMER_BLACK);
  centerText(detail, height() - (GAMER_DISPLAY_IS_SH8601 ? 74 : 12), textScale(), GAMER_WHITE);
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
  display.setTextSize(textScale());
  display.setTextColor(GAMER_WHITE, GAMER_BLACK);
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
#if GAMER_DISPLAY_IS_SSD1306
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
  display.setTextColor(GAMER_WHITE);
  display.display();
  Serial.println("OK: SSD1306 display ready");
#elif GAMER_DISPLAY_IS_SH8601
  if (!expander.begin(AMOLED_EXPANDER_I2C_ADDR, &Wire)) {
    Serial.println("AMOLED XCA9554 not found; trying display init anyway");
  } else {
    for (uint8_t pin = 0; pin < 3; pin++) {
      expander.pinMode(pin, OUTPUT);
      expander.digitalWrite(pin, LOW);
    }
    delay(20);
    for (uint8_t pin = 0; pin < 3; pin++) {
      expander.digitalWrite(pin, HIGH);
    }
  }

  amoledBus = new Arduino_ESP32QSPI(PIN_LCD_CS, PIN_LCD_SCLK, PIN_LCD_SDIO0,
                                    PIN_LCD_SDIO1, PIN_LCD_SDIO2, PIN_LCD_SDIO3);
  amoled = new Arduino_SH8601(amoledBus, PIN_LCD_RST, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  if (!amoled->begin()) {
    Serial.println("ERROR: SH8601 display init failed");
    displayOk = false;
    return;
  }

  amoled->setBrightness(AMOLED_BRIGHTNESS);
  amoled->fillScreen(GAMER_BLACK);
  display.attach(amoled);
  display.setRotation(0);
  display.setTextWrap(false);
  display.setTextSize(textScale());
  display.setTextColor(GAMER_WHITE, GAMER_BLACK);
  displayOk = true;
  Serial.println("OK: SH8601 AMOLED ready");
#endif
}

void GamerEngine::initButtons() {
#if GAMER_DISPLAY_IS_SSD1306
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(buttons[i].pin, INPUT);
    const bool pressed = digitalRead(buttons[i].pin) == BUTTON_ACTIVE_STATE;
    buttons[i].lastRawPressed = pressed;
    buttons[i].stablePressed = pressed;
    buttons[i].lastRawChange = millis();
  }
  Serial.println("OK: Buttons ready");
#elif GAMER_DISPLAY_IS_SH8601
  pinMode(PIN_BOOT_BUTTON, BOOT_BUTTON_ACTIVE_LOW ? INPUT_PULLUP : INPUT_PULLDOWN);
  const bool bootPressed = digitalRead(PIN_BOOT_BUTTON) == (BOOT_BUTTON_ACTIVE_LOW ? LOW : HIGH);
  bootButton.lastRawPressed = bootPressed;
  bootButton.stablePressed = bootPressed;
  bootButton.lastRawChange = millis();
  Serial.println("OK: Touch zones + BOOT exit ready");
#endif
}

#if GAMER_BOARD_PROFILE == GAMER_BOARD_WAVESHARE_AMOLED_18
void GamerEngine::initPower() {
  powerOk = pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, PIN_TOUCH_SDA, PIN_TOUCH_SCL);
  if (!powerOk) {
    Serial.println("AXP2101 PMIC unavailable");
    return;
  }

  pmu.enableBattDetection();
  pmu.enableBattVoltageMeasure();
  pmu.enableVbusVoltageMeasure();
  pmu.enableSystemVoltageMeasure();
  Serial.printf("OK: AXP2101 PMIC ready id=0x%X\n", static_cast<unsigned>(pmu.getChipID()));
}
#endif

#if GAMER_HAS_TOUCH
void GamerEngine::initTouch() {
  touchBus = std::make_shared<Arduino_HWIIC>(PIN_TOUCH_SDA, PIN_TOUCH_SCL, &Wire);
  touchDevice.reset(new Arduino_FT3x68(touchBus, FT3168_DEVICE_ADDRESS, DRIVEBUS_DEFAULT_VALUE,
                                       PIN_TOUCH_INT));

  for (uint8_t tries = 0; tries < 5; tries++) {
    if (touchDevice->begin()) {
      touchDevice->IIC_Write_Device_State(touchDevice->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
                                          touchDevice->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);
      Serial.printf("OK: FT3168 touch ready id=0x%X\n",
                    static_cast<unsigned>(touchDevice->IIC_Read_Device_ID()));
      touchOk = true;
      return;
    }
    Serial.println("FT3168 touch init retry");
    delay(400);
  }

  Serial.println("ERROR: FT3168 touch init failed");
  touchOk = false;
}

bool GamerEngine::readTouchPressed(uint16_t& x, uint16_t& y) {
  if (!touchOk || !touchDevice) return false;

  const int32_t fingers = static_cast<int32_t>(
      touchDevice->IIC_Read_Device_Value(touchDevice->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER));
  if (fingers <= 0) return false;

  const int32_t rawX = static_cast<int32_t>(
      touchDevice->IIC_Read_Device_Value(touchDevice->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X));
  const int32_t rawY = static_cast<int32_t>(
      touchDevice->IIC_Read_Device_Value(touchDevice->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y));
  remapTouchForRotation(rawX, rawY, x, y);
  return true;
}

void GamerEngine::updateTouchButtons() {
  uint16_t x = 0;
  uint16_t y = 0;
  const bool touched = readTouchPressed(x, y);

  const uint16_t leftEdge = width() / 3;
  const uint16_t rightEdge = (width() * 2) / 3;
  updateButton(buttons[BTN_LEFT], touched && x < leftEdge);
  updateButton(buttons[BTN_RIGHT], touched && x >= rightEdge);
  updateButton(buttons[BTN_SELECT], touched && x >= leftEdge && x < rightEdge);

  const bool bootPressed = digitalRead(PIN_BOOT_BUTTON) == (BOOT_BUTTON_ACTIVE_LOW ? LOW : HIGH);
  updateButton(bootButton, bootPressed, false);
  if (bootButton.longEvent) {
    buttons[BTN_SELECT].longEvent = true;
    buttons[BTN_SELECT].longFired = true;
  }
}
#endif

void GamerEngine::initLed() {
  FastLED.addLeds<WS2812B, RGB_LED_PIN, GRB>(leds, RGB_LED_COUNT);
  FastLED.setBrightness(RGB_LED_BRIGHTNESS);
  ledOff();
  Serial.println("OK: RGB LED ready");
}

void GamerEngine::updateButton(ButtonRuntime& button, bool rawPressed, bool allowShortRelease) {
  const uint32_t now = millis();
  button.pressEvent = false;
  button.releaseEvent = false;
  button.longEvent = false;

  if (rawPressed != button.lastRawPressed) {
    button.lastRawPressed = rawPressed;
    button.lastRawChange = now;
  }

  if ((now - button.lastRawChange) < BUTTON_DEBOUNCE_MS) {
    return;
  }

  if (rawPressed != button.stablePressed) {
    button.stablePressed = rawPressed;
    if (button.stablePressed) {
      button.pressEvent = true;
      button.longFired = false;
      button.pressedAt = now;
      button.releaseDuration = 0;
    } else {
      const uint32_t heldFor = min<uint32_t>(now - button.pressedAt, 65535);
      button.releaseDuration = static_cast<uint16_t>(heldFor);
      button.releaseEvent = allowShortRelease;
    }
  }

  if (button.stablePressed && !button.longFired &&
      (now - button.pressedAt) >= BUTTON_LONGPRESS_MS) {
    button.longFired = true;
    button.longEvent = true;
  }
}

void GamerEngine::updateButtons() {
#if GAMER_HAS_TOUCH
  updateTouchButtons();
#else
  for (uint8_t i = 0; i < 3; i++) {
    const bool rawPressed = digitalRead(buttons[i].pin) == BUTTON_ACTIVE_STATE;
    updateButton(buttons[i], rawPressed);
  }
#endif
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
