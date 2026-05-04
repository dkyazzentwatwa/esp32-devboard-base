#pragma once

#include "GamerEngine.h"

bool selectTap(GamerEngine& engine);
bool runIntro(GamerEngine& engine, const char* title, const char* help);
bool resultScreen(GamerEngine& engine, const char* title, int16_t value);
bool rectsOverlap(int16_t ax, int16_t ay, int16_t aw, int16_t ah,
                  int16_t bx, int16_t by, int16_t bw, int16_t bh);
int8_t heldAxis(GamerEngine& engine);
void drawScore(GamerEngine& engine, int16_t value);
int16_t gameX(GamerEngine& engine, int16_t oledX);
int16_t gameY(GamerEngine& engine, int16_t oledY);
int16_t gameSize(GamerEngine& engine, int16_t oledSize);
void drawCursorBox(GamerEngine& engine, int16_t x, int16_t y, int16_t w, int16_t h);
void drawMiniGrid(GamerEngine& engine, uint8_t cols, uint8_t rows, uint8_t cell,
                  int16_t ox, int16_t oy);
