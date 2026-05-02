#include "Games.h"

namespace {
void drawLander(GamerEngine& engine, int16_t x, int16_t y, bool thrust) {
  engine.screen().drawRect(x + 2, y + 2, 6, 5, SSD1306_WHITE);
  engine.screen().drawRect(x + 3, y, 4, 3, SSD1306_WHITE);
  engine.screen().drawFastVLine(x + 1, y + 5, 5, SSD1306_WHITE);
  engine.screen().drawFastVLine(x + 8, y + 5, 5, SSD1306_WHITE);
  if (thrust) {
    engine.screen().drawFastVLine(x + 5, y + 8, 7, SSD1306_WHITE);
  }
}
}

void runLunarModule(GamerEngine& engine) {
  while (true) {
    if (!engine.waitForSelectOrExit("LUNAR MODULE", "L/R steer", "SEL thrust")) return;

    uint8_t level = 1;
    int16_t x = 8;
    int16_t y = 2;
    int8_t vx = 1;
    int8_t vy = 0;
    int8_t fuel = 30;
    bool thrustFlash = false;
    uint32_t thrustUntil = 0;
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
      nextFrame = now + 90;

      if (engine.isHeld(BTN_LEFT) && fuel > 0) {
        vx--;
        fuel--;
      }
      if (engine.isHeld(BTN_RIGHT) && fuel > 0) {
        vx++;
        fuel--;
      }
      if (engine.wasPressed(BTN_SELECT) && fuel > 0) {
        vy -= 2;
        fuel--;
        thrustFlash = true;
        thrustUntil = now + 140;
        engine.ledPulse(CRGB::Orange, 90);
      }

      vy++;
      x += vx;
      y += vy / 2;

      if (x < 0) {
        x = 0;
        vx = 1;
      }
      if (x > 120) {
        x = 120;
        vx = -1;
      }
      if (fuel < 0) fuel = 0;
      if (thrustFlash && static_cast<int32_t>(now - thrustUntil) >= 0) thrustFlash = false;

      engine.clear();
      engine.screen().setCursor(0, 0);
      engine.screen().print("F:");
      engine.screen().print(fuel);
      char velocity[8];
      snprintf(velocity, sizeof(velocity), "V:%d", vy);
      engine.rightText(velocity);
      engine.screen().drawRect(94, 62, 22, 2, SSD1306_WHITE);
      drawLander(engine, x, y, thrustFlash);
      engine.show();

      if (y >= 52) {
        bool landed = x >= 92 && x <= 112 && abs(vx) <= 3 && vy <= 6;
        if (landed) {
          engine.ledPulse(CRGB::Green, 450);
          engine.clear();
          engine.centerText("LANDING OK", 18);
          char levelText[18];
          snprintf(levelText, sizeof(levelText), "Level %u", static_cast<unsigned>(level + 1));
          engine.centerText(levelText, 36);
          engine.show();
          delay(900);
          level++;
          x = 8;
          y = 2;
          vx = level;
          vy = 0;
          fuel = (32 - level) > 18 ? 32 - level : 18;
        } else {
          engine.ledPulse(CRGB::Red, 300);
          char detail[18];
          snprintf(detail, sizeof(detail), "Level %u", static_cast<unsigned>(level));
          if (!engine.showResult("CRASH", detail)) return;
          break;
        }
      }
    }
  }
}
