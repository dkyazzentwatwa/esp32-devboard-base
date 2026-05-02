#include "GamerEngine.h"
#include "Games.h"

GamerEngine engine;

uint8_t selectedGame = 0;
bool menuDirty = true;

void drawLauncher() {
  engine.clear();
  Adafruit_SSD1306& display = engine.screen();
  display.setCursor(0, 0);
  display.print("ESP32 PICO GAMER");

  char countText[8];
  snprintf(countText, sizeof(countText), "%u/%u", selectedGame + 1, GAME_COUNT);
  engine.rightText(countText);

  const uint8_t rows = 4;
  uint8_t first = selectedGame >= rows ? selectedGame - rows + 1 : 0;
  if (first + rows > GAME_COUNT) first = GAME_COUNT > rows ? GAME_COUNT - rows : 0;

  for (uint8_t row = 0; row < rows; row++) {
    uint8_t index = first + row;
    if (index >= GAME_COUNT) break;
    int16_t y = 14 + row * 10;
    if (index == selectedGame) {
      display.fillRect(0, y - 1, SCREEN_WIDTH, 9, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(2, y);
      display.print(GAME_LIBRARY[index].title);
      display.setTextColor(SSD1306_WHITE);
    } else {
      display.setCursor(2, y);
      display.print(GAME_LIBRARY[index].title);
    }
  }

  display.setCursor(0, 56);
  display.print(GAME_LIBRARY[selectedGame].category);
  engine.rightText("SEL PLAY", 56);
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

