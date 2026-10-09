#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <Hawa.h>
#include "config.h"
#include "web_page.h"

// Captive Portal DNS Server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// ==========================================
// System States Definition
// ==========================================
enum SystemState {
  STATE_IDLE,            // 0: Both motors stopped at initial 0 rotation
  STATE_STEP1_M1_CW,     // 1: Motor 1 turns clockwise for x seconds
  STATE_STEP2_M1_CCW,    // 2: Motor 1 turns anticlockwise for x seconds
  STATE_STEP3_PAUSE,     // 3: All rotation stops for y seconds
  STATE_STEP4_M2_CW,     // 4: Motor 2 turns clockwise for x seconds
  STATE_STEP5_M2_CCW,    // 5: Motor 2 turns anticlockwise for x seconds
  STATE_HOMING,          // 6: Reversing active motor to return to 0 rotation
  STATE_EMERGENCY_STOP   // 7: Manual emergency stop
};

// ==========================================
// Global Variables & Config
// ==========================================
SystemState currentState = STATE_IDLE;
bool cycleEnabled = false;
int currentStep = 0; // 0 = Idle/Homing, 1..5 = Active Steps

// Configurable Parameters (Stored persistently in EEPROM)
uint32_t rotationTimeMs = DEFAULT_ROTATION_TIME_MS; // Parameter x (ms)
uint32_t pauseTimeMs    = DEFAULT_PAUSE_TIME_MS;    // Parameter y (ms)
int motorSpeedPwm       = DEFAULT_PWM_SPEED;        // Drive PWM (200 - 1023)

// Motion Timing Tracking
uint32_t stepStartTime = 0;
uint32_t stepDuration  = 0;

// Homing Parameters for Returning to 0 Rotation
int homingMotor        = 0;
int homingDirection    = MOTOR_DIR_CCW;
uint32_t homingStartTime = 0;
uint32_t homingDuration  = 0;

// Current Live PWM outputs (+ve = CW/Forward, -ve = CCW/Reverse)
int currentM1Pwm = 0;
int currentM2Pwm = 0;

// Web Server on port 80 & OTA Update Server
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

// Inbuilt Status LED Timing & Web Command Blinker
uint32_t lastLedBlink = 0;
uint32_t commandBlinkEndMs = 0;
uint32_t lastCommandLedToggle = 0;
bool commandLedState = false;

// Trigger fast blinking burst on status LED upon command
void triggerCommandBlink(uint32_t durationMs = 350) {
  commandBlinkEndMs = millis() + durationMs;
  commandLedState = true;
  digitalWrite(PIN_LED_STATUS, LOW); // Active LOW on NodeMCU
#ifdef LED_BUILTIN
  if (LED_BUILTIN != PIN_LED_STATUS) digitalWrite(LED_BUILTIN, LOW);
#endif
}

// Serial Command Buffer
String serialBuffer = "";

// ==========================================
// EEPROM Persistent Storage Helpers
// ==========================================
void loadPersistentSettings() {
  EEPROM.begin(512);
  ClawPersistentSettings saved;
  EEPROM.get(EEPROM_CONFIG_ADDR, saved);

  if (saved.magic == EEPROM_CONFIG_MAGIC &&
      saved.rotationTimeMs >= 300 && saved.rotationTimeMs <= 30000 &&
      saved.pauseTimeMs <= 30000 &&
      saved.motorSpeedPwm >= 200 && saved.motorSpeedPwm <= 1023) {
    rotationTimeMs = saved.rotationTimeMs;
    pauseTimeMs    = saved.pauseTimeMs;
    motorSpeedPwm  = saved.motorSpeedPwm;
    Serial.printf("[EEPROM] Restored Settings: Rotation Time (x)=%u ms, Pause (y)=%u ms, Speed=%d PWM\n",
                  rotationTimeMs, pauseTimeMs, motorSpeedPwm);
  } else {
    // Defaults
    rotationTimeMs = DEFAULT_ROTATION_TIME_MS;
    pauseTimeMs    = DEFAULT_PAUSE_TIME_MS;
    motorSpeedPwm  = DEFAULT_PWM_SPEED;

    ClawPersistentSettings defSettings;
    defSettings.magic = EEPROM_CONFIG_MAGIC;
    defSettings.rotationTimeMs = rotationTimeMs;
    defSettings.pauseTimeMs = pauseTimeMs;
    defSettings.motorSpeedPwm = motorSpeedPwm;
    EEPROM.put(EEPROM_CONFIG_ADDR, defSettings);
    EEPROM.commit();
    Serial.println("[EEPROM] Initialized fresh persistent settings in flash memory.");
  }
}

