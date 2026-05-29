#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ctype.h>

#include "config.h"

WebSocketsClient webSocket;

static unsigned long lastHelloAt = 0;
static unsigned long helloCounter = 0;
static unsigned long lastWifiAttemptAt = 0;

String urlEncode(const char *value) {
  const char *hex = "0123456789ABCDEF";
  String encoded;

  while (*value) {
    unsigned char c = static_cast<unsigned char>(*value++);
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += static_cast<char>(c);
    } else {
      encoded += '%';
      encoded += hex[(c >> 4) & 0x0F];
      encoded += hex[c & 0x0F];
    }
  }

  return encoded;
}

String websocketPath() {
  String path = WS_PATH;
  path += "?token=";
  path += urlEncode(WS_TOKEN);
  path += "&device_id=";
  path += urlEncode(DEVICE_ID);
  return path;
}

void connectWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.printf("Connecting WiFi SSID=%s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAt < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected, IP=");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connect timeout; will retry");
  }
}

void setupWebSocket() {
  String path = websocketPath();

  Serial.printf("Connecting WebSocket %s://%s:%u%s\n",
                WS_USE_SSL ? "wss" : "ws", WS_HOST, WS_PORT, path.c_str());

  if (WS_USE_SSL) {
    webSocket.beginSSL(WS_HOST, WS_PORT, path.c_str());
  } else {
    webSocket.begin(WS_HOST, WS_PORT, path.c_str());
  }

  webSocket.onEvent([](WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
      case WStype_DISCONNECTED:
        Serial.println("WebSocket disconnected");
        break;
      case WStype_CONNECTED:
        Serial.printf("WebSocket connected: %s\n", payload);
        break;
      case WStype_TEXT:
        Serial.printf("WebSocket text received (%u bytes): %s\n",
                      static_cast<unsigned>(length), payload);
        break;
      case WStype_BIN:
        Serial.printf("WebSocket binary received: %u bytes\n",
                      static_cast<unsigned>(length));
        break;
      case WStype_PING:
        Serial.println("WebSocket ping");
        break;
      case WStype_PONG:
        Serial.println("WebSocket pong");
        break;
      case WStype_ERROR:
        Serial.println("WebSocket error");
        break;
      default:
        break;
    }
  });

  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);
}

void sendHelloIfDue() {
  if (millis() - lastHelloAt < HELLO_INTERVAL_MS) {
    return;
  }

  lastHelloAt = millis();
  helloCounter++;

  String message = "hello from esp32 ";
  message += DEVICE_ID;
  message += " #";
  message += helloCounter;
  message += " uptime_ms=";
  message += millis();

  if (webSocket.sendTXT(message)) {
    Serial.print("Sent: ");
    Serial.println(message);
  } else {
    Serial.println("Send skipped: WebSocket is not connected");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ESP32-S3 AI voice phase 1 WebSocket echo test");
  Serial.printf("Device ID: %s\n", DEVICE_ID);

  connectWifi();
  setupWebSocket();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastWifiAttemptAt > 10000) {
      lastWifiAttemptAt = millis();
      connectWifi();
    }
    delay(50);
    return;
  }

  webSocket.loop();
  sendHelloIfDue();
}
