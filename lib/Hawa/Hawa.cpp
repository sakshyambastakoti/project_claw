#include "Hawa.h"

HawaClass Hawa;

// Global forwarders for OTA C-style callbacks
static void globalOtaProgress(int percent, size_t written, size_t total) {
    Hawa._onOTAProgress(percent, written, total);
}

static void globalOtaStatus(bool success, const String& message) {
    Hawa._onOTAStatus(success, message);
}

HawaClass::HawaClass() {
    _isOtaRunning = false;
    _isConnected = false;
    _lastHeartbeat = 0;
}

void HawaClass::begin() {
    _config.begin();

    // Generate unique hardware device identifier
    String mac = WiFi.macAddress();
    mac.replace(":", "");
#if defined(ESP32)
    _deviceId = "hawa-esp32-" + mac;
#elif defined(ESP8266)
    _deviceId = "hawa-esp8266-" + mac;
#endif

    Serial.println("\n==================================");
    Serial.println("  HAWA IOT AGENT INITIALIZING");
    Serial.println("  Device ID: " + _deviceId);
    Serial.println("==================================");

    // Validate current app partition for rollback safety
    HawaOTA::validateCurrentApp();

    if (_config.hasWifiCredentials()) {
        Serial.printf("[HAWA] Loaded Stored Wi-Fi: %s\n", _config.ssid.c_str());
        Serial.printf("[HAWA] Loaded Hub Server:   %s\n", _config.serverUrl.c_str());
        _initNetwork();
        _setupWebSocket();
    } else {
        Serial.println("[HAWA] No Wi-Fi credentials in flash.");
        Serial.println("[HAWA] Ready for Web Dashboard provisioning or serial setup.");
    }
}

void HawaClass::begin(const char* ssid, const char* pass, const char* serverUrl, const char* deviceName) {
    _config.begin();
    _config.saveCredentials(ssid, pass, serverUrl, deviceName);
    begin();
}

void HawaClass::connectWifi(const String& newSsid, const String& newPass, const String& newServer, const String& newName) {
    _config.saveCredentials(newSsid, newPass, newServer, newName);
    _initNetwork();
    _setupWebSocket();
}

void HawaClass::_initNetwork() {
    // Preserve SoftAP while connecting Station (Dual AP+STA mode)
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(_config.ssid.c_str(), _config.password.c_str());

    Serial.printf("[WIFI] Connecting to %s", _config.ssid.c_str());
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(300);
        Serial.print(".");
        attempts++;
        checkSerialCommands();
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\n[WIFI] Connected! Station IP: %s | RSSI: %d dBm\n", 
                      WiFi.localIP().toString().c_str(), WiFi.RSSI());
    } else {
        Serial.println("\n[WIFI] Station connection pending. Access Point remains active.");
    }
}

void HawaClass::_setupWebSocket() {
    String server = _config.serverUrl;
    if (server.length() == 0) return;

    bool isSecure = false;
    if (server.startsWith("wss://")) {
        isSecure = true;
        server.remove(0, 6);
    } else if (server.startsWith("ws://")) {
        isSecure = false;
        server.remove(0, 5);
    } else if (server.startsWith("https://")) {
        isSecure = true;
        server.remove(0, 8);
    } else if (server.startsWith("http://")) {
        isSecure = false;
        server.remove(0, 7);
    }

    int slashIdx = server.indexOf('/');
    String host = (slashIdx >= 0) ? server.substring(0, slashIdx) : server;
    String path = (slashIdx >= 0) ? server.substring(slashIdx) : "/ws";
    if (path.length() == 0 || path == "/") path = "/ws";

    int port = isSecure ? 443 : 80;
    int colonIdx = host.indexOf(':');
    if (colonIdx >= 0) {
        port = host.substring(colonIdx + 1).toInt();
        host = host.substring(0, colonIdx);
    }

    Serial.printf("[WS] Configuring %s://%s:%d%s\n", isSecure ? "wss" : "ws", host.c_str(), port, path.c_str());

    if (isSecure) {
        _webSocket.beginSSL(host.c_str(), port, path.c_str());
    } else {
        _webSocket.begin(host.c_str(), port, path.c_str());
    }

    _webSocket.onEvent([this](WStype_t type, uint8_t * payload, size_t length) {
        this->_webSocketEvent(type, payload, length);
    });

    _webSocket.setReconnectInterval(5000);
}

void HawaClass::loop() {
    checkSerialCommands();

    if (WiFi.status() == WL_CONNECTED) {
        _webSocket.loop();

        if (millis() - _lastHeartbeat > 15000 && !_isOtaRunning && _isConnected) {
            _lastHeartbeat = millis();
            _sendHeartbeat();
        }
    }
}

void HawaClass::_webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            _isConnected = false;
            Serial.println("[WS] Disconnected from Hawa Hub");
            break;

        case WStype_CONNECTED:
            _isConnected = true;
            Serial.println("[WS] Connected to Hawa Hub! Sending CLIENT_HELLO...");
            _sendClientHello();
            break;

        case WStype_TEXT: {
            DynamicJsonDocument doc(1024);
            DeserializationError err = deserializeJson(doc, payload);
            if (err) return;

            String msgType = doc["type"] | "";

            // 1. Over-The-Air Update Command
            if (msgType == "OTA_START") {
                String downloadUrl = doc["downloadUrl"] | "";
                size_t size = doc["size"] | 0;
                String md5 = doc["md5"] | "";
                String targetVersion = doc["version"] | "";

                log("[OTA] Starting Over-The-Air Update from: " + downloadUrl);
                _isOtaRunning = true;

                bool ok = HawaOTA::performOTA(downloadUrl, size, md5, globalOtaProgress, globalOtaStatus);
                if (ok) {
                    if (targetVersion.length() > 0) {
                        _config.updateVersion(targetVersion);
                    }
                    delay(1000);
                    ESP.restart();
                } else {
                    _isOtaRunning = false;
                }
            }

            // 2. Remote Reboot
            if (msgType == "REBOOT") {
                log("[HAWA] Remote reboot requested from dashboard.");
                delay(500);
                ESP.restart();
            }

            // 3. User Custom Command Trigger
            if (msgType == "COMMAND") {
                String cmdName = doc["command"] | "";
                String cmdVal = doc["value"] | "";
                if (_commandCallbacks.find(cmdName) != _commandCallbacks.end()) {
                    _commandCallbacks[cmdName](cmdVal);
                }
            }
            break;
        }

        default:
            break;
    }
}