void savePersistentSettings() {
  ClawPersistentSettings toSave;
  toSave.magic = EEPROM_CONFIG_MAGIC;
  toSave.rotationTimeMs = rotationTimeMs;
  toSave.pauseTimeMs = pauseTimeMs;
  toSave.motorSpeedPwm = motorSpeedPwm;
  EEPROM.put(EEPROM_CONFIG_ADDR, toSave);
  EEPROM.commit();
  Serial.printf("[EEPROM] Saved Settings: Rotation Time (x)=%u ms, Pause (y)=%u ms, Speed=%d PWM\n",
                rotationTimeMs, pauseTimeMs, motorSpeedPwm);
}

// ==========================================
// Low-Level Motor Driver Control Functions
// ==========================================

void stopMotor1() {
  analogWrite(PIN_M1_RPWM, 0);
  analogWrite(PIN_M1_LPWM, 0);
  currentM1Pwm = 0;
}

void stopMotor2() {
  analogWrite(PIN_M2_RPWM, 0);
  analogWrite(PIN_M2_LPWM, 0);
  currentM2Pwm = 0;
}

void stopAllMotors() {
  stopMotor1();
  stopMotor2();
}

// Control Motor 1 (+1 = Clockwise, -1 = Anticlockwise, 0 = Stop)
void setMotor1(int direction, int pwm) {
  pwm = constrain(pwm, 0, 1023);
  if (direction > 0) {
    analogWrite(PIN_M1_LPWM, 0);
    analogWrite(PIN_M1_RPWM, pwm);
    currentM1Pwm = pwm;
  } else if (direction < 0) {
    analogWrite(PIN_M1_RPWM, 0);
    analogWrite(PIN_M1_LPWM, pwm);
    currentM1Pwm = -pwm;
  } else {
    stopMotor1();
  }
}

// Control Motor 2 (+1 = Clockwise, -1 = Anticlockwise, 0 = Stop)
void setMotor2(int direction, int pwm) {
  pwm = constrain(pwm, 0, 1023);
  if (direction > 0) {
    analogWrite(PIN_M2_LPWM, 0);
    analogWrite(PIN_M2_RPWM, pwm);
    currentM2Pwm = pwm;
  } else if (direction < 0) {
    analogWrite(PIN_M2_RPWM, 0);
    analogWrite(PIN_M2_LPWM, pwm);
    currentM2Pwm = -pwm;
  } else {
    stopMotor2();
  }
}

// Human-readable state name string
String getStateString() {
  switch (currentState) {
    case STATE_IDLE:          return "IDLE";
    case STATE_STEP1_M1_CW:   return "STEP 1: MOTOR 1 CLOCKWISE";
    case STATE_STEP2_M1_CCW:  return "STEP 2: MOTOR 1 ANTICLOCKWISE";
    case STATE_STEP3_PAUSE:   return "STEP 3: ALL ROTATION PAUSED";
    case STATE_STEP4_M2_CW:   return "STEP 4: MOTOR 2 CLOCKWISE";
    case STATE_STEP5_M2_CCW:  return "STEP 5: MOTOR 2 ANTICLOCKWISE";
    case STATE_HOMING:        return "HOMING (RETURNING TO 0)";
    case STATE_EMERGENCY_STOP:return "EMERGENCY STOP";
    default:                  return "UNKNOWN";
  }
}

// ==========================================
// High-Level Motion Commands & Homing Logic
// ==========================================

void startCycle() {
  stopAllMotors();
  delay(MOTOR_DEADTIME_MS);

  cycleEnabled = true;
  currentState = STATE_STEP1_M1_CW;
  currentStep = 1;
  stepStartTime = millis();
  stepDuration = rotationTimeMs;

  setMotor1(MOTOR_DIR_CW, motorSpeedPwm);
  stopMotor2();

  Serial.printf("[CLAW] Cycle STARTED -> Step 1: Motor 1 CW for %u ms (PWM %d)\n",
                rotationTimeMs, motorSpeedPwm);
  Hawa.log("[CLAW] Cycle Started: Step 1 M1 CW");
  Hawa.sendData("state", "STEP1_M1_CW");
}

