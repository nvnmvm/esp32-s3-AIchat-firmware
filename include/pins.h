#pragma once

// 0.96 inch SH1106 OLED, U8g2 software I2C.
#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9
#define OLED_I2C_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// MS4030 I2S microphone.
#define PIN_MIC_BCLK 4
#define PIN_MIC_WS 5
#define PIN_MIC_SD 6

// ASRPRO UART0 TX -> ESP32-S3 Serial1 RX.
#define PIN_ASRPRO_RX 16
#define PIN_ASRPRO_TX 17
#define ASRPRO_BAUD 9600

// MAX98357A I2S speaker amplifier.
#define PIN_SPK_BCLK 12
#define PIN_SPK_LRCLK 13
#define PIN_SPK_DIN 14
#define PIN_SPK_SD 15
