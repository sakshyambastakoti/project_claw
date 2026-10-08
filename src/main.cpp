#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "config.h"
#include "web_page.h"

// ==========================================
// System States Definition
// ==========================================
enum SystemState {
  STATE_IDLE,
  STATE_DEPLOYING,
  STATE_DEPLOYED,
  STATE_RETRACTING,
  STATE_RETRACTED,
  STATE_DEMO_DEPLOYING,
  STATE_DEMO_HOLDING,
  STATE_DEMO_RETRACTING,
  STATE_EMERGENCY_STOP
};

// ==========================================
// Global Variables & Config
// ==========================================
SystemState currentState = STATE_IDLE;

int motorSpeedPwm     = DEFAULT_PWM_SPEED;
uint32_t deployTimeMs = DEFAULT_DEPLOY_TIME_MS;
uint32_t retractTimeMs= DEFAULT_RETRACT_TIME_MS;
uint32_t demoHoldMs   = DEFAULT_DEMO_HOLD_MS;

// Active Motion Timing Tracking
uint32_t motionStartTime = 0;
uint32_t activeDuration  = 0;

// Current Live PWM outputs
int currentM1Pwm = 0;
int currentM2Pwm = 0;

// Web Server on port 80
ESP8266WebServer server(80);

// Status LED timing
uint32_t lastLedBlink = 0;

// ==========================================
// Low-Level Motor Driver Control Functions
// ==========================================

// Stop Motor 1 immediately and ensure dead-time
void stopMotor1() {
  analogWrite(PIN_M1_RPWM, 0);
  analogWrite(PIN_M1_LPWM, 0);
  currentM1Pwm = 0;
}

// Stop Motor 2 immediately and ensure dead-time
void stopMotor2() {
  analogWrite(PIN_M2_RPWM, 0);
  analogWrite(PIN_M2_LPWM, 0);
  currentM2Pwm = 0;
}

// Stop all motors safely
void stopAllMotors() {
  stopMotor1();
  stopMotor2();
}

// Control Motor 1 (direction: 1 = Forward/Contract, -1 = Reverse)
void setMotor1(int direction, int pwm) {
  pwm = constrain(pwm, 0, 1023);
  if (direction > 0) {
    analogWrite(PIN_M1_LPWM, 0);
    analogWrite(PIN_M1_RPWM, pwm);
  } else if (direction < 0) {
    analogWrite(PIN_M1_RPWM, 0);
    analogWrite(PIN_M1_LPWM, pwm);
  } else {
    stopMotor1();
    return;
  }
  currentM1Pwm = pwm;
}

// Control Motor 2 (direction: 1 = Forward/Retract, -1 = Reverse)
void setMotor2(int direction, int pwm) {
  pwm = constrain(pwm, 0, 1023);
  if (direction > 0) {
    analogWrite(PIN_M2_LPWM, 0);
    analogWrite(PIN_M2_RPWM, pwm);
  } else if (direction < 0) {
    analogWrite(PIN_M2_RPWM, 0);
    analogWrite(PIN_M2_LPWM, pwm);
  } else {
    stopMotor2();
    return;
  }
  currentM2Pwm = pwm;
}

// ==========================================
// High-Level Motion Commands
// ==========================================

void startDeploy() {
  stopAllMotors();
  delay(MOTOR_DEADTIME_MS); // Prevent shoot-through / backlash

  currentState = STATE_DEPLOYING;
  motionStartTime = millis();
  activeDuration = deployTimeMs;

  setMotor1(1, motorSpeedPwm); // Motor 1 pulls Red cable
  Serial.printf("[CLAW] Started Deploy (Duration: %u ms, PWM: %d)\n", activeDuration, motorSpeedPwm);
}

void startRetract() {
  stopAllMotors();
  delay(MOTOR_DEADTIME_MS);

  currentState = STATE_RETRACTING;
  motionStartTime = millis();
  activeDuration = retractTimeMs;

  setMotor2(1, motorSpeedPwm); // Motor 2 pulls Blue cable
  Serial.printf("[CLAW] Started Retract (Duration: %u ms, PWM: %d)\n", activeDuration, motorSpeedPwm);
}