// Math-based Homing: calculates net offset and reverses active motor to return to initial 0 rotation
void stopCycleWithHoming() {
  if (!cycleEnabled && currentState == STATE_IDLE) {
    return;
  }
  cycleEnabled = false;

  uint32_t now = millis();
  uint32_t elapsed = (stepStartTime > 0 && now >= stepStartTime) ? (now - stepStartTime) : 0;

  int motorToZero = 0;
  int zeroDirection = MOTOR_DIR_CCW;
  uint32_t zeroDuration = 0;

  switch (currentState) {
    case STATE_STEP1_M1_CW:
      // Motor 1 turned CW for 'elapsed' ms.
      // Net offset: +elapsed CW. To zero: rotate CCW for 'elapsed' ms.
      motorToZero = 1;
      zeroDirection = MOTOR_DIR_CCW;
      zeroDuration = min(elapsed, rotationTimeMs);
      break;

    case STATE_STEP2_M1_CCW:
      // Motor 1 completed rotationTimeMs in CW, and has reversed for 'elapsed' in CCW.
      // Net offset: (rotationTimeMs - elapsed) in CW direction.
      if (elapsed < rotationTimeMs) {
        motorToZero = 1;
        zeroDirection = MOTOR_DIR_CCW; // Keep reversing until 0 reached
        zeroDuration = rotationTimeMs - elapsed;
      } else {
        motorToZero = 0;
        zeroDuration = 0;
      }
      break;

    case STATE_STEP3_PAUSE:
      // In pause, Motor 1 already fully reversed to 0, and Motor 2 hasn't moved.
      // Both motors are already at 0 rotation!
      motorToZero = 0;
      zeroDuration = 0;
      break;

    case STATE_STEP4_M2_CW:
      // Motor 2 turned CW for 'elapsed' ms.
      // Net offset: +elapsed CW. To zero: rotate CCW for 'elapsed' ms.
      motorToZero = 2;
      zeroDirection = MOTOR_DIR_CCW;
      zeroDuration = min(elapsed, rotationTimeMs);
      break;

    case STATE_STEP5_M2_CCW:
      // Motor 2 completed rotationTimeMs in CW, and has reversed for 'elapsed' in CCW.
      // Net offset: (rotationTimeMs - elapsed) in CW direction.
      if (elapsed < rotationTimeMs) {
        motorToZero = 2;
        zeroDirection = MOTOR_DIR_CCW; // Keep reversing until 0 reached
        zeroDuration = rotationTimeMs - elapsed;
      } else {
        motorToZero = 0;
        zeroDuration = 0;
      }
      break;

    case STATE_HOMING:
      // Already returning to 0, let homing finish
      return;

    default:
      motorToZero = 0;
      zeroDuration = 0;
      break;
  }

  stopAllMotors();

  if (motorToZero > 0 && zeroDuration > 30) {
    currentState = STATE_HOMING;
    currentStep = 0;
    homingMotor = motorToZero;
    homingDirection = zeroDirection;
    homingDuration = zeroDuration;
    homingStartTime = millis();

    delay(MOTOR_DEADTIME_MS);
    if (motorToZero == 1) {
      setMotor1(zeroDirection, motorSpeedPwm);
      stopMotor2();
    } else {
      setMotor2(zeroDirection, motorSpeedPwm);
      stopMotor1();
    }
    Serial.printf("[CLAW] Homing Motor %d to 0 rotation: running CCW for %u ms\n",
                  motorToZero, zeroDuration);
    Hawa.log("[CLAW] Homing M" + String(motorToZero) + " to 0 for " + String(zeroDuration) + " ms");
    Hawa.sendData("state", "HOMING");
  } else {
    currentState = STATE_IDLE;
    currentStep = 0;
    Serial.println("[CLAW] Cycle Stopped. Both motors verified at initial 0 rotation.");
    Hawa.log("[CLAW] Stopped at initial 0 rotation");
    Hawa.sendData("state", "IDLE");
  }
}

