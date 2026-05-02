#include "Games.h"

namespace {
void drawRoad(GamerEngine& engine, int16_t shift) {
  engine.screen().drawLine(18 + shift / 4, 34, 110 + shift / 4, 34, SSD1306_WHITE);
  engine.screen().drawLine(46 + shift, 34, 16, 63, SSD1306_WHITE);
  engine.screen().drawLine(82 + shift, 34, 112, 63, SSD1306_WHITE);
  engine.screen().drawLine(62 + shift / 2, 38, 60, 63, SSD1306_WHITE);
  engine.screen().drawLine(66 + shift / 2, 38, 68, 63, SSD1306_WHITE);
}

void drawBike(GamerEngine& engine, int16_t x, int16_t y) {
  engine.screen().drawRect(x - 2, y - 6, 4, 3, SSD1306_WHITE);
  engine.screen().drawRect(x - 1, y - 2, 2, 4, SSD1306_WHITE);
  engine.screen().drawPixel(x, y - 8, SSD1306_WHITE);
}
}

void runFullSpeed(GamerEngine& engine) {
  while (true) {
    if (!engine.waitForSelectOrExit("FULL SPEED", "L/R steer", "SEL start")) return;

    int16_t playerX = 0;
    int16_t roadShift = 0;
    int8_t roadDir = 1;
    int16_t obstacleY = -20;
    int16_t obstacleX = 0;
    uint8_t speed = 1;
    uint16_t score = 0;
    uint32_t nextFrame = 0;

    while (true) {
      engine.tick();
      if (engine.shouldExitGame()) {
        engine.waitForRelease();
        return;
      }

      uint32_t now = millis();
      if (now < nextFrame) {
        delay(2);
        continue;
      }
      nextFrame = now + 70;

      if (engine.isHeld(BTN_LEFT)) playerX -= 3;
      if (engine.isHeld(BTN_RIGHT)) playerX += 3;

      roadShift += roadDir;
      if (roadShift <= -28 || roadShift >= 24) roadDir = -roadDir;

      obstacleY += speed + 1;
      if (obstacleY > 64) {
        obstacleY = -10;
        obstacleX = random(-34, 35);
        score++;
        engine.ledPulse(CRGB::Aqua, 70);
        if (score % 8 == 0 && speed < 5) speed++;
      }

      int16_t roadCenter = roadShift;
      if (roadShift < -12) playerX++;
      else if (roadShift > 12) playerX--;

      bool crash = playerX < -46 || playerX > 46;
      if (obstacleY > 42 && obstacleY < 62 && abs(playerX - obstacleX - roadCenter / 2) < 7) {
        crash = true;
      }

      engine.clear();
      engine.screen().setCursor(0, 0);
      engine.screen().print("S:");
      engine.screen().print(score);
      char speedText[8];
      snprintf(speedText, sizeof(speedText), "%uk", static_cast<unsigned>((score + 1) * 5));
      engine.rightText(speedText);
      drawRoad(engine, roadShift);
      drawBike(engine, 64 + playerX, 56);
      if (obstacleY > -8) drawBike(engine, 64 + obstacleX + roadCenter / 2, obstacleY);
      engine.show();

      if (crash) {
        engine.ledPulse(CRGB::Red, 300);
        char detail[18];
        snprintf(detail, sizeof(detail), "Score %u", score);
        if (!engine.showResult("CRASH", detail)) return;
        break;
      }
    }
  }
}

