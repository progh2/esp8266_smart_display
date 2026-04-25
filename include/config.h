#pragma once

// ==========================================
// PIN DEFINITIONS
// ==========================================
// OLED (I2C)
#define OLED_SDA_PIN D2 // GPIO4
#define OLED_SCL_PIN D1 // GPIO5

// Matrix (SPI)
#define MAX_DIN_PIN D7  // GPIO13 (MOSI)
#define MAX_CLK_PIN D5  // GPIO14 (SCK)
#define MAX_CS_PIN  D8  // GPIO15 (SS)

// ==========================================
// HARDWARE SETTINGS
// ==========================================
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

// Matrix Dimensions (Rotated 90 degrees)
// Original is 32x8, but we use it as 8x32
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 32

// ==========================================
// SYSTEM SETTINGS
// ==========================================
#define NTP_SERVER "pool.ntp.org"
#define NTP_OFFSET_SEC 32400 // KST (UTC+9)

// Display modes
enum DisplayMode {
    MODE_DASHBOARD = 0,
    MODE_GAME = 1
};