void triggerEmergencyStop() {
  stopAllMotors();
  cycleEnabled = false;
  currentState = STATE_EMERGENCY_STOP;
  currentStep = 0;
  Serial.println("[CLAW] *** EMERGENCY HARD STOP TRIGGERED ***");
  Hawa.log("[CLAW] Emergency Stop triggered");
  Hawa.sendData("state", "EMERGENCY_STOP");
}

// ==========================================
// Non-Blocking State Machine Update Loop
// ==========================================
void updateStateMachine() {
  uint32_t now = millis();
  uint32_t elapsed = (stepStartTime > 0) ? (now - stepStartTime) : 0;

  switch (currentState) {
    case STATE_STEP1_M1_CW:
      if (elapsed >= rotationTimeMs) {
        stopAllMotors();
        delay(MOTOR_DEADTIME_MS);
        currentState = STATE_STEP2_M1_CCW;
        currentStep = 2;
        stepStartTime = millis();
        stepDuration = rotationTimeMs;
        setMotor1(MOTOR_DIR_CCW, motorSpeedPwm);
        stopMotor2();
        Serial.printf("[CLAW] Step 2: Motor 1 CCW for %u ms\n", rotationTimeMs);
        Hawa.sendData("state", "STEP2_M1_CCW");
      }
      break;

    case STATE_STEP2_M1_CCW:
      if (elapsed >= rotationTimeMs) {
        stopAllMotors();
        delay(MOTOR_DEADTIME_MS);
        currentState = STATE_STEP3_PAUSE;
        currentStep = 3;
        stepStartTime = millis();
        stepDuration = pauseTimeMs;
        Serial.printf("[CLAW] Step 3: All rotation PAUSED for %u ms\n", pauseTimeMs);
        Hawa.sendData("state", "STEP3_PAUSE");
      }
      break;

    case STATE_STEP3_PAUSE:
      if (elapsed >= pauseTimeMs) {
        currentState = STATE_STEP4_M2_CW;
        currentStep = 4;
        stepStartTime = millis();
        stepDuration = rotationTimeMs;
        setMotor2(MOTOR_DIR_CW, motorSpeedPwm);
        stopMotor1();
        Serial.printf("[CLAW] Step 4: Motor 2 CW for %u ms\n", rotationTimeMs);
        Hawa.sendData("state", "STEP4_M2_CW");
      }
      break;

    case STATE_STEP4_M2_CW:
      if (elapsed >= rotationTimeMs) {
        stopAllMotors();
        delay(MOTOR_DEADTIME_MS);
        currentState = STATE_STEP5_M2_CCW;
        currentStep = 5;
        stepStartTime = millis();
        stepDuration = rotationTimeMs;
        setMotor2(MOTOR_DIR_CCW, motorSpeedPwm);
        stopMotor1();
        Serial.printf("[CLAW] Step 5: Motor 2 CCW for %u ms\n", rotationTimeMs);
        Hawa.sendData("state", "STEP5_M2_CCW");
      }
      break;

    case STATE_STEP5_M2_CCW:
      if (elapsed >= rotationTimeMs) {
        stopAllMotors();
        delay(MOTOR_DEADTIME_MS);
        // Step 6: Loop back to Step 1 if cycle is still enabled
        if (cycleEnabled) {
          currentState = STATE_STEP1_M1_CW;
          currentStep = 1;
          stepStartTime = millis();
          stepDuration = rotationTimeMs;
          setMotor1(MOTOR_DIR_CW, motorSpeedPwm);
          stopMotor2();
          Serial.printf("[CLAW] Step 6 -> Looping back to Step 1: Motor 1 CW (%u ms)\n", rotationTimeMs);
          Hawa.sendData("state", "STEP1_M1_CW");
        } else {
          currentState = STATE_IDLE;
          currentStep = 0;
          Serial.println("[CLAW] Sequence complete. Both motors at 0 rotation.");
          Hawa.sendData("state", "IDLE");
        }
      }
      break;

    case STATE_HOMING:
      if (millis() - homingStartTime >= homingDuration) {
        stopAllMotors();
        currentState = STATE_IDLE;
        currentStep = 0;
        Serial.println("[CLAW] Homing complete! Both motors returned to initial 0 rotation.");
        Hawa.log("[CLAW] Homing finished at 0 rotation");
        Hawa.sendData("state", "IDLE");
      }
      break;

    case STATE_IDLE:
    case STATE_EMERGENCY_STOP:
    default:
      break;
  }
}