void startDemo() {
  stopAllMotors();
  delay(MOTOR_DEADTIME_MS);

  currentState = STATE_DEMO_DEPLOYING;
  motionStartTime = millis();
  activeDuration = deployTimeMs;

  setMotor1(1, motorSpeedPwm);
  Serial.printf("[CLAW] Started Full Demo Sequence\n");
}

void triggerEmergencyStop() {
  stopAllMotors();
  currentState = STATE_EMERGENCY_STOP;
  activeDuration = 0;
  Serial.println("[CLAW] *** EMERGENCY STOP TRIGGERED ***");
}

// Return human-readable state string
String getStateString() {
  switch (currentState) {
    case STATE_IDLE:            return "IDLE";
    case STATE_DEPLOYING:       return "DEPLOYING";
    case STATE_DEPLOYED:        return "DEPLOYED";
    case STATE_RETRACTING:      return "RETRACTING";
    case STATE_RETRACTED:       return "RETRACTED";
    case STATE_DEMO_DEPLOYING:  return "DEMO (DEPLOYING)";
    case STATE_DEMO_HOLDING:    return "DEMO (HOLDING)";
    case STATE_DEMO_RETRACTING: return "DEMO (RETRACTING)";
    case STATE_EMERGENCY_STOP:  return "STOPPED";
    default:                    return "UNKNOWN";
  }
}

// ==========================================
// State Machine Update Loop (Non-blocking)
// ==========================================
void updateStateMachine() {
  uint32_t now = millis();
  uint32_t elapsed = now - motionStartTime;

  switch (currentState) {
    case STATE_DEPLOYING:
      if (elapsed >= activeDuration) {
        stopMotor1();
        currentState = STATE_DEPLOYED;
        activeDuration = 0;
        Serial.println("[CLAW] Deploy cycle finished.");
      }
      break;

    case STATE_RETRACTING:
      if (elapsed >= activeDuration) {
        stopMotor2();
        currentState = STATE_RETRACTED;
        activeDuration = 0;
        Serial.println("[CLAW] Retract cycle finished.");
      }
      break;

    case STATE_DEMO_DEPLOYING:
      if (elapsed >= activeDuration) {
        stopMotor1();
        currentState = STATE_DEMO_HOLDING;
        motionStartTime = millis();
        activeDuration = demoHoldMs;
        Serial.printf("[CLAW] Demo: Holding claws open for %u ms...\n", demoHoldMs);
      }
      break;

    case STATE_DEMO_HOLDING:
      if (elapsed >= activeDuration) {
        currentState = STATE_DEMO_RETRACTING;
        motionStartTime = millis();
        activeDuration = retractTimeMs;
        setMotor2(1, motorSpeedPwm);
        Serial.println("[CLAW] Demo: Retracting claws...");
      }
      break;

    case STATE_DEMO_RETRACTING:
      if (elapsed >= activeDuration) {
        stopMotor2();
        currentState = STATE_IDLE;
        activeDuration = 0;
        Serial.println("[CLAW] Demo sequence complete. Ready.");
      }
      break;

    case STATE_IDLE:
    case STATE_DEPLOYED:
    case STATE_RETRACTED:
    case STATE_EMERGENCY_STOP:
    default:
      // Inactive states
      break;
  }
}

// ==========================================
// Web Server Request Handlers
// ==========================================

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleStatus() {
  uint32_t now = millis();
  uint32_t elapsed = (activeDuration > 0) ? (now - motionStartTime) : 0;
  uint32_t remaining = (elapsed < activeDuration) ? (activeDuration - elapsed) : 0;

  String json = "{";
  json += "\"state\":\"" + getStateString() + "\",";
  json += "\"m1_pwm\":" + String(currentM1Pwm) + ",";
  json += "\"m2_pwm\":" + String(currentM2Pwm) + ",";
  json += "\"target_pwm\":" + String(motorSpeedPwm) + ",";
  json += "\"total_duration\":" + String(activeDuration) + ",";
  json += "\"remaining\":" + String(remaining);
  json += "}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleDeploy() {
  startDeploy();
  server.send(200, "text/plain", "OK");
}

