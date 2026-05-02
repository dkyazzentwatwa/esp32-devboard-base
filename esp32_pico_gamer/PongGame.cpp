#include "Games.h"

void runPong(GamerEngine& engine) {
  const int8_t ballSize = 4;
  const int8_t paddleWidth = 18;
  const int8_t paddleHeight = 4;
  const int8_t paddleY = SCREEN_HEIGHT - 10;
  const int8_t paddleSpeed = 4;

  while (true) {
    if (!engine.waitForSelectOrExit("PONG", "L/R move paddle", "SEL start")) return;

    int16_t ballX = 64;
    int16_t ballY = 16;
    int8_t ballVx = random(0, 2) == 0 ? -2 : 2;
    int8_t ballVy = 2;
    int16_t paddleX = (SCREEN_WIDTH - paddleWidth) / 2;
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
      nextFrame = now + 24;

      if (engine.isHeld(BTN_LEFT)) paddleX -= paddleSpeed;
      if (engine.isHeld(BTN_RIGHT)) paddleX += paddleSpeed;
      paddleX = constrain(paddleX, 0, SCREEN_WIDTH - paddleWidth);

      ballX += ballVx;
      ballY += ballVy;

      bool collision = false;
      if (ballX <= 0) {
        ballX = 0;
        ballVx = -ballVx;
        collision = true;
      }
      if (ballX + ballSize >= SCREEN_WIDTH) {
        ballX = SCREEN_WIDTH - ballSize;
        ballVx = -ballVx;
        collision = true;
      }
      if (ballY <= 0) {
        ballY = 0;
        ballVy = -ballVy;
        collision = true;
      }
      if (ballY + ballSize >= paddleY && ballX + ballSize >= paddleX && ballX <= paddleX + paddleWidth) {
        ballY = paddleY - ballSize;
        ballVy = -abs(ballVy);
        int16_t offset = ballX - (paddleX + paddleWidth / 2);
        int8_t nextVx = ballVx + offset / 8;
        if (nextVx < -4) nextVx = -4;
        if (nextVx > 4) nextVx = 4;
        ballVx = nextVx;
        if (ballVx == 0) ballVx = random(0, 2) == 0 ? -1 : 1;
        score += 10;
        collision = true;
      }

      if (ballY + ballSize > SCREEN_HEIGHT) {
        engine.ledPulse(CRGB::Red, 280);
        char detail[18];
        snprintf(detail, sizeof(detail), "Score %u", score);
        if (!engine.showResult("GAME OVER", detail)) return;
        break;
      }

      if (collision) engine.ledPulse(CRGB::Blue, 45);

      engine.clear();
      engine.screen().fillRect(paddleX, paddleY, paddleWidth, paddleHeight, SSD1306_WHITE);
      engine.screen().fillRect(ballX, ballY, ballSize, ballSize, SSD1306_WHITE);
      char scoreText[8];
      snprintf(scoreText, sizeof(scoreText), "%u", score);
      engine.rightText(scoreText);
      engine.show();
    }
  }
}
