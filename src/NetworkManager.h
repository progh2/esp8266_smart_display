#pragma once
#include <Arduino.h>

class NetworkManager {
public:
    NetworkManager();
    void begin();
    void update(unsigned long currentMillis);
    
    bool isConnected();
    const char* getIP();
    const char* getTimeString();

private:
    unsigned long lastTimeUpdate;
    char timeStr[10];
    char ipStr[20];
};
