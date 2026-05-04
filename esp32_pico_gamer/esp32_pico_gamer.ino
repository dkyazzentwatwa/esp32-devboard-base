#include "GamerEngine.h"
#include "Games.h"

GamerEngine engine;

uint8_t selectedGame = 0;
bool menuDirty = true;

void drawLauncher() {
  engine.clear();
  Adafruit_GFX& display = engine.screen();
  const uint8_t scale = engine.textScale();
  const int16_t margin = GAMER_DISPLAY_IS_SH8601 ? 16 : 0;
  const int16_t headerH = GAMER_DISPLAY_IS_SH8601 ? 44 : 10;
  const int16_t footerH = GAMER_DISPLAY_IS_SH8601 ? 34 : 10;
  const int16_t rowH = GAMER_DISPLAY_IS_SH8601 ? 34 : 10;

  display.setTextSize(scale);
  display.setTextColor(GAMER_WHITE, GAMER_BLACK);
  display.setCursor(margin, GAMER_DISPLAY_IS_SH8601 ? 14 : 0);
  display.print(GAMER_DISPLAY_IS_SH8601 ? "ESP32 AMOLED GAMER" : "ESP32 PICO GAMER");

  char countText[8];
  snprintf(countText, sizeof(countText), "%u/%u", selectedGame + 1, GAME_COUNT);
  String headerRight = countText;
  const String power = engine.powerLabel();
  if (power.length() > 0) {
    headerRight += " ";
    headerRight += power;
  }
  engine.rightText(headerRight.c_str(), GAMER_DISPLAY_IS_SH8601 ? 14 : 0);

  const uint8_t rows = max<uint8_t>(4, (engine.height() - headerH - footerH - 10) / rowH);
  uint8_t first = selectedGame >= rows ? selectedGame - rows + 1 : 0;
  if (first + rows > GAME_COUNT) first = GAME_COUNT > rows ? GAME_COUNT - rows : 0;

  for (uint8_t row = 0; row < rows; row++) {
    uint8_t index = first + row;
    if (index >= GAME_COUNT) break;
    int16_t y = headerH + 4 + row * rowH;
    if (index == selectedGame) {
      display.fillRect(0, y - 4, engine.width(), rowH - 3, GAMER_ACCENT);
      display.setTextColor(GAMER_BLACK, GAMER_ACCENT);
      display.setCursor(margin, y);
      display.print(GAME_LIBRARY[index].title);
      display.setTextColor(GAMER_WHITE, GAMER_BLACK);
    } else {
      display.setCursor(margin, y);
      display.print(GAME_LIBRARY[index].title);
    }
  }

  display.setCursor(margin, engine.height() - footerH + (GAMER_DISPLAY_IS_SH8601 ? 8 : 0));
  display.print(GAME_LIBRARY[selectedGame].category);
  engine.rightText(GAMER_HAS_TOUCH ? "TAP PLAY" : "SEL PLAY",
                   engine.height() - footerH + (GAMER_DISPLAY_IS_SH8601 ? 8 : 0));
  engine.show();
}

void setup() {
  randomSeed(micros());
  engine.begin();
}

void loop() {
  engine.tick();

  if (menuDirty) {
    drawLauncher();
    menuDirty = false;
  }

  if (engine.wasPressed(BTN_LEFT)) {
    selectedGame = selectedGame == 0 ? GAME_COUNT - 1 : selectedGame - 1;
    engine.ledPulse(CRGB::Blue, 55);
    menuDirty = true;
  }

  if (engine.wasPressed(BTN_RIGHT)) {
    selectedGame = (selectedGame + 1) % GAME_COUNT;
    engine.ledPulse(CRGB::Blue, 55);
    menuDirty = true;
  }

  if (engine.wasReleased(BTN_SELECT) && engine.releasedDuration(BTN_SELECT) < BUTTON_LONGPRESS_MS) {
    engine.ledPulse(CRGB::Green, 90);
    GAME_LIBRARY[selectedGame].run(engine);
    engine.waitForRelease();
    menuDirty = true;
  }

  delay(10);
}
