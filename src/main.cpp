#include <Arduino.h>
#include "include/config.h"
#include "DisplayManager.h"
#include "MatrixManager.h"
#include "NetworkManager.h"
#include "WebServerManager.h"
#include "BreakoutGame.h"

// Global instances
DisplayManager displayMgr;
MatrixManager matrixMgr;
NetworkManager netMgr;
WebServerManager webMgr;

BreakoutGame* game;

// Externs from WebServerManager
extern int paddleAction;
extern int currentMode;

unsigned long lastDisplayUpdate = 0;

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("\nStarting Smart Retro Display...");

    // Initialize hardware
    displayMgr.begin();
    matrixMgr.begin();
    
    // Initialize network
    netMgr.begin();
    
    // Initialize web server
    webMgr.begin();

    // Initialize game
    game = new BreakoutGame(&matrixMgr, &displayMgr);

    Serial.println("Setup completed.");
}

void loop() {
    unsigned long currentMillis = millis();

    // 1. Network & Web updates (Non-blocking)
    netMgr.update(currentMillis);
    webMgr.update();

    // 2. Handle Game Controller Input
    if (currentMode == MODE_GAME) {
        game->movePaddle(paddleAction);
        game->update(currentMillis);
    }

    // 3. Update OLED Display (every 500ms)
    if (currentMillis - lastDisplayUpdate > 500) {
        lastDisplayUpdate = currentMillis;
        
        if (currentMode == MODE_DASHBOARD) {
            // Test pattern on matrix when in dashboard mode
            matrixMgr.testPattern(currentMillis);
            
            // Show Dashboard on OLED
            displayMgr.showDashboard(netMgr.getTimeString(), netMgr.getIP());
            
        } else if (currentMode == MODE_GAME) {
            // Show Game status on OLED
            displayMgr.showGameInfo(game->getScore(), game->getLives(), game->getState() == STATE_GAMEOVER);
        }
    }
}
