#pragma once

#include <Arduino.h>

#define GAMER_BOARD_DEVKIT_SSD1306 1
#define GAMER_BOARD_WAVESHARE_AMOLED_18 2

#ifndef GAMER_BOARD_PROFILE
#define GAMER_BOARD_PROFILE GAMER_BOARD_DEVKIT_SSD1306
#endif

#if GAMER_BOARD_PROFILE == GAMER_BOARD_DEVKIT_SSD1306
#define GAMER_DISPLAY_IS_SSD1306 1
#define GAMER_DISPLAY_IS_SH8601 0
#define GAMER_HAS_TOUCH 0
#define GAMER_HAS_BOOT_BUTTON 0
constexpr const char* GAMER_BOARD_NAME = "devkit-ssd1306";
constexpr uint8_t OLED_SDA_PIN = 5;
constexpr uint8_t OLED_SCL_PIN = 4;
constexpr uint8_t OLED_ADDR_PRIMARY = 0x3C;
constexpr uint8_t OLED_ADDR_SECONDARY = 0x3D;
constexpr int8_t OLED_RESET_PIN = -1;

constexpr uint8_t BUTTON_LEFT_PIN = 34;
constexpr uint8_t BUTTON_RIGHT_PIN = 36;
constexpr uint8_t BUTTON_SELECT_PIN = 39;
constexpr uint8_t BUTTON_ACTIVE_STATE = LOW;

constexpr uint16_t BUTTON_DEBOUNCE_MS = 35;
constexpr uint16_t BUTTON_LONGPRESS_MS = 800;

constexpr uint8_t RGB_LED_PIN = 27;
constexpr uint8_t RGB_LED_COUNT = 1;
constexpr uint8_t RGB_LED_BRIGHTNESS = 48;

constexpr uint16_t SCREEN_WIDTH = 128;
constexpr uint16_t SCREEN_HEIGHT = 64;
constexpr uint16_t GAMER_BLACK = 0;
constexpr uint16_t GAMER_WHITE = 1;
constexpr uint16_t GAMER_ACCENT = 1;
constexpr uint16_t GAMER_DIM = 1;
constexpr uint16_t GAMER_INVERSE = 2;

#elif GAMER_BOARD_PROFILE == GAMER_BOARD_WAVESHARE_AMOLED_18
#define GAMER_DISPLAY_IS_SSD1306 0
#define GAMER_DISPLAY_IS_SH8601 1
#define GAMER_HAS_TOUCH 1
#define GAMER_HAS_BOOT_BUTTON 1
constexpr const char* GAMER_BOARD_NAME = "waveshare-touch-amoled-1.8";

#ifndef ARDUINO_USB_MODE
#define ARDUINO_USB_MODE 0
#endif
#ifndef ARDUINO_USB_CDC_ON_BOOT
#define ARDUINO_USB_CDC_ON_BOOT 1
#endif

constexpr int PIN_LCD_SDIO0 = 4;
constexpr int PIN_LCD_SDIO1 = 5;
constexpr int PIN_LCD_SDIO2 = 6;
constexpr int PIN_LCD_SDIO3 = 7;
constexpr int PIN_LCD_SCLK = 11;
constexpr int PIN_LCD_CS = 12;
constexpr int PIN_LCD_RST = -1;

constexpr int PIN_TOUCH_SDA = 15;
constexpr int PIN_TOUCH_SCL = 14;
constexpr int PIN_TOUCH_INT = 21;

constexpr int PIN_BOOT_BUTTON = 0;
constexpr bool BOOT_BUTTON_ACTIVE_LOW = true;

constexpr uint8_t AMOLED_EXPANDER_I2C_ADDR = 0x20;
constexpr uint8_t AMOLED_BRIGHTNESS = 255;

constexpr uint16_t BUTTON_DEBOUNCE_MS = 35;
constexpr uint16_t BUTTON_LONGPRESS_MS = 800;

constexpr uint8_t RGB_LED_PIN = 48;
constexpr uint8_t RGB_LED_COUNT = 1;
constexpr uint8_t RGB_LED_BRIGHTNESS = 32;

constexpr uint16_t SCREEN_WIDTH = 368;
constexpr uint16_t SCREEN_HEIGHT = 448;

constexpr uint16_t GAMER_BLACK = 0x0000;
constexpr uint16_t GAMER_WHITE = 0xFFFF;
constexpr uint16_t GAMER_ACCENT = 0x07FF;
constexpr uint16_t GAMER_DIM = 0x7BEF;
constexpr uint16_t GAMER_INVERSE = 0xF81F;

#else
#error "Unsupported GAMER_BOARD_PROFILE"
#endif

constexpr uint32_t SERIAL_BAUD = 115200;
