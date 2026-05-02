#pragma once

#include <Arduino.h>

constexpr uint8_t OLED_SDA_PIN = 5;
constexpr uint8_t OLED_SCL_PIN = 4;
constexpr uint8_t OLED_ADDR_PRIMARY = 0x3C;
constexpr uint8_t OLED_ADDR_SECONDARY = 0x3D;

constexpr uint8_t BUTTON_LEFT_PIN = 34;
constexpr uint8_t BUTTON_RIGHT_PIN = 36;
constexpr uint8_t BUTTON_SELECT_PIN = 39;
constexpr uint8_t BUTTON_ACTIVE_STATE = LOW;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 35;
constexpr uint16_t BUTTON_LONGPRESS_MS = 800;

constexpr uint8_t RGB_LED_PIN = 27;
constexpr uint8_t RGB_LED_COUNT = 1;
constexpr uint8_t RGB_LED_BRIGHTNESS = 48;

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr int8_t OLED_RESET_PIN = -1;

