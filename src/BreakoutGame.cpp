#include "BreakoutGame.h"
#include <Arduino.h>

BreakoutGame::BreakoutGame(MatrixManager* m, DisplayManager* d) 
    : matrix(m), display(d), lastTick(0) {
    reset();
}

void BreakoutGame::reset() {
    state = STATE_WAITING;
    score = 0;
    lives = 3;
    paddleWidth = 3;
    nextLevel();
}

void BreakoutGame::nextLevel() {
    paddleX = 2.5; // Center
    paddleDir = 0;
    
    ballX = 4.0;
    ballY = 28.0;
    // Initial velocity
    ballVX = (random(0, 2) == 0 ? -0.5 : 0.5);
    ballVY = -0.6; // Upwards
    
    // Initialize bricks
    for(int r = 0; r < 10; r++) {
        for(int c = 0; c < 8; c++) {
            // Leave some gap at the top and scatter a bit
            if (r > 1 && r < 8 && (random(0, 10) > 2)) {
                bricks[r][c] = true;
            } else {
                bricks[r][c] = false;
            }
        }
    }
}

void BreakoutGame::movePaddle(int direction) {
    paddleDir = direction;
}

void BreakoutGame::update(unsigned long currentMillis) {
    // Game loop tick every 50ms
    if (currentMillis - lastTick < 50) return;
    lastTick = currentMillis;

    if (state == STATE_WAITING) {
        // Start game logic
        state = STATE_PLAYING;
    }

    if (state == STATE_PLAYING) {
        // Move Paddle
        paddleX += paddleDir * 0.8;
        if (paddleX < 0) paddleX = 0;
        if (paddleX > MATRIX_WIDTH - paddleWidth) paddleX = MATRIX_WIDTH - paddleWidth;

        // Move Ball
        ballX += ballVX;
        ballY += ballVY;

        checkCollisions();

        // Ball fell out
        if (ballY >= MATRIX_HEIGHT) {
            lives--;
            if (lives <= 0) {
                state = STATE_GAMEOVER;
            } else {
                // Reset ball
                ballX = paddleX + paddleWidth / 2.0;
                ballY = 28.0;
                ballVX = (random(0, 2) == 0 ? -0.5 : 0.5);
                ballVY = -0.6;
            }
        }
    }

    draw();
}

void BreakoutGame::checkCollisions() {
    // Wall collisions
    if (ballX <= 0) { ballX = 0; ballVX = -ballVX; }
    if (ballX >= MATRIX_WIDTH - 1) { ballX = MATRIX_WIDTH - 1; ballVX = -ballVX; }
    if (ballY <= 0) { ballY = 0; ballVY = -ballVY; }

    // Paddle collision
    // Paddle is at y = 31
    if (ballY >= 30.0 && ballY <= 31.0 && ballVY > 0) {
        if (ballX >= paddleX - 0.5 && ballX <= paddleX + paddleWidth + 0.5) {
            ballVY = -ballVY;
            // Adjust angle based on where it hit the paddle
            float hitPos = (ballX - paddleX) / paddleWidth; // 0.0 to 1.0
            ballVX = (hitPos - 0.5) * 1.5; 
            ballY = 29.5;
        }
    }

    // Brick collisions
    int bx = (int)(ballX + 0.5);
    int by = (int)(ballY + 0.5);

    if (by >= 0 && by < 10 && bx >= 0 && bx < 8) {
        if (bricks[by][bx]) {
            bricks[by][bx] = false; // Break it
            ballVY = -ballVY;
            score += 10;
        }
    }
}

void BreakoutGame::draw() {
    matrix->clear();

    // Draw Bricks
    for(int r = 0; r < 10; r++) {
        for(int c = 0; c < 8; c++) {
            if (bricks[r][c]) {
                matrix->drawPixel(c, r, true);
            }
        }
    }

    // Draw Paddle
    int px = (int)paddleX;
    for (int i = 0; i < paddleWidth; i++) {
        if (px + i < MATRIX_WIDTH) {
            matrix->drawPixel(px + i, 31, true);
        }
    }

    // Draw Ball
    if (state == STATE_PLAYING) {
        matrix->drawPixel((int)(ballX + 0.5), (int)(ballY + 0.5), true);
    } else if (state == STATE_GAMEOVER) {
        // Simple game over pattern (X)
        for (int i = 0; i < 8; i++) {
            matrix->drawPixel(i, i + 12, true);
            matrix->drawPixel(7 - i, i + 12, true);
        }
    }

    matrix->update();
}