// ==========================================
// Web Server Request Handlers
// ==========================================

void handleRoot() {
  server.sendHeader("Content-Encoding", "gzip");
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.send_P(200, "text/html", (PGM_P)INDEX_HTML_GZ, INDEX_HTML_GZ_LEN);
}

void handleStatus() {
  uint32_t now = millis();
  uint32_t elapsed = 0;
  uint32_t duration = 0;

  if (currentState == STATE_HOMING) {
    elapsed = (now >= homingStartTime) ? (now - homingStartTime) : 0;
    duration = homingDuration;
  } else if (currentStep >= 1 && currentStep <= 5) {
    elapsed = (now >= stepStartTime) ? (now - stepStartTime) : 0;
    duration = stepDuration;
  }

  uint32_t remaining = (elapsed < duration) ? (duration - elapsed) : 0;
  int progress = (duration > 0) ? constrain((int)((elapsed * 100) / duration), 0, 100) : 0;

  bool isWifiConnected = (WiFi.status() == WL_CONNECTED);

  String json = "{";
  json += "\"enabled\":" + String(cycleEnabled ? "true" : "false") + ",";
  json += "\"state\":\"" + getStateString() + "\",";
  json += "\"step\":" + String(currentStep) + ",";
  json += "\"is_homing\":" + String(currentState == STATE_HOMING ? "true" : "false") + ",";
  json += "\"rotation_time\":" + String(rotationTimeMs) + ",";
  json += "\"pause_time\":" + String(pauseTimeMs) + ",";
  json += "\"speed\":" + String(motorSpeedPwm) + ",";
  json += "\"step_elapsed\":" + String(elapsed) + ",";
  json += "\"step_duration\":" + String(duration) + ",";
  json += "\"step_remaining\":" + String(remaining) + ",";
  json += "\"step_progress\":" + String(progress) + ",";
  json += "\"m1_pwm\":" + String(currentM1Pwm) + ",";
  json += "\"m2_pwm\":" + String(currentM2Pwm) + ",";
  // Wireless Status
  json += "\"wifi_connected\":" + String(isWifiConnected ? "true" : "false") + ",";
  json += "\"wifi_ssid\":\"" + (isWifiConnected ? WiFi.SSID() : Hawa.getSsid()) + "\",";
  json += "\"station_ip\":\"" + (isWifiConnected ? WiFi.localIP().toString() : "") + "\",";
  json += "\"ap_ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"rssi\":" + String(isWifiConnected ? WiFi.RSSI() : 0) + ",";
  json += "\"hawa_connected\":" + String(Hawa.isConnected() ? "true" : "false") + ",";
  json += "\"hawa_device_id\":\"" + Hawa.getDeviceId() + "\",";
  json += "\"hawa_server\":\"" + Hawa.getServerUrl() + "\",";
  json += "\"hawa_device_name\":\"" + Hawa.getDeviceName() + "\",";
  json += "\"hawa_ota_running\":" + String(Hawa.isOtaRunning() ? "true" : "false");
  json += "}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleToggle() {
  triggerCommandBlink(400);

  if (server.hasArg("state")) {
    String s = server.arg("state");
    s.toLowerCase();
    if (s == "1" || s == "true" || s == "on") {
      startCycle();
    } else {
      stopCycleWithHoming();
    }
  } else {
    // Parameterless toggle flip
    if (cycleEnabled) {
      stopCycleWithHoming();
    } else {
      startCycle();
    }
  }

  handleStatus();
}

