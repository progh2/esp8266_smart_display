# ESP8266 Smart Display 🚀

ESP8266 보드를 기반으로 한 다기능 IoT 스마트 디스플레이 프로젝트입니다. OLED 디스플레이와 8x8 도트 매트릭스를 활용하여 실시간 정보와 사용자 메시지를 표시합니다.

## 🌟 주요 기능

- **실시간 시계 및 날씨**: NTP 서버로부터 시간을 동기화하고, Open-Meteo API를 통해 현재 날씨 정보를 표시합니다.
- **YouTube 구독자 현황**: YouTube Data API v3를 연동하여 특정 채널의 구독자 수를 실시간으로 시각화합니다.
- **웹 서버 기반 메시지 전송**: 같은 네트워크상의 스마트폰이나 PC에서 웹 페이지에 접속하여 전광판에 즉시 메시지를 출력할 수 있습니다.
- **다이나믹 디스플레이**: OLED(SSD1306)에는 상세 정보를, MAX7219 도트 매트릭스에는 흐르는 텍스트를 표시합니다.

## 🛠 하드웨어 구성

- **Controller**: ESP8266 (NodeMCU 등)
- **Display 1**: SSD1306 OLED (128x64, I2C)
- **Display 2**: MAX7219 Dot Matrix (8x8 x 4 modules, SPI)

## 🚀 시작하기

### 1. 라이브러리 설치 (PlatformIO 권장)
- `U8g2`
- `MD_Parola`
- `MD_MAX72xx`
- `ArduinoJson`
- `NTPClient`

### 2. 비밀 정보 설정
`include/secrets.h` 파일을 생성하고 다음과 같이 작성하세요 (기본적으로 `.gitignore`에 포함되어 업로드되지 않습니다):

```cpp
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define YT_API_KEY "Your_YouTube_API_Key"

#endif
```

### 3. 빌드 및 업로드
PlatformIO를 사용하여 프로젝트를 빌드하고 보드에 업로드합니다.

## 📝 라이선스
MIT License
