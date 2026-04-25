#include "WebServerManager.h"
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Global variable to hold paddle direction (read by main loop)
// 0: stop, -1: left, 1: right
int paddleAction = 0;
// Global variable for mode
int currentMode = 0; // 0: dashboard, 1: game

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            }
            break;
        case WStype_TEXT:
            {
                // Parse JSON
                StaticJsonDocument<200> doc;
                DeserializationError error = deserializeJson(doc, payload);
                if (!error) {
                    const char* action = doc["action"];
                    if (action) {
                        if (strcmp(action, "move") == 0) {
                            const char* dir = doc["dir"];
                            const char* state = doc["state"]; // "down" or "up"
                            
                            if (strcmp(state, "down") == 0) {
                                if (strcmp(dir, "left") == 0) paddleAction = -1;
                                else if (strcmp(dir, "right") == 0) paddleAction = 1;
                            } else if (strcmp(state, "up") == 0) {
                                paddleAction = 0;
                            }
                        } else if (strcmp(action, "mode") == 0) {
                            currentMode = doc["mode"].as<int>();
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
}

WebServerManager::WebServerManager() {
}

void WebServerManager::begin() {
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    
    // Serve static files
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    
    // Start server
    server.begin();
    
    // Start WebSockets
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void WebServerManager::update() {
    webSocket.loop();
}