void handleRetract() {
  startRetract();
  server.send(200, "text/plain", "OK");
}

void handleDemo() {
  startDemo();
  server.send(200, "text/plain", "OK");
}

void handleStop() {
  triggerEmergencyStop();
  server.send(200, "text/plain", "OK");
}

void handleConfig() {
  if (server.hasArg("pwm")) {
    motorSpeedPwm = constrain(server.arg("pwm").toInt(), 200, 1023);
  }
  if (server.hasArg("deploy")) {
    deployTimeMs = constrain(server.arg("deploy").toInt(), 500, 15000);
  }
  if (server.hasArg("retract")) {
    retractTimeMs = constrain(server.arg("retract").toInt(), 500, 15000);
  }
  server.send(200, "text/plain", "CONFIG_UPDATED");
}

// ==========================================
// Setup & Main Loop
// ==========================================

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n\n========================================");
  Serial.println("  PROJECT CLAW — ESP8266 INITIALIZING   ");
  Serial.println("========================================");

  // Configure Motor Pins
  pinMode(PIN_M1_RPWM, OUTPUT);
  pinMode(PIN_M1_LPWM, OUTPUT);
  pinMode(PIN_M2_RPWM, OUTPUT);
  pinMode(PIN_M2_LPWM, OUTPUT);
  pinMode(PIN_LED_STATUS, OUTPUT);

  // Set PWM Frequency (1 kHz standard for smooth BTS7960 switching)
  analogWriteFreq(1000);

  // Ensure motors start completely stopped
  stopAllMotors();

  // Initialize Wi-Fi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("[WiFi] Access Point Created! SSID: ");
  Serial.println(WIFI_AP_SSID);
  Serial.print("[WiFi] AP IP Address: ");
  Serial.println(myIP);

  // Start mDNS responder
  if (MDNS.begin(MDNS_HOSTNAME)) {
    Serial.printf("[mDNS] Responder started: http://%s.local\n", MDNS_HOSTNAME);
    MDNS.addService("http", "tcp", 80);
  }

  // Setup Web Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/deploy", HTTP_POST, handleDeploy);
  server.on("/api/retract", HTTP_POST, handleRetract);
  server.on("/api/demo", HTTP_POST, handleDemo);
  server.on("/api/stop", HTTP_POST, handleStop);
  server.on("/api/config", HTTP_POST, handleConfig);

  server.begin();
  Serial.println("[HTTP] Web Server started on port 80.");
  Serial.println("[CLAW] System initialized in IDLE state.");
}

void loop() {
  // Handle HTTP client requests
  server.handleClient();
  MDNS.update();

  // Run non-blocking motion state machine
  updateStateMachine();

  // Status LED indication
  uint32_t now = millis();
  if (currentState == STATE_EMERGENCY_STOP) {
    // Rapid flashing for emergency stop
    if (now - lastLedBlink >= 100) {
      lastLedBlink = now;
      digitalWrite(PIN_LED_STATUS, !digitalRead(PIN_LED_STATUS));
    }
  } else if (currentM1Pwm > 0 || currentM2Pwm > 0) {
    // Solid ON when any motor is running (NodeMCU LED is active LOW)
    digitalWrite(PIN_LED_STATUS, LOW);
  } else {
    // Gentle heartbeat blink when idle
    if (now - lastLedBlink >= 1000) {
      lastLedBlink = now;
      digitalWrite(PIN_LED_STATUS, LOW);
      delay(20);
      digitalWrite(PIN_LED_STATUS, HIGH);
    }
  }
}
