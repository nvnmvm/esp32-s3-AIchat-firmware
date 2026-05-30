#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <U8g2lib.h>
#include <ctype.h>
#include <driver/i2s.h>
#include <string.h>
#include <time.h>

#include "../include/config.h"
#include "../include/pins.h"

#ifndef I2S_COMM_FORMAT_STAND_I2S
#define I2S_COMM_FORMAT_STAND_I2S I2S_COMM_FORMAT_I2S
#endif

WebSocketsClient webSocket;
HardwareSerial AsrSerial(1);
U8G2_SH1106_128X64_NONAME_F_SW_I2C display(U8G2_R0, PIN_OLED_SCL, PIN_OLED_SDA, U8X8_PIN_NONE);

enum class DeviceState {
  Idle,
  Recording,
  Processing,
  Playing,
};

static DeviceState state = DeviceState::Idle;
static bool websocketConnected = false;
static bool displayReady = false;
static bool expectingAudio = false;
static unsigned long lastWifiAttemptAt = 0;
static unsigned long recordingStartedAt = 0;
static unsigned long lastClockDisplayAt = 0;
static String asrLine;

static const char *NTP_SERVER_1 = "ntp.aliyun.com";
static const char *NTP_SERVER_2 = "pool.ntp.org";
static const char *SHANGHAI_TZ = "CST-8";
static const size_t AUDIO_CHUNK_BYTES = (AUDIO_SAMPLE_RATE * 2 * AUDIO_CHUNK_MS) / 1000;
static uint8_t audioChunk[AUDIO_CHUNK_BYTES];

bool isPlaceholder(const char *value) {
  return strstr(value, "YOUR_") != nullptr ||
         strstr(value, "CHANGE_ME") != nullptr ||
         strlen(value) == 0;
}

bool validateConfig() {
  bool ok = true;

  if (isPlaceholder(WIFI_SSID)) {
    Serial.println("Config error: WIFI_SSID is not configured.");
    ok = false;
  }

  if (isPlaceholder(WIFI_PASSWORD)) {
    Serial.println("Config error: WIFI_PASSWORD is not configured.");
    ok = false;
  }

  if (isPlaceholder(WS_HOST)) {
    Serial.println("Config error: WS_HOST is not configured.");
    ok = false;
  }

  if (isPlaceholder(WS_TOKEN)) {
    Serial.println("Config error: WS_TOKEN is not configured.");
    ok = false;
  }

  if (WS_PORT == 0) {
    Serial.println("Config error: WS_PORT cannot be 0.");
    ok = false;
  }

  return ok;
}

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

void drawDisplayLine(uint8_t y, const String &text) {
  String clipped = text;
  clipped.replace("\r", " ");
  clipped.replace("\n", " ");

  while (clipped.length() > 0 && display.getUTF8Width(clipped.c_str()) > OLED_WIDTH) {
    int lastByte = clipped.length() - 1;
    while (lastByte > 0 && (static_cast<uint8_t>(clipped[lastByte]) & 0xC0) == 0x80) {
      lastByte--;
    }
    clipped.remove(lastByte);
  }

  display.drawUTF8(0, y, clipped.c_str());
}

void showScreen(const String &line1, const String &line2 = "", const String &line3 = "") {
  Serial.printf("[screen] %s | %s | %s\n", line1.c_str(), line2.c_str(), line3.c_str());

  if (!displayReady) {
    return;
  }

  display.clearBuffer();
  display.setFont(u8g2_font_wqy12_t_gb2312);
  drawDisplayLine(13, line1);
  if (line2.length() > 0) {
    drawDisplayLine(34, line2);
  }
  if (line3.length() > 0) {
    drawDisplayLine(55, line3);
  }
  display.sendBuffer();
}

String shanghaiTimeText() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 50)) {
    return "上海时间同步中";
  }

  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  return String("上海时间 ") + buffer;
}

void showShanghaiTime() {
  lastClockDisplayAt = millis();
  showScreen("等待唤醒", shanghaiTimeText(), "说 小一小一");
}

void sendJson(const char *type) {
  JsonDocument doc;
  doc["type"] = type;
  String payload;
  serializeJson(doc, payload);
  webSocket.sendTXT(payload);
}

void setupDisplay() {
  if (!OLED_ENABLED) {
    return;
  }

  display.setI2CAddress(OLED_I2C_ADDR << 1);
  displayReady = display.begin();
  if (!displayReady) {
    Serial.println("OLED init failed. Expected SH1106 U8g2 SW I2C address=0x3C SDA=GPIO8 SCL=GPIO9.");
    return;
  }

  display.setPowerSave(0);
  display.setContrast(180);
  showScreen("阶段二", "屏幕初始化完成", "SH1106 0x3C");
}

