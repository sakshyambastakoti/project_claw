#ifndef HAWA_H
#define HAWA_H

#include <Arduino.h>
#include <functional>
#include <map>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "HawaConfig.h"
#include "HawaOTA.h"

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#endif

typedef std::function<void(const String& payload)> HawaCommandCallback;

#if defined(ESP8266)
class HawaWebSocketsClient : public WebSocketsClient {
public:
    void loop() {
        if(_port == 0) return;
        WEBSOCKETS_YIELD();
        if(!clientIsConnected(&_client)) {
            if((millis() - _lastConnectionFail) < _reconnectInterval) {
                return;
            }

            if(_client.isSSL) {
                if(_client.ssl) {
                    delete _client.ssl;
                    _client.ssl = NULL;
                    _client.tcp = NULL;
                }
                _client.ssl = new WiFiClientSecure();
                _client.tcp = _client.ssl;
                // BearSSL memory tuning for ESP8266:
                // Reduce buffer from 16KB default to 4KB RX / 1KB TX to fit reliably in heap
                _client.ssl->setBufferSizes(4096, 1024);
                _client.ssl->setInsecure();
            } else {
                if(_client.tcp) {
                    delete _client.tcp;
                    _client.tcp = NULL;
                }
                _client.tcp = new WiFiClient();
            }

            if(!_client.tcp) {
                return;
            }
            WEBSOCKETS_YIELD();
            if(_client.tcp->connect(_host.c_str(), _port)) {
                connectedCb();
                _lastConnectionFail = 0;
            } else {
                connectFailedCb();
                _lastConnectionFail = millis();
            }
        } else {
            handleClientData();
            WEBSOCKETS_YIELD();
            if(_client.status == WSC_CONNECTED) {
                handleHBPing();
                handleHBTimeout(&_client);
            }
        }
    }

    void connectFailedCb() {
        WebSocketsClient::connectFailedCb();
        runCbEvent(WStype_ERROR, (uint8_t*)"Connect failed", 14);
    }
};
#else
typedef WebSocketsClient HawaWebSocketsClient;
#endif

class HawaClass {
private:
    HawaConfig _config;
    HawaWebSocketsClient _webSocket;
    String _deviceId;
    bool _isOtaRunning;
    bool _isConnected;
    bool _webSocketConfigured;
    unsigned long _lastHeartbeat;
    std::map<String, HawaCommandCallback> _commandCallbacks;

    void _initNetwork();
    void _setupWebSocket();
    void _webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
    void _sendClientHello();
    void _sendHeartbeat();

public:
    HawaClass();

    // Initialize with credentials stored in NVS / EEPROM
    void begin();

    // Initialize with explicit Wi-Fi and Hawa Hub URL
    void begin(const char* ssid, const char* pass, const char* serverUrl, const char* deviceName = "Project-CLAW");

    // Must be called in loop() to handle WebSocket and OTA events
    void loop();

    // Send text log to both local Serial and Hawa Web Dashboard
    void log(const String& message);

    // Send numeric telemetry data to Hawa Web Dashboard
    void sendData(const String& key, float value);

    // Send string telemetry data to Hawa Web Dashboard
    void sendData(const String& key, const String& value);

    // Register a remote command listener triggered from Hawa Dashboard
    void onCommand(const String& command, HawaCommandCallback callback);

    // Process serial provisioning commands (e.g. from Web Flasher)
    void checkSerialCommands();

    // Check if connected to Hawa Hub
    bool isConnected() const { return _isConnected; }

    // Check if OTA is currently executing
    bool isOtaRunning() const { return _isOtaRunning; }

    // Get assigned hardware device ID
    String getDeviceId() const { return _deviceId; }

    // Get current IP address
    String getIp() const { return WiFi.localIP().toString(); }

    // Get device nickname/name
    String getDeviceName() const { return _config.deviceName; }

    // Get active server URL
    String getServerUrl() const { return _config.serverUrl; }

    // Get stored Wi-Fi SSID
    String getSsid() const { return _config.ssid; }

    // Check if Wi-Fi credentials exist
    bool hasWifiCredentials() { return _config.hasWifiCredentials(); }

    // Save credentials from web page or API
    void saveCredentials(const String& newSsid, const String& newPass, const String& newServer = "", const String& newName = "") {
        _config.saveCredentials(newSsid, newPass, newServer, newName);
        _webSocketConfigured = false;
        _isConnected = false;
        _webSocket.disconnect();
    }

    // Connect to Wi-Fi without wiping AP mode
    void connectWifi(const String& newSsid, const String& newPass, const String& newServer = "", const String& newName = "");

    // Clear saved credentials
    void clearCredentials() { _config.clear(); }

    // Internal callbacks
    void _onOTAProgress(int percent, size_t written, size_t total);
    void _onOTAStatus(bool success, const String& message);
};

extern HawaClass Hawa;

#endif // HAWA_H
