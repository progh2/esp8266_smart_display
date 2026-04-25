#pragma once
#include <Arduino.h>
#include <MD_MAX72xx.h>

class MatrixManager {
public:
    MatrixManager();
    void begin();
    void clear();
    void update();
    void drawPixel(int x, int y, bool state);
    
    // Test animation
    void testPattern(unsigned long currentMillis);

private:
    unsigned long lastUpdate;
    // Helper to rotate coordinates
    void setLedRotated(int x, int y, bool state);
};
