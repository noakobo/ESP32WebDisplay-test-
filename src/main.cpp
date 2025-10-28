#define PRINT_FPS  // フレームレート見たくないときはコメントにする
#include <Arduino.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <WiFi.h>

#include "esp_camera.h"      // fmt2jpg()を使うため
#include "noaFrameBuffer.h"  // AIが考えてくれたフレームバッファ

WebServer server(80);
WebSocketsServer websocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload,
                    size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      websocket.disconnect();
      break;
    case WStype_CONNECTED: {
      IPAddress ip = websocket.remoteIP(num);
      websocket.sendTXT(num, "Connected");
    } break;
    case WStype_TEXT:
      break;
    case WStype_BIN:
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
      Serial.println("WStype_FRAGMENT_TEXT_START");
    case WStype_FRAGMENT_BIN_START:
      Serial.println("WStype_FRAGMENT_BIN_START");
    case WStype_FRAGMENT:
      Serial.println("WStype_FRAGMENT");
    case WStype_FRAGMENT_FIN:
      Serial.println("WStype_FRAGMENT_FIN");
      break;
  }
}

void indexHtml() {
  fs::File file = LittleFS.open("/index.html");
  String htmlDoc = file.readString();
  file.close();
  server.send(200, "text/html", htmlDoc);
}

void notFound() { server.send(404, "text/plain", "404 not found"); }

void setup() {
  Serial.begin(115200);
  delay(1000);
  if (psramFound()) {
    Serial.printf("psramFound(true)\n\n");
  } else {
    Serial.printf("psramFound(false)\n\n");
  }

  if (psramInit()) {
    Serial.printf("psramInit(true)\n");
    Serial.printf("Total heap: %d\n", ESP.getHeapSize());
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
    Serial.printf("\n");
  } else {
    Serial.printf("psramInit(false)\n\n");
  }

  if (LittleFS.begin(true) == false) {
    while (1) {
      Serial.printf("LitteFS fall\n\n");
      delay(1000);
      // ESP.restart();
      return;
    };
  } else {
    Serial.printf("LitteFS begin\n\n");
  }

  // アクセスポイント経由の時はこちら
  WiFi.begin("ssid", "pass");
  while (WiFi.status() != WL_CONNECTED) delay(100);
  Serial.printf("ip address:%s\n", WiFi.localIP().toString());

  // softAPの時はこちら
  // WiFi.softAP("esp32WebDisplay", "");
  // Serial.printf("ip address:%s\n", WiFi.softAPIP().toString());

  frameBuffer.setResolution(320, 240);  // フレームバッファの解像度を設定

  websocket.begin();
  websocket.onEvent(webSocketEvent);

  server.on("/", indexHtml);
  server.onNotFound(notFound);
  server.begin();
}

//********************************************************************************
/// @brief フレームバッファをjpegとしてクライアントへ送信
void jpegSend() {
  static unsigned long tm;
  uint8_t* jpegImage = nullptr;  // jpegイメージ
  size_t imageLen;               // jpegイメージのバイト数

#ifdef PRINT_FPS
  tm = millis();
#endif
  // フレームバッファをjpeg化して送信(fmt2jpgはRGB444をサポートしていません)
  if (fmt2jpg((uint8_t*)frameBuffer.getFrameBuffer(), frameBuffer.getSize(),
              320, 240, PIXFORMAT_RGB565, 80, &jpegImage, &imageLen)) {
    websocket.broadcastBIN(jpegImage, imageLen);
    free(jpegImage);
#ifdef PRINT_FPS
    log_i("tm=%ld ms", millis() - tm);
#endif
  }
}

void loop() {
  server.handleClient();
  websocket.loop();
  static int x = 0, y = 0;
  static int colorIndex = 0;

  x += 10;
  if (320 <= x) {
    x = 0;
    y += 10;
    if (240 <= y) {
      y = 0;
    }
  }

  if (7 < ++colorIndex) colorIndex = 0;
  frameBuffer.circleFill(x, y, 20, colorIndex);

  jpegSend();
  delay(1);
}