void setupI2sMic() {
  i2s_config_t config = {};
  config.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX);
  config.sample_rate = AUDIO_SAMPLE_RATE;
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  config.dma_buf_count = 4;
  config.dma_buf_len = 512;
  config.use_apll = false;
  config.tx_desc_auto_clear = false;
  config.fixed_mclk = 0;

  i2s_pin_config_t pins = {};
  pins.bck_io_num = PIN_MIC_BCLK;
  pins.ws_io_num = PIN_MIC_WS;
  pins.data_out_num = I2S_PIN_NO_CHANGE;
  pins.data_in_num = PIN_MIC_SD;

  esp_err_t err = i2s_driver_install(I2S_NUM_0, &config, 0, nullptr);
  if (err != ESP_OK) {
    Serial.printf("I2S mic driver install failed: %d\n", err);
    return;
  }

  err = i2s_set_pin(I2S_NUM_0, &pins);
  if (err != ESP_OK) {
    Serial.printf("I2S mic pin setup failed: %d\n", err);
    return;
  }

  i2s_zero_dma_buffer(I2S_NUM_0);
}

void setupI2sSpeaker() {
  pinMode(PIN_SPK_SD, OUTPUT);
  digitalWrite(PIN_SPK_SD, HIGH);

  i2s_config_t config = {};
  config.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX);
  config.sample_rate = AUDIO_SAMPLE_RATE;
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  config.dma_buf_count = 4;
  config.dma_buf_len = 512;
  config.use_apll = false;
  config.tx_desc_auto_clear = true;
  config.fixed_mclk = 0;

  i2s_pin_config_t pins = {};
  pins.bck_io_num = PIN_SPK_BCLK;
  pins.ws_io_num = PIN_SPK_LRCLK;
  pins.data_out_num = PIN_SPK_DIN;
  pins.data_in_num = I2S_PIN_NO_CHANGE;

  esp_err_t err = i2s_driver_install(I2S_NUM_1, &config, 0, nullptr);
  if (err != ESP_OK) {
    Serial.printf("I2S speaker driver install failed: %d\n", err);
    return;
  }

  err = i2s_set_pin(I2S_NUM_1, &pins);
  if (err != ESP_OK) {
    Serial.printf("I2S speaker pin setup failed: %d\n", err);
    return;
  }

  i2s_zero_dma_buffer(I2S_NUM_1);
}

void syncShanghaiTime() {
  configTzTime(SHANGHAI_TZ, NTP_SERVER_1, NTP_SERVER_2);

  struct tm timeinfo;
  for (int attempt = 0; attempt < 20; attempt++) {
    if (getLocalTime(&timeinfo, 500)) {
      Serial.printf("Shanghai time synced: %04d-%02d-%02d %02d:%02d:%02d\n",
                    timeinfo.tm_year + 1900,
                    timeinfo.tm_mon + 1,
                    timeinfo.tm_mday,
                    timeinfo.tm_hour,
                    timeinfo.tm_min,
                    timeinfo.tm_sec);
      return;
    }
  }

  Serial.println("Shanghai time sync timeout; idle screen will show sync status until SNTP updates.");
}

void connectWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  showScreen("网络连接中", WIFI_SSID);
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
    showScreen("WiFi连接成功", WiFi.localIP().toString());
    delay(1200);
    showScreen("同步上海时间", "请稍候");
    syncShanghaiTime();
    showShanghaiTime();
  } else {
    Serial.println("WiFi connect timeout; will retry");
    showScreen("网络连接失败", "正在重试");
  }
}

void startRecordingNow() {
  if (!websocketConnected) {
    showScreen("云端未连接", "无法录音");
    return;
  }

  i2s_zero_dma_buffer(I2S_NUM_0);
  sendJson("start_record");
  recordingStartedAt = millis();
  state = DeviceState::Recording;
  showScreen("听取中", "请说话");
}

void cancelTurn(const char *reason) {
  sendJson("cancel");
  expectingAudio = false;
  state = DeviceState::Idle;
  i2s_zero_dma_buffer(I2S_NUM_0);
  i2s_zero_dma_buffer(I2S_NUM_1);
  showScreen("已取消", reason);
  delay(800);
  showShanghaiTime();
}

void finishRecording(const char *reason) {
  if (state != DeviceState::Recording) {
    return;
  }

  Serial.printf("Finish recording: %s\n", reason);
  sendJson("finish_record");
  state = DeviceState::Processing;
  showScreen("思考中", "已上传云端");
}

