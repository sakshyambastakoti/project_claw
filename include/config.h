#pragma once
#include <Arduino.h>

// ==========================================
// Project CLAW - Hardware Pin Configuration
// NodeMCU ESP8266 Pins for BTS7960 Motor Drivers
// ==========================================

// Motor 1: Contraction (Upper Motor - Red Cable System)
#define PIN_M1_RPWM  5   // NodeMCU D1 (GPIO5)
#define PIN_M1_LPWM  4   // NodeMCU D2 (GPIO4)

// Motor 2: Retraction (Lower Motor - Blue Cable System)
#define PIN_M2_RPWM  14  // NodeMCU D5 (GPIO14)
#define PIN_M2_LPWM  12  // NodeMCU D6 (GPIO12)

// Status LED (Onboard NodeMCU Blue LED)
#define PIN_LED_STATUS 2 // GPIO2 (D4, Active LOW on NodeMCU)

// ==========================================
// Default Timing & Motor Motion Parameters
// ==========================================
#define DEFAULT_PWM_SPEED           850    // Speed range: 0 - 1023 (ESP8266 10-bit PWM)
#define DEFAULT_ROTATION_TIME_MS    3000   // Parameter x: Duration motor runs CW & CCW (ms)
#define DEFAULT_PAUSE_TIME_MS       2000   // Parameter y: Rest duration between motor cycles (ms)
#define MOTOR_DEADTIME_MS           100    // Safe dead-time pause when reversing direction (ms)

// Motor Directions (+1 = Clockwise / Pull, -1 = Anticlockwise / Reverse)
#define MOTOR_DIR_CW                1
#define MOTOR_DIR_CCW              -1
#define MOTOR_DIR_PULL              1
#define MOTOR_DIR_RELEASE          -1

// ==========================================
// EEPROM Persistent Storage Configuration
// Saves Parameters x, y, and Speed across reboots
// ==========================================
#define EEPROM_CONFIG_ADDR          320    // Offset 320 (0-287 reserved by Hawa)
#define EEPROM_CONFIG_MAGIC         0x434C4157 // "CLAW" identifier

struct ClawPersistentSettings {
  uint32_t magic;           // EEPROM_CONFIG_MAGIC
  uint32_t rotationTimeMs;  // Parameter x in ms (Rotation Time)
  uint32_t pauseTimeMs;     // Parameter y in ms (Pause Interval)
  int motorSpeedPwm;        // PWM duty (Motor Speed: 200 - 1023)
};

// ==========================================
// Wi-Fi Access Point Configuration
// ==========================================
#define WIFI_AP_SSID     "Project-CLAW"
#define WIFI_AP_PASSWORD "claw12345"    // Min 8 chars for WPA2
#define MDNS_HOSTNAME    "claw"         // Access at http://claw.local
