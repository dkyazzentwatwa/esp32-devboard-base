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

int16_t gameX(GamerEngine& engine, int16_t oledX) {
  return static_cast<int32_t>(oledX) * engine.width() / 128;
}

int16_t gameY(GamerEngine& engine, int16_t oledY) {
  return static_cast<int32_t>(oledY) * engine.height() / 64;
}

int16_t gameSize(GamerEngine& engine, int16_t oledSize) {
  const int16_t sx = max<int16_t>(1, static_cast<int32_t>(oledSize) * engine.width() / 128);
  const int16_t sy = max<int16_t>(1, static_cast<int32_t>(oledSize) * engine.height() / 64);
  return min(sx, sy);
}

void drawCursorBox(GamerEngine& engine, int16_t x, int16_t y, int16_t w, int16_t h) {
  engine.screen().drawRect(x - 1, y - 1, w + 2, h + 2, GAMER_WHITE);
}

void drawMiniGrid(GamerEngine& engine, uint8_t cols, uint8_t rows, uint8_t cell,
                  int16_t ox, int16_t oy) {
  const int16_t scaledCell = gameSize(engine, cell);
  const int16_t scaledOx = gameX(engine, ox);
  const int16_t scaledOy = gameY(engine, oy);
  for (uint8_t x = 0; x <= cols; x++) {
    engine.screen().drawFastVLine(scaledOx + x * scaledCell, scaledOy, rows * scaledCell,
                                  GAMER_WHITE);
  }
  for (uint8_t y = 0; y <= rows; y++) {
    engine.screen().drawFastHLine(scaledOx, scaledOy + y * scaledCell, cols * scaledCell,
                                  GAMER_WHITE);
  }
}