void handleCloudJson(const char *payload, size_t length) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.printf("JSON parse failed: %s\n", error.c_str());
    return;
  }

  const char *type = doc["type"] | "";
  const char *text = doc["text"] | "";
  Serial.printf("Cloud JSON type=%s text=%s\n", type, text);

  if (strcmp(type, "status") == 0) {
    if (state == DeviceState::Processing || state == DeviceState::Recording) {
      showScreen("思考中", text);
    } else {
      showScreen("状态", text);
    }
  } else if (strcmp(type, "asr_text") == 0) {
    state = DeviceState::Processing;
    showScreen("思考中", text);
  } else if (strcmp(type, "answer_text") == 0) {
    state = DeviceState::Processing;
    showScreen("回答中", text);
  } else if (strcmp(type, "audio_start") == 0) {
    expectingAudio = true;
    state = DeviceState::Playing;
    i2s_zero_dma_buffer(I2S_NUM_1);
    showScreen("回答中", "正在播放");
  } else if (strcmp(type, "audio_end") == 0) {
    expectingAudio = false;
    state = DeviceState::Idle;
    showShanghaiTime();
  } else if (strcmp(type, "error") == 0) {
    expectingAudio = false;
    state = DeviceState::Idle;
    showScreen("云端错误", text);
  }
}

void handleCloudAudio(uint8_t *payload, size_t length) {
  if (!expectingAudio || length == 0) {
    Serial.printf("Ignored binary payload: %u bytes\n", static_cast<unsigned>(length));
    return;
  }

  size_t written = 0;
  i2s_write(I2S_NUM_1, payload, length, &written, portMAX_DELAY);
  Serial.printf("Played audio chunk: %u/%u bytes\n",
                static_cast<unsigned>(written),
                static_cast<unsigned>(length));
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
        websocketConnected = false;
        expectingAudio = false;
        state = DeviceState::Idle;
        Serial.println("WebSocket disconnected");
        showScreen("云端断开", "正在重连");
        break;
      case WStype_CONNECTED:
        websocketConnected = true;
        Serial.printf("WebSocket connected: %s\n", payload);
        showShanghaiTime();
        break;
      case WStype_TEXT:
        handleCloudJson(reinterpret_cast<const char *>(payload), length);
        break;
      case WStype_BIN:
        handleCloudAudio(payload, length);
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

void processAsrCommand(const String &command) {
  Serial.printf("ASRPRO command: %s\n", command.c_str());

  if (command == "WAKE") {
    if (state == DeviceState::Idle) {
      startRecordingNow();
    }
  } else if (command == "CANCEL") {
    cancelTurn("语音取消");
  } else if (command == "STOP") {
    sendJson("stop");
    expectingAudio = false;
    state = DeviceState::Idle;
    showScreen("已停止");
    delay(800);
    showShanghaiTime();
  }
}

void pollAsrPro() {
  while (AsrSerial.available() > 0) {
    char c = static_cast<char>(AsrSerial.read());
    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      asrLine.trim();
      asrLine.toUpperCase();
      if (asrLine.length() > 0) {
        processAsrCommand(asrLine);
      }
      asrLine = "";
      continue;
    }

    if (isprint(static_cast<unsigned char>(c)) && asrLine.length() < 32) {
      asrLine += c;
    }
  }
}

void refreshClockIfIdle() {
  if (state != DeviceState::Idle || !websocketConnected || !displayReady) {
    return;
  }

  if (millis() - lastClockDisplayAt < 1000) {
    return;
  }

  showShanghaiTime();
}

void streamMicIfRecording() {
  if (state != DeviceState::Recording) {
    return;
  }

  webSocket.loop();

  size_t bytesRead = 0;
  esp_err_t err = i2s_read(I2S_NUM_0, audioChunk, sizeof(audioChunk), &bytesRead, 20 / portTICK_PERIOD_MS);
  if (err == ESP_OK && bytesRead > 0) {
    webSocket.sendBIN(audioChunk, bytesRead);
    Serial.printf("Sent PCM chunk: %u bytes\n", static_cast<unsigned>(bytesRead));
  }

  if (millis() - recordingStartedAt > RECORD_MAX_MS) {
    finishRecording("max time");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ESP32-S3 AI voice phase 2 voice/screen loopback");
  Serial.printf("Device ID: %s\n", DEVICE_ID);

  if (!validateConfig()) {
    Serial.println("Fix include/config.h, then flash again.");
    while (true) {
      delay(1000);
    }
  }

  setupDisplay();
  setupI2sMic();
  setupI2sSpeaker();
  AsrSerial.begin(ASRPRO_BAUD, SERIAL_8N1, PIN_ASRPRO_RX, PIN_ASRPRO_TX);

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
  pollAsrPro();
  refreshClockIfIdle();
  streamMicIfRecording();
}
