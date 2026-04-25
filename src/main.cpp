#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "secrets.h"

// WiFi 설정
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// 웹 서버 설정
ESP8266WebServer server(80);

// NTP 시간 설정
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000);

// 1. OLED 설정
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 12, /* data=*/ 14, /* reset=*/ U8X8_PIN_NONE);

// 2. 매트릭스 설정
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // 4개 모듈 기준 (보통 1세트)
#define CS_PIN  4      // D2
#define DATA_PIN 13    // D7
#define CLK_PIN 5      // D1
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// 상태 변수
String currentMessage = "";
bool hasNewMessage = false;
unsigned long messageDisplayTime = 0;
const unsigned long MESSAGE_DURATION = 15000;

const unsigned long MODE_INTERVAL = 15000; 

int currentMode = 0; 
unsigned long lastModeChange = 0;

// 테트리스 설정
#define BOARD_WIDTH 8
#define BOARD_HEIGHT 32
const int TETRIS_MODE = 2;

// 날씨 데이터 변수
String weatherString = "업데이트 중...";
int weatherCode = 0;
unsigned long lastWeatherUpdate = 0;
const unsigned long WEATHER_INTERVAL = 600000; // 10분

// 유튜브 데이터 변수
const char* ytApiKey = YT_API_KEY;
const char* ytHandle = "@gihunham";
int ytSubscribers = 0;
unsigned long lastYouTubeUpdate = 0;
const unsigned long YOUTUBE_INTERVAL = 600000; // 10분

void fetchWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    
    String url = "https://api.open-meteo.com/v1/forecast?latitude=37.566&longitude=126.9784&current_weather=true";
    
    if (http.begin(client, url)) {
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
          float temp = doc["current_weather"]["temperature"];
          weatherCode = doc["current_weather"]["weathercode"];
          
          String desc = "";
          if (weatherCode == 0) desc = "맑음";
          else if (weatherCode >= 1 && weatherCode <= 3) desc = "구름/흐림";
          else if (weatherCode == 45 || weatherCode == 48) desc = "안개";
          else if (weatherCode >= 51 && weatherCode <= 67) desc = "비";
          else if (weatherCode >= 71 && weatherCode <= 77) desc = "눈";
          else if (weatherCode >= 80 && weatherCode <= 82) desc = "소나기";
          else if (weatherCode >= 95) desc = "뇌우";
          else desc = "기타";
          
          weatherString = desc + ", " + String(temp, 1) + "C";
        }
      }
      http.end();
    }
  }
}

void fetchYouTubeSubscribers() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); 
    HTTPClient http;
    
    String url = "https://www.googleapis.com/youtube/v3/channels?part=statistics&forHandle=";
    url += ytHandle;
    url += "&key=";
    url += ytApiKey;
    
    if (http.begin(client, url)) {
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
          if (doc["items"].size() > 0) {
            ytSubscribers = doc["items"][0]["statistics"]["subscriberCount"].as<int>();
          }
        }
      }
      http.end();
    }
  }
}

