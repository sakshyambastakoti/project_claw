#ifndef HAWA_CONFIG_H
#define HAWA_CONFIG_H

#include <Arduino.h>

#if defined(ESP32)
#include <Preferences.h>
#elif defined(ESP8266)
#include <EEPROM.h>
#endif

class HawaConfig {
private:
#if defined(ESP32)
    Preferences prefs;
#endif

public:
    String ssid;
    String password;
    String serverUrl;
    String deviceName;
    String firmwareVersion;

    HawaConfig() {
        firmwareVersion = "1.0.0";
    }

    void begin() {
#if defined(ESP32)
        prefs.begin("hawa", false);
        ssid = prefs.getString("ssid", "");
        password = prefs.getString("pass", "");
        serverUrl = prefs.getString("server", "");
        deviceName = prefs.getString("name", "Hawa-ESP32");
        firmwareVersion = prefs.getString("ver", "1.0.0");
#elif defined(ESP8266)
        EEPROM.begin(512);
        char sBuf[64] = {0}, pBuf[64] = {0}, uBuf[128] = {0}, nBuf[32] = {0};
        EEPROM.get(0, sBuf);
        EEPROM.get(64, pBuf);
        EEPROM.get(128, uBuf);
        EEPROM.get(256, nBuf);
        sBuf[sizeof(sBuf) - 1] = '\0';
        pBuf[sizeof(pBuf) - 1] = '\0';
        uBuf[sizeof(uBuf) - 1] = '\0';
        nBuf[sizeof(nBuf) - 1] = '\0';

        // Check if EEPROM is fresh / uninitialized (0xFF)
        if ((uint8_t)sBuf[0] == 0xFF) sBuf[0] = '\0';
        if ((uint8_t)pBuf[0] == 0xFF) pBuf[0] = '\0';
        if ((uint8_t)uBuf[0] == 0xFF) uBuf[0] = '\0';
        if ((uint8_t)nBuf[0] == 0xFF) nBuf[0] = '\0';

        ssid = String(sBuf);
        password = String(pBuf);
        serverUrl = String(uBuf);
        if (serverUrl.length() == 0) {
            serverUrl = "wss://hawa-platform.onrender.com/ws";
        }
        deviceName = (String(nBuf).length() > 0) ? String(nBuf) : "Project-CLAW";
#endif
    }

    bool hasWifiCredentials() {
        return (ssid.length() > 0);
    }

    void saveCredentials(const String& newSsid, const String& newPass, const String& newServer, const String& newName) {
#if defined(ESP32)
        prefs.begin("hawa", false);
        if (newSsid.length() > 0) prefs.putString("ssid", newSsid);
        if (newPass.length() >= 0) prefs.putString("pass", newPass);
        if (newServer.length() > 0) prefs.putString("server", newServer);
        if (newName.length() > 0) prefs.putString("name", newName);
        prefs.end();
#elif defined(ESP8266)
        char sBuf[64] = {0}, pBuf[64] = {0}, uBuf[128] = {0}, nBuf[32] = {0};
        strncpy(sBuf, newSsid.c_str(), sizeof(sBuf) - 1);
        strncpy(pBuf, newPass.c_str(), sizeof(pBuf) - 1);
        strncpy(uBuf, newServer.c_str(), sizeof(uBuf) - 1);
        strncpy(nBuf, newName.c_str(), sizeof(nBuf) - 1);
        EEPROM.put(0, sBuf);
        EEPROM.put(64, pBuf);
        EEPROM.put(128, uBuf);
        EEPROM.put(256, nBuf);
        EEPROM.commit();
#endif
        ssid = newSsid;
        password = newPass;
        if (newServer.length() > 0) serverUrl = newServer;
        if (newName.length() > 0) deviceName = newName;
    }

    void updateVersion(const String& newVer) {
#if defined(ESP32)
        prefs.begin("hawa", false);
        prefs.putString("ver", newVer);
        prefs.end();
#endif
        firmwareVersion = newVer;
    }

    void clear() {
#if defined(ESP32)
        prefs.begin("hawa", false);
        prefs.clear();
        prefs.end();
#elif defined(ESP8266)
        for (int i = 0; i < 512; i++) EEPROM.write(i, 0);
        EEPROM.commit();
#endif
        ssid = "";
        password = "";
        serverUrl = "";
        deviceName = "Project-CLAW";
    }
};

#endif // HAWA_CONFIG_H
