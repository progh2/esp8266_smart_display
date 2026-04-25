#pragma once
#include <Arduino.h>

class WebServerManager {
public:
    WebServerManager();
    void begin();
    void update(); // call in loop (for WebSockets or clients handling)

private:
    // async server and websocket objects inside cpp
};
