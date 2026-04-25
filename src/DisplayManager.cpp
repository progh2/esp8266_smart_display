#include "DisplayManager.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "../include/config.h"

// Initialize U8g2 for I2C OLED
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, OLED_SCL_PIN, OLED_SDA_PIN, U8X8_PIN_NONE);

DisplayManager::DisplayManager() : lastUpdate(0) {
}

void DisplayManager::begin() {
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Booting...");
    u8g2.sendBuffer();
}

void DisplayManager::update(unsigned long currentMillis) {
    // Only used if animations or generic periodic updates are needed here
}

void DisplayManager::showDashboard(const char* timeStr, const char* ipStr) {
    u8g2.clearBuffer();
    
    // Header
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 15, "Smart Display");
    
    // Time
    u8g2.setFont(u8g2_font_logisoso24_tr);
    u8g2.drawStr(10, 48, timeStr);
    
    // IP Address
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 63, ipStr);
    
    u8g2.sendBuffer();
}

void DisplayManager::showGameInfo(int score, int lives, bool isGameOver) {
    u8g2.clearBuffer();
    
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 15, "BREAKOUT");
    
    u8g2.setFont(u8g2_font_ncenB08_tr);
    
    char scoreBuf[20];
    sprintf(scoreBuf, "Score: %d", score);
    u8g2.drawStr(0, 35, scoreBuf);
    
    char livesBuf[20];
    sprintf(livesBuf, "Lives: %d", lives);
    u8g2.drawStr(0, 50, livesBuf);
    
    if (isGameOver) {
        u8g2.drawStr(0, 64, "GAME OVER");
    } else {
        u8g2.drawStr(0, 64, "PLAYING...");
    }
    
    u8g2.sendBuffer();
}
