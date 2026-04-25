#include "NetworkManager.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "../include/config.h"
#include "../include/secrets.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET_SEC, 60000);

NetworkManager::NetworkManager() : lastTimeUpdate(0) {
    strcpy(timeStr, "--:--");
    strcpy(ipStr, "0.0.0.0");
}

void NetworkManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    // Connecting is handled asynchronously in update(), but we can do a brief wait here
    Serial.print("Connecting to WiFi");
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
        delay(500);
        Serial.print(".");
        retries++;
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Connected! IP address: ");
        Serial.println(WiFi.localIP());
        String ip = WiFi.localIP().toString();
        strncpy(ipStr, ip.c_str(), sizeof(ipStr) - 1);
        
        timeClient.begin();
    }
}

void NetworkManager::update(unsigned long currentMillis) {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();
        
        if (currentMillis - lastTimeUpdate > 1000) {
            lastTimeUpdate = currentMillis;
            int hours = timeClient.getHours();
            int minutes = timeClient.getMinutes();
            sprintf(timeStr, "%02d:%02d", hours, minutes);
        }
    } else {
        // Reconnect logic could be added here if non-blocking is strict
    }
}

bool NetworkManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

const char* NetworkManager::getIP() {
    return ipStr;
}

const char* NetworkManager::getTimeString() {
    return timeStr;
}