void HawaClass::_sendClientHello() {
    DynamicJsonDocument doc(512);
    doc["type"] = "CLIENT_HELLO";
    doc["deviceId"] = _deviceId;
    doc["name"] = _config.deviceName;
#if defined(ESP32)
    doc["chip"] = "ESP32 (" + String(ESP.getChipModel()) + ")";
#elif defined(ESP8266)
    doc["chip"] = "ESP8266";
#endif
    doc["mac"] = WiFi.macAddress();
    doc["ip"] = WiFi.localIP().toString();
    doc["rssi"] = WiFi.RSSI();
    doc["firmwareVersion"] = _config.firmwareVersion;
    doc["freeHeap"] = ESP.getFreeHeap();
    doc["uptime"] = millis() / 1000;

    String out;
    serializeJson(doc, out);
    _webSocket.sendTXT(out);
}

void HawaClass::_sendHeartbeat() {
    DynamicJsonDocument doc(256);
    doc["type"] = "HEARTBEAT";
    doc["deviceId"] = _deviceId;
    doc["rssi"] = WiFi.RSSI();
    doc["freeHeap"] = ESP.getFreeHeap();
    doc["uptime"] = millis() / 1000;

    String out;
    serializeJson(doc, out);
    _webSocket.sendTXT(out);
}

void HawaClass::log(const String& message) {
    Serial.println(message);
    if (_isConnected && !_isOtaRunning) {
        DynamicJsonDocument doc(512);
        doc["type"] = "SERIAL_LOG";
        doc["deviceId"] = _deviceId;
        doc["text"] = message;
        String out;
        serializeJson(doc, out);
        _webSocket.sendTXT(out);
    }
}

void HawaClass::sendData(const String& key, float value) {
    if (_isConnected && !_isOtaRunning) {
        DynamicJsonDocument doc(384);
        doc["type"] = "METRICS_REPORT";
        doc["deviceId"] = _deviceId;
        JsonObject data = doc.createNestedObject("data");
        data[key] = value;
        String out;
        serializeJson(doc, out);
        _webSocket.sendTXT(out);
    }
}

void HawaClass::sendData(const String& key, const String& value) {
    if (_isConnected && !_isOtaRunning) {
        DynamicJsonDocument doc(384);
        doc["type"] = "METRICS_REPORT";
        doc["deviceId"] = _deviceId;
        JsonObject data = doc.createNestedObject("data");
        data[key] = value;
        String out;
        serializeJson(doc, out);
        _webSocket.sendTXT(out);
    }
}

void HawaClass::onCommand(const String& command, HawaCommandCallback callback) {
    _commandCallbacks[command] = callback;
}

void HawaClass::_onOTAProgress(int percent, size_t written, size_t total) {
    if (_isConnected) {
        DynamicJsonDocument doc(256);
        doc["type"] = "OTA_PROGRESS";
        doc["deviceId"] = _deviceId;
        doc["percent"] = percent;
        doc["writtenBytes"] = written;
        doc["totalBytes"] = total;
        String out;
        serializeJson(doc, out);
        _webSocket.sendTXT(out);
    }
}

void HawaClass::_onOTAStatus(bool success, const String& message) {
    if (_isConnected) {
        DynamicJsonDocument doc(256);
        doc["type"] = "OTA_COMPLETE";
        doc["deviceId"] = _deviceId;
        doc["success"] = success;
        doc["message"] = message;
        String out;
        serializeJson(doc, out);
        _webSocket.sendTXT(out);
    }
}

void HawaClass::checkSerialCommands() {
    if (Serial.available() > 0) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) return;

        if (line.startsWith("WIFI:")) {
            int firstComma = line.indexOf(',');
            if (firstComma > 5) {
                String nSsid = line.substring(5, firstComma);
                String nPass = line.substring(firstComma + 1);
                nSsid.trim(); nPass.trim();
                _config.saveCredentials(nSsid, nPass, _config.serverUrl, _config.deviceName);
                Serial.printf("[HAWA] Saved Wi-Fi: %s. Restarting...\n", nSsid.c_str());
                delay(400);
                ESP.restart();
            }
        } else if (line.startsWith("HAWA_CONFIG:")) {
            String jsonStr = line.substring(12);
            DynamicJsonDocument doc(512);
            if (!deserializeJson(doc, jsonStr)) {
                String nSsid = doc["ssid"] | _config.ssid;
                String nPass = doc["pass"] | _config.password;
                String nServer = doc["server"] | _config.serverUrl;
                String nName = doc["name"] | _config.deviceName;
                _config.saveCredentials(nSsid, nPass, nServer, nName);
                Serial.printf("[HAWA] Saved Config for %s. Restarting...\n", nName.c_str());
                delay(400);
                ESP.restart();
            }
        }
    }
}
