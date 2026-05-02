#include "Games.h"
#include "GameUtils.h"

namespace {
struct ShotState {
  int16_t x;
  int16_t y;
  bool active;
};

bool frameReady(uint32_t& nextFrame, uint16_t frameMs) {
  uint32_t now = millis();
  if (now < nextFrame) return false;
  nextFrame = now + frameMs;
  return true;
}

void drawTarget(GamerEngine& engine, int16_t x, int16_t y, uint8_t style) {
  if (style == 0) {
    engine.screen().drawCircle(x + 4, y + 4, 4, SSD1306_WHITE);
  } else if (style == 1) {
    engine.screen().drawRect(x, y, 10, 7, SSD1306_WHITE);
    engine.screen().drawFastHLine(x + 2, y + 7, 6, SSD1306_WHITE);
  } else if (style == 2) {
    engine.screen().drawTriangle(x + 5, y, x, y + 8, x + 10, y + 8, SSD1306_WHITE);
  } else {
    engine.screen().fillRect(x, y, 8, 8, SSD1306_WHITE);
  }
}

void runTurretPattern(GamerEngine& engine, const char* title, uint8_t style, bool sideTravel, bool manyTargets) {
  while (true) {
    if (!runIntro(engine, title, "L/R aim SEL fire")) return;
    int16_t turretX = 60;
    int16_t targetX[3] = {20, 70, 110};
    int16_t targetY[3] = {-10, -32, -54};
    int8_t targetDx[3] = {1, -1, 1};
    ShotState shot = {0, 0, false};
    uint16_t score = 0;
    uint8_t lives = 3;
    uint32_t nextFrame = 0;

    while (true) {
      engine.tick();
      if (engine.shouldExitGame()) {
        engine.waitForRelease();
        return;
      }
      turretX += heldAxis(engine) * 4;
      turretX = constrain(turretX, 0, 120);
      if (selectTap(engine) && !shot.active) {
        shot = {static_cast<int16_t>(turretX + 4), 52, true};
        engine.ledPulse(CRGB::Blue, 35);
      }
      if (!frameReady(nextFrame, 34)) {
        delay(2);
        continue;
      }

      if (shot.active) {
        shot.y -= 6;
        if (shot.y < 0) shot.active = false;
      }

      uint8_t count = manyTargets ? 3 : 1;
      for (uint8_t i = 0; i < count; i++) {
        targetY[i] += 2 + score / 18;
        if (sideTravel) {
          targetX[i] += targetDx[i];
          if (targetX[i] < 0 || targetX[i] > 118) targetDx[i] = -targetDx[i];
        }
        if (targetY[i] > 64) {
          targetY[i] = -random(10, 45);
          targetX[i] = random(4, 116);
          if (lives > 0) lives--;
          engine.ledPulse(CRGB::Red, 90);
        }
        if (shot.active && rectsOverlap(shot.x, shot.y, 2, 7, targetX[i], targetY[i], 10, 10)) {
          shot.active = false;
          targetY[i] = -random(12, 60);
          targetX[i] = random(4, 116);
          score++;
          engine.ledPulse(CRGB::Green, 65);
        }
      }

      engine.clear();
      engine.screen().drawRect(turretX, 56, 10, 6, SSD1306_WHITE);
      engine.screen().drawFastVLine(turretX + 5, 50, 7, SSD1306_WHITE);
      if (shot.active) engine.screen().fillRect(shot.x, shot.y, 2, 7, SSD1306_WHITE);
      for (uint8_t i = 0; i < count; i++) drawTarget(engine, targetX[i], targetY[i], style);
      engine.screen().setCursor(0, 0);
      engine.screen().print("L:");
      engine.screen().print(lives);
      drawScore(engine, score);
      engine.show();

      if (lives == 0) {
        engine.ledPulse(CRGB::Red, 280);
        if (!resultScreen(engine, "GAME OVER", score)) return;
        break;
      }
    }
  }
}

void runMissilePattern(GamerEngine& engine, const char* title, bool wideBlast) {
  while (true) {
    if (!runIntro(engine, title, "L/R site SEL")) return;
    int16_t siteX = 60;
    int16_t threatX = random(8, 120);
    int16_t threatY = -8;
    int16_t blastX = -20;
    int16_t blastY = -20;
    uint8_t blastLife = 0;
    uint16_t score = 0;
    uint8_t base = 5;
    uint32_t nextFrame = 0;

    while (true) {
      engine.tick();
      if (engine.shouldExitGame()) {
        engine.waitForRelease();
        return;
      }
      siteX += heldAxis(engine) * 4;
      siteX = constrain(siteX, 0, 124);
      if (selectTap(engine) && blastLife == 0) {
        blastX = siteX;
        blastY = 36;
        blastLife = wideBlast ? 10 : 6;
        engine.ledPulse(CRGB::Aqua, 50);
      }
      if (!frameReady(nextFrame, 42)) {
        delay(2);
        continue;
      }
      threatY += 2 + score / 16;
      if (blastLife > 0) blastLife--;
      int16_t radius = wideBlast ? 12 : 8;
      if (blastLife > 0 && abs(threatX - blastX) < radius && abs(threatY - blastY) < radius) {
        threatY = -random(10, 40);
        threatX = random(8, 120);
        score++;
        engine.ledPulse(CRGB::Green, 70);
      }
      if (threatY > 62) {
        threatY = -random(10, 40);
        threatX = random(8, 120);
        if (base > 0) base--;
        engine.ledPulse(CRGB::Red, 90);
      }

      engine.clear();
      engine.screen().drawFastHLine(0, 63, SCREEN_WIDTH, SSD1306_WHITE);
      engine.screen().drawFastVLine(siteX, 50, 10, SSD1306_WHITE);
      engine.screen().drawLine(siteX - 3, 56, siteX + 3, 56, SSD1306_WHITE);
      engine.screen().fillRect(threatX, threatY, 4, 8, SSD1306_WHITE);
      if (blastLife > 0) engine.screen().drawCircle(blastX, blastY, radius, SSD1306_WHITE);
      engine.screen().setCursor(0, 0);
      engine.screen().print("B:");
      engine.screen().print(base);
      drawScore(engine, score);
      engine.show();

      if (base == 0) {
        engine.ledPulse(CRGB::Red, 260);
        if (!resultScreen(engine, "BASE LOST", score)) return;
        break;
      }
    }
  }
}
}

void runAsteroidsLite(GamerEngine& engine) { runTurretPattern(engine, "ASTEROIDS", 0, true, true); }
void runInvadersLite(GamerEngine& engine) { runTurretPattern(engine, "INVADERS", 1, false, true); }
void runMissileCommandLite(GamerEngine& engine) { runMissilePattern(engine, "MISSILE CMD", true); }
void runTurretDefense(GamerEngine& engine) { runTurretPattern(engine, "TURRET DEF", 2, false, false); }
void runUfoDefender(GamerEngine& engine) { runTurretPattern(engine, "UFO DEFENDER", 1, true, false); }
void runMeteorBlaster(GamerEngine& engine) { runMissilePattern(engine, "METEOR BLAST", false); }

