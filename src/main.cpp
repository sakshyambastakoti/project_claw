#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>
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
int unwindSpeedPwm    = DEFAULT_UNWIND_SPEED_PWM;
uint32_t deployTimeMs = DEFAULT_DEPLOY_TIME_MS;
uint32_t retractTimeMs= DEFAULT_RETRACT_TIME_MS;
uint32_t demoHoldMs   = DEFAULT_DEMO_HOLD_MS;

// Active Motion Timing Tracking
uint32_t motionStartTime = 0;
uint32_t activeDuration  = 0;

// Current Live PWM outputs (positive = pull/forward, negative = unwind/reverse)
int currentM1Pwm = 0;
int currentM2Pwm = 0;

// Web Server on port 80 & OTA Update Server
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

// Status LED timing
uint32_t lastLedBlink = 0;

// Serial Command Buffer
String serialBuffer = "";

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

// Control Motor 1 (direction: +1 = Forward/Contract, -1 = Reverse/Release)
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

// Control Motor 2 (direction: +1 = Forward/Retract, -1 = Reverse/Release)
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

// ==========================================
// High-Level Motion Commands
// ==========================================

void startDeploy() {
  stopAllMotors();
  delay(MOTOR_DEADTIME_MS); // Prevent shoot-through / backlash

  currentState = STATE_DEPLOYING;
  motionStartTime = millis();
  activeDuration = deployTimeMs;

  // Motor 1 pulls Red cable (Deploy / Contract)
  // Motor 2 runs in reverse to unwind / pay out Blue cable
  setMotor1(MOTOR_DIR_PULL, motorSpeedPwm);
  setMotor2(MOTOR_DIR_RELEASE, unwindSpeedPwm);

  Serial.printf("[CLAW] Started Deploy (Duration: %u ms | M1 Pull PWM: %d, M2 Unwind PWM: %d)\n",
                activeDuration, motorSpeedPwm, unwindSpeedPwm);
  Hawa.log("[CLAW] Deploy started (M1 Pull: " + String(motorSpeedPwm) + ", M2 Unwind: " + String(unwindSpeedPwm) + ")");
  Hawa.sendData("state", "DEPLOYING");
}

void startRetract() {
  stopAllMotors();
  delay(MOTOR_DEADTIME_MS);

  currentState = STATE_RETRACTING;
  motionStartTime = millis();
  activeDuration = retractTimeMs;

  // Motor 2 pulls Blue cable (Retract)
  // Motor 1 runs in reverse to unwind / pay out Red cable
  setMotor2(MOTOR_DIR_PULL, motorSpeedPwm);
  setMotor1(MOTOR_DIR_RELEASE, unwindSpeedPwm);

  Serial.printf("[CLAW] Started Retract (Duration: %u ms | M2 Pull PWM: %d, M1 Unwind PWM: %d)\n",
                activeDuration, motorSpeedPwm, unwindSpeedPwm);
  Hawa.log("[CLAW] Retract started (M2 Pull: " + String(motorSpeedPwm) + ", M1 Unwind: " + String(unwindSpeedPwm) + ")");
  Hawa.sendData("state", "RETRACTING");
}

void startDemo() {
  stopAllMotors();
  delay(MOTOR_DEADTIME_MS);

  currentState = STATE_DEMO_DEPLOYING;
  motionStartTime = millis();
  activeDuration = deployTimeMs;

  // Demo Deploy: Motor 1 pulls, Motor 2 unwinds
  setMotor1(MOTOR_DIR_PULL, motorSpeedPwm);
  setMotor2(MOTOR_DIR_RELEASE, unwindSpeedPwm);

  Serial.printf("[CLAW] Started Full Demo Sequence (Deploy phase)\n");
  Hawa.log("[CLAW] Auto Demo sequence initiated");
  Hawa.sendData("state", "DEMO_DEPLOYING");
}

void triggerEmergencyStop() {
  stopAllMotors();
  currentState = STATE_EMERGENCY_STOP;
  activeDuration = 0;
  Serial.println("[CLAW] *** EMERGENCY STOP TRIGGERED ***");
  Hawa.log("[CLAW] *** EMERGENCY STOP TRIGGERED ***");
  Hawa.sendData("state", "EMERGENCY_STOP");
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
        stopAllMotors();
        currentState = STATE_DEPLOYED;
        activeDuration = 0;
        Serial.println("[CLAW] Deploy cycle finished.");
        Hawa.log("[CLAW] Deploy completed");
        Hawa.sendData("state", "DEPLOYED");
      }
      break;

    case STATE_RETRACTING:
      if (elapsed >= activeDuration) {
        stopAllMotors();
        currentState = STATE_RETRACTED;
        activeDuration = 0;
        Serial.println("[CLAW] Retract cycle finished.");
        Hawa.log("[CLAW] Retract completed");
        Hawa.sendData("state", "RETRACTED");
      }
      break;

    case STATE_DEMO_DEPLOYING:
      if (elapsed >= activeDuration) {
        stopAllMotors();
        currentState = STATE_DEMO_HOLDING;
        motionStartTime = millis();
        activeDuration = demoHoldMs;
        Serial.printf("[CLAW] Demo: Holding claws open for %u ms...\n", demoHoldMs);
        Hawa.sendData("state", "DEMO_HOLDING");
      }
      break;

    case STATE_DEMO_HOLDING:
      if (elapsed >= activeDuration) {
        currentState = STATE_DEMO_RETRACTING;
        motionStartTime = millis();
        activeDuration = retractTimeMs;
        // Demo Retract: Motor 2 pulls Blue cable, Motor 1 unwinds Red cable
        setMotor2(MOTOR_DIR_PULL, motorSpeedPwm);
        setMotor1(MOTOR_DIR_RELEASE, unwindSpeedPwm);
        Serial.println("[CLAW] Demo: Retracting claws (M2 Pull, M1 Unwind)...");
        Hawa.sendData("state", "DEMO_RETRACTING");
      }
      break;

    case STATE_DEMO_RETRACTING:
      if (elapsed >= activeDuration) {
        stopAllMotors();
        currentState = STATE_IDLE;
        activeDuration = 0;
        Serial.println("[CLAW] Demo sequence complete. Ready.");
        Hawa.log("[CLAW] Demo sequence completed");
        Hawa.sendData("state", "IDLE");
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
  server.sendHeader("Content-Encoding", "gzip");
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.send_P(200, "text/html", (PGM_P)INDEX_HTML_GZ, INDEX_HTML_GZ_LEN);
}