void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>미니 전광판 제어</title>";
  html += "<style>body{font-family:sans-serif;text-align:center;padding:20px;background:#f0f0f0;}";
  html += "input[type=text]{width:80%;padding:10px;font-size:18px;margin-bottom:10px; border-radius:5px; border:1px solid #ccc;}";
  html += "input[type=submit]{padding:10px 20px;font-size:18px;background:#007BFF;color:white;border:none;border-radius:5px; cursor:pointer;}";
  html += "</style></head><body>";
  html += "<h2>전광판 메시지 전송 🚀</h2>";
  html += "<form action='/send' method='POST'>";
  html += "<input type='text' name='message' placeholder='띄울 문구(영어/숫자)를 입력하세요' required><br>";
  html += "<input type='submit' value='전송하기'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleSend() {
  if (server.hasArg("message")) {
    currentMessage = server.arg("message");
    hasNewMessage = true;
    messageDisplayTime = millis();
    
    myDisplay.displayClear();
    myDisplay.displayText(currentMessage.c_str(), PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    
    String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<script>alert('메시지 전송 완료!'); window.location.href='/';</script></head><body></body></html>";
    server.send(200, "text/html", html);
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void updateOLED() {
  u8g2.clearBuffer();
  
  if (hasNewMessage) {
    u8g2.setFont(u8g2_font_unifont_t_korean1);
    u8g2.setCursor(5, 20);
    u8g2.print("수신된 메시지:");
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(5, 50);
    u8g2.print(currentMessage);
  } else {
    if (currentMode == 0) {
      // [시계 & 날씨 모드]
      timeClient.update();
      String formattedTime = timeClient.getFormattedTime();
      
      // 날씨 아이콘 선택
      char icon = '@'; // 기본값 구름
      if (weatherCode == 0) icon = 'E'; // 해
      else if (weatherCode >= 1 && weatherCode <= 3) icon = 'A'; // 구름+해
      else if (weatherCode >= 51 && weatherCode <= 67) icon = 'C'; // 비
      else if (weatherCode >= 71 && weatherCode <= 77) icon = 'G'; // 눈
      else if (weatherCode >= 80 && weatherCode <= 82) icon = 'C'; // 소나기
      else if (weatherCode >= 95) icon = 'F'; // 뇌우
      
      // 아이콘 그리기 (32x32)
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(5, 55, icon);
      
      // 시간 출력
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.setCursor(45, 28);
      u8g2.print(formattedTime);
      
      // 날씨 텍스트 출력
      u8g2.setFont(u8g2_font_unifont_t_korean1);
      u8g2.setCursor(45, 55);
      u8g2.print(weatherString);
      
    } else if (currentMode == 1) {
      u8g2.setFont(u8g2_font_unifont_t_korean1);
      u8g2.setCursor(10, 25);
      u8g2.print("gihunham 유튜브");
      u8g2.setCursor(10, 55);
      u8g2.print("구독자: " + String(ytSubscribers) + "명");
    }
  }
  u8g2.sendBuffer();
}

// --- 테트리스 엔진 시작 ---
const uint8_t PIECES[7][4][2] = {
  {{0,0}, {1,0}, {0,1}, {1,1}}, // O
  {{0,0}, {0,1}, {0,2}, {0,3}}, // I
  {{0,0}, {0,1}, {0,2}, {1,2}}, // L
  {{1,0}, {1,1}, {1,2}, {0,2}}, // J
  {{0,1}, {1,1}, {1,0}, {2,0}}, // S
  {{0,0}, {1,0}, {1,1}, {2,1}}, // Z
  {{0,0}, {1,0}, {2,0}, {1,1}}  // T
};

class TetrisGame {
public:
  uint8_t board[BOARD_WIDTH][BOARD_HEIGHT];
  int currentPiece;
  int pieceX, pieceY, rotation;
  int targetX, targetRotation;
  unsigned long lastDrop = 0;
  int score = 0;

  TetrisGame() { reset(); }

  void reset() {
    memset(board, 0, sizeof(board));
    score = 0;
    spawnPiece();
  }

  void spawnPiece() {
    currentPiece = random(0, 7);
    pieceX = BOARD_WIDTH / 2 - 1;
    pieceY = 0;
    rotation = 0;
    calculateBestMove();
    if (checkCollision(pieceX, pieceY, rotation)) reset();
  }

  bool checkCollision(int x, int y, int r, int p = -1) {
    if (p == -1) p = currentPiece;
    for (int i = 0; i < 4; i++) {
      int px = PIECES[p][i][0];
      int py = PIECES[p][i][1];
      // 회전 로직 (단순화)
      for(int j=0; j<r; j++) { int t = px; px = 1-py; py = t; }
      int nx = x + px;
      int ny = y + py;
      if (nx < 0 || nx >= BOARD_WIDTH || ny >= BOARD_HEIGHT) return true;
      if (ny >= 0 && board[nx][ny]) return true;
    }
    return false;
  }

  void calculateBestMove() {
    float bestScore = -1000000;
    targetX = pieceX;
    targetRotation = 0;

    for (int r = 0; r < 4; r++) {
      for (int x = -2; x < BOARD_WIDTH; x++) {
        int y = 0;
        while (!checkCollision(x, y + 1, r)) y++;
        if (checkCollision(x, y, r)) continue;

        float s = evaluateBoard(x, y, r);
        if (s > bestScore) {
          bestScore = s;
          targetX = x;
          targetRotation = r;
        }
      }
    }
  }

  float evaluateBoard(int x, int y, int r) {
    // 가상의 보드 시뮬레이션 및 점수 계산
    int lines = 0;
    int holes = 0;
    int aggregateHeight = 0;
    // ... (간략화된 휴리스틱)
    return -(y * -1.0) + (random(0, 100) / 100.0); // 임시: 낮게 쌓기 우선
  }

  void update() {
    if (millis() - lastDrop > 100) {
      if (rotation != targetRotation) rotation = (rotation + 1) % 4;
      else if (pieceX < targetX) pieceX++;
      else if (pieceX > targetX) pieceX--;
      else if (!checkCollision(pieceX, pieceY + 1, rotation)) pieceY++;
      else {
        lockPiece();
        spawnPiece();
      }
      lastDrop = millis();
    }
  }

  void lockPiece() {
    for (int i = 0; i < 4; i++) {
      int px = PIECES[currentPiece][i][0];
      int py = PIECES[currentPiece][i][1];
      for(int j=0; j<rotation; j++) { int t = px; px = 1-py; py = t; }
      if (pieceY + py >= 0) board[pieceX + px][pieceY + py] = 1;
    }
    clearLines();
  }

  void clearLines() {
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
      bool full = true;
      for (int x = 0; x < BOARD_WIDTH; x++) if (!board[x][y]) full = false;
      if (full) {
        score++;
        for (int ty = y; ty > 0; ty--)
          for (int tx = 0; tx < BOARD_WIDTH; tx++) board[tx][ty] = board[tx][ty - 1];
        y++;
      }
    }
  }

  void draw(MD_MAX72XX *mx) {
    mx->clear();
    for (int x = 0; x < BOARD_WIDTH; x++)
      for (int y = 0; y < BOARD_HEIGHT; y++)
        if (board[x][y]) mx->setPoint(x, 31 - y, true);

    for (int i = 0; i < 4; i++) {
      int px = PIECES[currentPiece][i][0];
      int py = PIECES[currentPiece][i][1];
      for(int j=0; j<rotation; j++) { int t = px; px = 1-py; py = t; }
      mx->setPoint(pieceX + px, 31 - (pieceY + py), true);
    }
  }
};

TetrisGame tetris;
// --- 테트리스 엔진 끝 ---

void setup() {
  Serial.begin(115200);
  
  u8g2.begin();
  u8g2.enableUTF8Print();
  myDisplay.begin();
  myDisplay.setIntensity(8); 
  myDisplay.displayClear();
  myDisplay.setTextAlignment(PA_CENTER);
  
  // 매트릭스 180도 회전 (뒤집기 설정)
  myDisplay.setZone(0, 0, MAX_DEVICES - 1);
  myDisplay.setZoneEffect(0, true, PA_FLIP_UD);
  myDisplay.setZoneEffect(0, true, PA_FLIP_LR);
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_korean1);
  u8g2.setCursor(10, 30);
  u8g2.print("WiFi 연결 중...");
  u8g2.sendBuffer();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  timeClient.begin();
  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  server.begin();
  
  // 첫 날씨 및 유튜브 가져오기
  u8g2.clearBuffer();
  u8g2.setCursor(10, 30);
  u8g2.print("데이터 동기화 중...");
  u8g2.sendBuffer();
  
  fetchWeather();
  lastWeatherUpdate = millis();
  
  fetchYouTubeSubscribers();
  lastYouTubeUpdate = millis();

  u8g2.clearBuffer();
  u8g2.setCursor(10, 25);
  u8g2.print("모든 준비 완료!");
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(10, 50);
  u8g2.print("IP: ");
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
  
  myDisplay.displayText(WiFi.localIP().toString().c_str(), PA_CENTER, 60, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  while(!myDisplay.displayAnimate()) { delay(10); }
  delay(1000); 
  myDisplay.displayClear();
}

void loop() {
  server.handleClient();
  
  if (millis() - lastWeatherUpdate > WEATHER_INTERVAL) {
    fetchWeather();
    lastWeatherUpdate = millis();
  }
  
  if (millis() - lastYouTubeUpdate > YOUTUBE_INTERVAL) {
    fetchYouTubeSubscribers();
    lastYouTubeUpdate = millis();
  }
  
  if (!hasNewMessage && (millis() - lastModeChange > MODE_INTERVAL)) {
    currentMode = (currentMode + 1) % 3; 
    lastModeChange = millis();
    myDisplay.displayClear();
    if (currentMode == TETRIS_MODE) tetris.reset();
  }

  if (currentMode == TETRIS_MODE) {
    tetris.update();
    tetris.draw(myDisplay.getGraphicObject());
  } else if (myDisplay.displayAnimate()) {
    if (hasNewMessage) {
      myDisplay.displayReset();
    } else {
      // 전역/정적 버퍼를 사용하여 메모리 누수 및 포인터 오류 방지
      static char matrixMsg[40];
      snprintf(matrixMsg, sizeof(matrixMsg), "YT: %d    ", ytSubscribers);
      
      myDisplay.displayText(matrixMsg, PA_CENTER, 60, 2000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      myDisplay.displayReset();
    }
  }

  if (hasNewMessage && (millis() - messageDisplayTime > MESSAGE_DURATION)) {
    hasNewMessage = false;
    myDisplay.displayClear();
  }

  static unsigned long lastOledUpdate = 0;
  if (millis() - lastOledUpdate > 500) {
    updateOLED();
    lastOledUpdate = millis();
  }
}