void handleConfig() {
  triggerCommandBlink(300);
  bool changed = false;

  // Parameter x: Rotation Time
  if (server.hasArg("rotation_time")) {
    rotationTimeMs = constrain(server.arg("rotation_time").toInt(), 300, 30000);
    changed = true;
  } else if (server.hasArg("x")) {
    rotationTimeMs = constrain(server.arg("x").toInt(), 300, 30000);
    changed = true;
  }

  // Parameter y: Pause Interval
  if (server.hasArg("pause_time")) {
    pauseTimeMs = constrain(server.arg("pause_time").toInt(), 0, 30000);
    changed = true;
  } else if (server.hasArg("y")) {
    pauseTimeMs = constrain(server.arg("y").toInt(), 0, 30000);
    changed = true;
  }

  // Motor Speed / PWM
  if (server.hasArg("speed")) {
    motorSpeedPwm = constrain(server.arg("speed").toInt(), 200, 1023);
    changed = true;
  } else if (server.hasArg("pwm")) {
    motorSpeedPwm = constrain(server.arg("pwm").toInt(), 200, 1023);
    changed = true;
  }

  if (changed) {
    savePersistentSettings();
  }

  handleStatus();
}

void handleEmergencyStop() {
  triggerCommandBlink(400);
  triggerEmergencyStop();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "STOPPED");
}

// Wi-Fi Network Scan
void handleWifiScan() {
  int n = WiFi.scanNetworks(false, false);
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"secure\":" + String(WiFi.encryptionType(i) == ENC_TYPE_NONE ? "false" : "true");
    json += "}";
  }
  json += "]";
  WiFi.scanDelete();

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

// Wi-Fi Credentials Provisioning
void handleWifiSave() {
  triggerCommandBlink(500);
  String ssid = server.hasArg("ssid") ? server.arg("ssid") : "";
  String pass = server.hasArg("password") ? server.arg("password") : "";
  String serverUrl = server.hasArg("server") ? server.arg("server") : "";
  String devName = server.hasArg("name") ? server.arg("name") : "";

  ssid.trim();
  pass.trim();
  serverUrl.trim();
  devName.trim();

  if (ssid.length() == 0) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Wi-Fi SSID is required\"}");
    return;
  }

  Serial.printf("[WiFi] Provisioning request -> SSID: %s\n", ssid.c_str());
  if (serverUrl.length() == 0) {
    serverUrl = "wss://hawa-platform.onrender.com/ws";
  }

  Hawa.saveCredentials(ssid, pass, serverUrl, devName);

  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  WiFi.begin(ssid.c_str(), pass.c_str());

  Hawa.log("[WIFI] New credentials saved for SSID: " + ssid);

  String resp = "{\"status\":\"ok\",\"message\":\"Wi-Fi settings saved. Connecting to " + ssid + "...\"}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", resp);
}

// Clear Saved Wi-Fi Credentials
void handleWifiClear() {
  triggerCommandBlink(500);
  Hawa.clearCredentials();
  WiFi.disconnect();
  Serial.println("[WiFi] Credentials wiped.");

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Wi-Fi credentials cleared.\"}");
}

// Remote Reboot Handler
void handleReboot() {
  triggerCommandBlink(1000);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Rebooting hardware...\"}");
  delay(500);
  ESP.restart();
}

// ==========================================
// Setup & Main Loop
// ==========================================

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n\n========================================");
  Serial.println("   PROJECT CLAW — ESP8266 INITIALIZING  ");
  Serial.println("========================================");

  // Configure Motor Pins
  pinMode(PIN_M1_RPWM, OUTPUT);
  pinMode(PIN_M1_LPWM, OUTPUT);
  pinMode(PIN_M2_RPWM, OUTPUT);
  pinMode(PIN_M2_LPWM, OUTPUT);
  pinMode(PIN_LED_STATUS, OUTPUT);
  digitalWrite(PIN_LED_STATUS, HIGH); // Off initially (Active LOW)
#ifdef LED_BUILTIN
  if (LED_BUILTIN != PIN_LED_STATUS) {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
  }
