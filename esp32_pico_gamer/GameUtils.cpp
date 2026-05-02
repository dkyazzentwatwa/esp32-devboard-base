#include "GameUtils.h"

bool selectTap(GamerEngine& engine) {
  return engine.wasReleased(BTN_SELECT) && engine.releasedDuration(BTN_SELECT) < BUTTON_LONGPRESS_MS;
}

bool runIntro(GamerEngine& engine, const char* title, const char* help) {
  return engine.waitForSelectOrExit(title, help, "SEL start");
}

bool resultScreen(GamerEngine& engine, const char* title, int16_t value) {
  char detail[18];
  snprintf(detail, sizeof(detail), "Score %d", value);
  return engine.showResult(title, detail);
}

bool rectsOverlap(int16_t ax, int16_t ay, int16_t aw, int16_t ah,
                  int16_t bx, int16_t by, int16_t bw, int16_t bh) {
  return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

int8_t heldAxis(GamerEngine& engine) {
  if (engine.isHeld(BTN_LEFT)) return -1;
  if (engine.isHeld(BTN_RIGHT)) return 1;
  return 0;
}

void drawScore(GamerEngine& engine, int16_t value) {
  char scoreText[10];
  snprintf(scoreText, sizeof(scoreText), "%d", value);
  engine.rightText(scoreText);
}

void drawCursorBox(GamerEngine& engine, int16_t x, int16_t y, int16_t w, int16_t h) {
  engine.screen().drawRect(x - 1, y - 1, w + 2, h + 2, SSD1306_WHITE);
}

void drawMiniGrid(GamerEngine& engine, uint8_t cols, uint8_t rows, uint8_t cell,
                  int16_t ox, int16_t oy) {
  for (uint8_t x = 0; x <= cols; x++) {
    engine.screen().drawFastVLine(ox + x * cell, oy, rows * cell, SSD1306_WHITE);
  }
  for (uint8_t y = 0; y <= rows; y++) {
    engine.screen().drawFastHLine(ox, oy + y * cell, cols * cell, SSD1306_WHITE);
  }
}

