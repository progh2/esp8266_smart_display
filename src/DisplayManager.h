#pragma once
#include <Arduino.h>

class DisplayManager {
public:
    DisplayManager();
    void begin();
    void update(unsigned long currentMillis);
    
    void showDashboard(const char* timeStr, const char* ipStr);
    void showGameInfo(int score, int lives, bool isGameOver);

private:
    unsigned long lastUpdate;
    // U8g2 object will be defined in the cpp
};
