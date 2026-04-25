#pragma once
#include <Arduino.h>
#include "MatrixManager.h"
#include "DisplayManager.h"

enum GameState {
    STATE_WAITING,
    STATE_PLAYING,
    STATE_GAMEOVER
};

class BreakoutGame {
public:
    BreakoutGame(MatrixManager* m, DisplayManager* d);
    void reset();
    void update(unsigned long currentMillis);
    
    // Actions from web UI
    void movePaddle(int direction); // -1 left, 1 right, 0 stop

    GameState getState() { return state; }
    int getScore() { return score; }
    int getLives() { return lives; }

private:
    MatrixManager* matrix;
    DisplayManager* display;

    unsigned long lastTick;
    
    GameState state;
    int score;
    int lives;

    // Paddle
    float paddleX; // 0 to 7
    int paddleWidth;
    int paddleDir;

    // Ball
    float ballX;
    float ballY;
    float ballVX;
    float ballVY;

    // Bricks (8x10 array)
    bool bricks[10][8];

    void draw();
    void checkCollisions();
    void nextLevel();
};