void handleStatus() {
  uint32_t now = millis();
  uint32_t elapsed = (activeDuration > 0) ? (now - motionStartTime) : 0;
  uint32_t remaining = (elapsed < activeDuration) ? (activeDuration - elapsed) : 0;

  bool isWifiConnected = (WiFi.status() == WL_CONNECTED);

  String json = "{";
  json += "\"state\":\"" + getStateString() + "\",";
  json += "\"m1_pwm\":" + String(currentM1Pwm) + ",";
  json += "\"m2_pwm\":" + String(currentM2Pwm) + ",";
  json += "\"target_pwm\":" + String(motorSpeedPwm) + ",";
  json += "\"unwind_pwm\":" + String(unwindSpeedPwm) + ",";
  json += "\"total_duration\":" + String(activeDuration) + ",";
  json += "\"remaining\":" + String(remaining) + ",";
  // Wi-Fi and Hawa Wireless Status
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
    unwindSpeedPwm = motorSpeedPwm; // Follow main speed by default
  }
  if (server.hasArg("unwind_pwm")) {
    unwindSpeedPwm = constrain(server.arg("unwind_pwm").toInt(), 100, 1023);
  }
  if (server.hasArg("deploy")) {
    deployTimeMs = constrain(server.arg("deploy").toInt(), 500, 15000);
  }
  if (server.hasArg("retract")) {
    retractTimeMs = constrain(server.arg("retract").toInt(), 500, 15000);
  }
  server.send(200, "text/plain", "CONFIG_UPDATED");
}

// Wi-Fi Network Scan
void handleWifiScan() {
  int n = WiFi.scanNetworks(false, false); // synchronous scan for immediate response
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

// Wi-Fi Credentials Provisioning from Web Page
void handleWifiSave() {
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

  Serial.printf("[WiFi] Provisioning request from Web Page -> SSID: %s\n", ssid.c_str());

  if (serverUrl.length() == 0) {
    serverUrl = "wss://hawa-platform.onrender.com/ws";
  }

  // Save to persistent storage through Hawa
  Hawa.saveCredentials(ssid, pass, serverUrl, devName);

  // Attempt connection in background while keeping AP intact
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  WiFi.begin(ssid.c_str(), pass.c_str());

  Hawa.log("[WIFI] New credentials saved from Web Dashboard for SSID: " + ssid);

  String resp = "{\"status\":\"ok\",\"message\":\"Wi-Fi settings saved. Connecting to " + ssid + "...\"}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", resp);
}

// Clear Saved Wi-Fi Credentials
void handleWifiClear() {
  Hawa.clearCredentials();
  WiFi.disconnect();
  Serial.println("[WiFi] Credentials wiped via Web Page request.");

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Wi-Fi credentials cleared. Device running in AP mode.\"}");
}

// Remote Reboot Handler
void handleReboot() {
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
  Hawa.onCommand("deploy", [](const String& val) {
    Hawa.log("[HAWA CMD] Deploy requested from cloud");
    startDeploy();
  });
  Hawa.onCommand("retract", [](const String& val) {
    Hawa.log("[HAWA CMD] Retract requested from cloud");
    startRetract();
  });
  Hawa.onCommand("stop", [](const String& val) {
    Hawa.log("[HAWA CMD] Emergency stop requested from cloud");
    triggerEmergencyStop();
  });
  Hawa.onCommand("demo", [](const String& val) {
    Hawa.log("[HAWA CMD] Auto demo requested from cloud");
    startDemo();
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
  server.on("/api/deploy", HTTP_POST, handleDeploy);
  server.on("/api/retract", HTTP_POST, handleRetract);
  server.on("/api/demo", HTTP_POST, handleDemo);
  server.on("/api/stop", HTTP_POST, handleStop);
  server.on("/api/config", HTTP_POST, handleConfig);

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
  Serial.println("[CLAW] System initialized in IDLE state.");
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
        if (serialBuffer == "DEPLOY") {
          startDeploy();
        } else if (serialBuffer == "RETRACT") {
          startRetract();
        } else if (serialBuffer == "STOP") {
          triggerEmergencyStop();
        } else if (serialBuffer == "DEMO") {
          startDemo();
        } else if (serialBuffer.startsWith("SET_PWM=")) {
          int p = serialBuffer.substring(8).toInt();
          if (p >= 200 && p <= 1023) {
            motorSpeedPwm = p;
            unwindSpeedPwm = p;
            Serial.printf("[CONFIG] Motor and unwind PWM set to %d\n", p);
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

  // Status LED indication
  uint32_t now = millis();
  if (currentState == STATE_EMERGENCY_STOP) {
    // Rapid flashing for emergency stop
    if (now - lastLedBlink >= 100) {
      lastLedBlink = now;
      digitalWrite(PIN_LED_STATUS, !digitalRead(PIN_LED_STATUS));
    }
  } else if (abs(currentM1Pwm) > 0 || abs(currentM2Pwm) > 0) {
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