#endif

  // Set PWM Frequency (1 kHz standard for BTS7960)
  analogWriteFreq(1000);

  // Ensure motors start completely stopped at 0 rotation
  stopAllMotors();

  // Load persistent settings (Parameter x, y, and PWM) from EEPROM
  loadPersistentSettings();

  // Initialize Dual AP + STA Wi-Fi Mode so local AP is always available
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);

  IPAddress apIP = WiFi.softAPIP();
  Serial.print("[WiFi] Access Point Created! SSID: ");
  Serial.println(WIFI_AP_SSID);
  Serial.print("[WiFi] AP IP Address: ");
  Serial.println(apIP);

  // Start Captive Portal DNS Server (resolves all domains to 192.168.4.1)
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("[DNS] Captive Portal DNS Server active on port 53");

  // Initialize Hawa Universal IoT & OTA Agent
  Hawa.begin();

  // Register remote Hawa Cloud commands
  Hawa.onCommand("toggle", [](const String& val) {
    triggerCommandBlink(400);
    if (cycleEnabled) {
      Hawa.log("[HAWA CMD] Cycle OFF requested -> Homing to 0");
      stopCycleWithHoming();
    } else {
      Hawa.log("[HAWA CMD] Cycle ON requested");
      startCycle();
    }
  });
  Hawa.onCommand("on", [](const String& val) {
    triggerCommandBlink(400);
    Hawa.log("[HAWA CMD] Cycle ON requested");
    startCycle();
  });
  Hawa.onCommand("off", [](const String& val) {
    triggerCommandBlink(400);
    Hawa.log("[HAWA CMD] Cycle OFF requested -> Homing to 0");
    stopCycleWithHoming();
  });
  Hawa.onCommand("stop", [](const String& val) {
    triggerCommandBlink(400);
    Hawa.log("[HAWA CMD] Emergency stop requested");
    triggerEmergencyStop();
  });

  // Start mDNS responder
  if (MDNS.begin(MDNS_HOSTNAME)) {
    Serial.printf("[mDNS] Responder started: http://%s.local\n", MDNS_HOSTNAME);
    MDNS.addService("http", "tcp", 80);
  }

  // Setup Web Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/generate_204", HTTP_GET, handleRoot);        // Android captive portal
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot); // Apple iOS captive portal
  server.on("/canonical.html", HTTP_GET, handleRoot);
  server.on("/connecttest.txt", HTTP_GET, handleRoot);
  server.on("/ncsi.txt", HTTP_GET, handleRoot);

  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/toggle", HTTP_POST, handleToggle);
  server.on("/api/config", HTTP_POST, handleConfig);
  server.on("/api/stop", HTTP_POST, handleEmergencyStop);
  server.on("/api/emergency-stop", HTTP_POST, handleEmergencyStop);

  // Backward-compatible endpoints for old scripts
  server.on("/api/deploy", HTTP_POST, handleToggle);
  server.on("/api/retract", HTTP_POST, handleToggle);
  server.on("/api/demo", HTTP_POST, handleToggle);

  // Wi-Fi Provisioning & Hawa Cloud API Endpoints
  server.on("/api/wifi-scan", HTTP_GET, handleWifiScan);
  server.on("/api/wifi-save", HTTP_POST, handleWifiSave);
  server.on("/api/wifi-clear", HTTP_POST, handleWifiClear);
  server.on("/api/reboot", HTTP_POST, handleReboot);

  // Captive Portal 404 Redirection
  server.onNotFound([]() {
    if (server.uri().startsWith("/api/")) {
      server.send(404, "application/json", "{\"status\":\"not_found\"}");
    } else {
      server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/", true);
      server.send(302, "text/plain", "");
    }
  });

  // Setup Web OTA Firmware Update at /update
  httpUpdater.setup(&server, "/update");

  server.begin();
  Serial.println("[HTTP] Web Server & OTA Updater (/update) started on port 80.");
  Serial.println("[CLAW] System ready in IDLE state (0 rotation).");
}

