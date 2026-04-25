#include "MatrixManager.h"
#include <SPI.h>
#include "../include/config.h"

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, MAX_DIN_PIN, MAX_CLK_PIN, MAX_CS_PIN, MAX_DEVICES);

MatrixManager::MatrixManager() : lastUpdate(0) {
}

void MatrixManager::begin() {
    mx.begin();
    mx.control(MD_MAX72XX::INTENSITY, 5); // Default brightness
    // Disable auto-update to avoid flickering during game renders
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    clear();
    update();
}

void MatrixManager::clear() {
    mx.clear();
}

void MatrixManager::update() {
    mx.update();
}

// 8x32 orientation handling. 
// Standard orientation is 32 wide, 8 high.
// We want 8 wide, 32 high. (0,0) at top-left.
void MatrixManager::drawPixel(int x, int y, bool state) {
    if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) return;
    
    // Rotate 90 degrees to make it vertical
    // Hardware is 32 wide, 8 high.
    // (x, y) where x: 0~7, y: 0~31
    // hardware X (col) = y
    // hardware Y (row) = x (might need inversion depending on module)
    // Let's assume FC16_HW orientation. Top-left of 8x32 should map to Top-right of 32x8 if rotated clockwise
    
    int hwCol = y;
    int hwRow = MATRIX_WIDTH - 1 - x;
    
    mx.setPoint(hwRow, hwCol, state);
}

void MatrixManager::testPattern(unsigned long currentMillis) {
    // Simple test animation to check orientation
    if (currentMillis - lastUpdate > 100) {
        lastUpdate = currentMillis;
        clear();
        static int y = 0;
        drawPixel(3, y, true);
        drawPixel(4, y, true);
        y++;
        if (y >= MATRIX_HEIGHT) y = 0;
        update();
    }
}
