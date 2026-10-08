#pragma once
#include <Arduino.h>

// ==========================================
// Project CLAW - Hardware Pin Configuration
// NodeMCU ESP8266 Pins for BTS7960 Motor Drivers
// ==========================================

// Motor 1: Contraction / Deployment (Upper Motor - Red Cable System)
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
#define DEFAULT_PWM_SPEED       850    // Speed range: 0 - 1023 (ESP8266 default PWM range)
#define DEFAULT_DEPLOY_TIME_MS  3000   // Time Motor 1 runs to contract/deploy claws
#define DEFAULT_RETRACT_TIME_MS 3000   // Time Motor 2 runs to retract claws
#define DEFAULT_DEMO_HOLD_MS    2000   // Time claws stay held open in demo mode
#define MOTOR_DEADTIME_MS       150    // Safe pause between switching motor states

// ==========================================
// Wi-Fi Access Point Configuration
// ==========================================
#define WIFI_AP_SSID     "Project-CLAW"
#define WIFI_AP_PASSWORD "claw12345"    // Min 8 chars for WPA2
#define MDNS_HOSTNAME    "claw"         // Access at http://claw.local