// ==========================================
// Serial Command Handler (Web Serial / USB CLI)
// ==========================================
void handleSerialCommands() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (serialBuffer.length() > 0) {
        serialBuffer.trim();
        serialBuffer.toUpperCase();

        if (serialBuffer == "ON" || serialBuffer == "START") {
          triggerCommandBlink(400);
          startCycle();
        } else if (serialBuffer == "OFF" || serialBuffer == "STOP") {
          triggerCommandBlink(400);
          stopCycleWithHoming();
        } else if (serialBuffer == "TOGGLE") {
          triggerCommandBlink(400);
          if (cycleEnabled) stopCycleWithHoming(); else startCycle();
        } else if (serialBuffer == "KILL" || serialBuffer == "EMERGENCY") {
          triggerCommandBlink(400);
          triggerEmergencyStop();
        } else if (serialBuffer.startsWith("SET_X=")) {
          int val = serialBuffer.substring(6).toInt();
          if (val >= 300 && val <= 30000) {
            rotationTimeMs = val;
            savePersistentSettings();
          }
        } else if (serialBuffer.startsWith("SET_Y=")) {
          int val = serialBuffer.substring(6).toInt();
          if (val >= 0 && val <= 30000) {
            pauseTimeMs = val;
            savePersistentSettings();
          }
        } else if (serialBuffer.startsWith("SET_PWM=") || serialBuffer.startsWith("SET_SPEED=")) {
          int idx = serialBuffer.indexOf('=');
          int val = serialBuffer.substring(idx + 1).toInt();
          if (val >= 200 && val <= 1023) {
            motorSpeedPwm = val;
            savePersistentSettings();
          }
        }
        serialBuffer = "";
      }
    } else {
      if (serialBuffer.length() < 64) {
        serialBuffer += c;
      }
    }
  }
}

void loop() {
  // Handle USB / Web Serial commands
  handleSerialCommands();

  // Handle Captive Portal DNS resolution
  dnsServer.processNextRequest();

  // Handle HTTP client requests
  server.handleClient();
  MDNS.update();

  // Handle Hawa IoT WebSocket, Heartbeats, and Wireless OTA
  Hawa.loop();

  // Run non-blocking motion state machine
  updateStateMachine();

  // ==========================================
  // Inbuilt Status LED Indication
  // ==========================================
  uint32_t now = millis();

  // 1. Web / Serial Command Acknowledgement Strobe (Fast 50ms toggle)
  if (now < commandBlinkEndMs) {
    if (now - lastCommandLedToggle >= 50) {
      lastCommandLedToggle = now;
      commandLedState = !commandLedState;
      int val = commandLedState ? LOW : HIGH;
      digitalWrite(PIN_LED_STATUS, val);
#ifdef LED_BUILTIN
      if (LED_BUILTIN != PIN_LED_STATUS) digitalWrite(LED_BUILTIN, val);
#endif
    }
  }
  // 2. Emergency Stop State: Rapid warning strobe (100ms)
  else if (currentState == STATE_EMERGENCY_STOP) {
    if (now - lastLedBlink >= 100) {
      lastLedBlink = now;
      int s = !digitalRead(PIN_LED_STATUS);
      digitalWrite(PIN_LED_STATUS, s);
#ifdef LED_BUILTIN
      if (LED_BUILTIN != PIN_LED_STATUS) digitalWrite(LED_BUILTIN, s);
#endif
    }
  }
  // 3. Homing State: Smooth alert pulse (150ms)
  else if (currentState == STATE_HOMING) {
    if (now - lastLedBlink >= 150) {
      lastLedBlink = now;
      int s = !digitalRead(PIN_LED_STATUS);
      digitalWrite(PIN_LED_STATUS, s);
#ifdef LED_BUILTIN
      if (LED_BUILTIN != PIN_LED_STATUS) digitalWrite(LED_BUILTIN, s);
#endif
    }
  }
  // 4. Active Motion (Sequence Running): Fast active blinking (80ms)
  else if (currentStep > 0 && (abs(currentM1Pwm) > 0 || abs(currentM2Pwm) > 0)) {
    if (now - lastLedBlink >= 80) {
      lastLedBlink = now;
      int s = !digitalRead(PIN_LED_STATUS);
      digitalWrite(PIN_LED_STATUS, s);
#ifdef LED_BUILTIN
      if (LED_BUILTIN != PIN_LED_STATUS) digitalWrite(LED_BUILTIN, s);
#endif
    }
  }
  // 5. Idle State: Heartbeat pulse once every second
  else {
    if (now - lastLedBlink >= 1000) {
      lastLedBlink = now;
      digitalWrite(PIN_LED_STATUS, LOW);
#ifdef LED_BUILTIN
      if (LED_BUILTIN != PIN_LED_STATUS) digitalWrite(LED_BUILTIN, LOW);
#endif
      delay(20);
      digitalWrite(PIN_LED_STATUS, HIGH);
#ifdef LED_BUILTIN
      if (LED_BUILTIN != PIN_LED_STATUS) digitalWrite(LED_BUILTIN, HIGH);
#endif
    }
  }
}
