#ifndef HAWA_OTA_H
#define HAWA_OTA_H

#include <Arduino.h>

#if defined(ESP32)
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <esp_ota_ops.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266httpUpdate.h>
#endif

typedef void (*OTAProgressCallback)(int percent, size_t written, size_t total);
typedef void (*OTAStatusCallback)(bool success, const String& message);

class HawaOTA {
public:
#if defined(ESP32)
    static bool performOTA(const String& downloadUrl, size_t expectedSize, const String& expectedMD5, 
                           OTAProgressCallback progressCb, OTAStatusCallback statusCb) {
        HTTPClient http;
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        http.setTimeout(30000);

        bool isHttps = downloadUrl.startsWith("https://");

        WiFiClient plainClient;
        WiFiClientSecure secureClient;
        WiFiClient* clientPtr = nullptr;

        if (isHttps) {
            secureClient.setInsecure(); // Bypass CA verification for flexible Cloudflare / tunnel routing
            clientPtr = &secureClient;
        } else {
            clientPtr = &plainClient;
        }

        if (!http.begin(*clientPtr, downloadUrl)) {
            if (statusCb) statusCb(false, "Failed to connect to download URL");
            return false;
        }

        int httpCode = http.GET();
        if (httpCode != HTTP_CODE_OK) {
            String err = "HTTP GET failed, error: " + String(httpCode) + " (" + http.errorToString(httpCode) + ")";
            http.end();
            if (statusCb) statusCb(false, err);
            return false;
        }

        int contentLength = http.getSize();
        if (contentLength <= 0 && expectedSize > 0) {
            contentLength = expectedSize;
        }

        if (contentLength <= 0) {
            http.end();
            if (statusCb) statusCb(false, "Invalid content length for firmware binary");
            return false;
        }

        bool canBegin = Update.begin(contentLength, U_FLASH);
        if (!canBegin) {
            String err = "Not enough space to begin OTA: " + String(Update.errorString());
            http.end();
            if (statusCb) statusCb(false, err);
            return false;
        }

        if (expectedMD5.length() == 32) {
            Update.setMD5(expectedMD5.c_str());
        }

        WiFiClient* stream = http.getStreamPtr();
        uint8_t buff[1024];
        size_t written = 0;
        int lastPercent = -1;

        while (http.connected() && (written < contentLength || contentLength == -1)) {
            size_t sizeAvailable = stream->available();
            if (sizeAvailable > 0) {
                int c = stream->readBytes(buff, ((sizeAvailable > sizeof(buff)) ? sizeof(buff) : sizeAvailable));
                Update.write(buff, c);
                written += c;

                if (contentLength > 0) {
                    int percent = (written * 100) / contentLength;
                    if (percent != lastPercent && percent % 2 == 0) {
                        lastPercent = percent;
                        if (progressCb) progressCb(percent, written, contentLength);
                    }
                }
            } else {
                delay(1);
            }

            if (contentLength > 0 && written >= (size_t)contentLength) {
                break;
            }
        }

        if (Update.end()) {
            if (Update.isFinished()) {
                http.end();
                if (statusCb) statusCb(true, "OTA Update written successfully. Rebooting...");
                return true;
            } else {
                String err = "Update not finished: " + String(Update.errorString());
                http.end();
                if (statusCb) statusCb(false, err);
                return false;
            }
        } else {
            String err = "Update error: " + String(Update.errorString());
            http.end();
            if (statusCb) statusCb(false, err);
            return false;
        }
    }

    static void validateCurrentApp() {
        esp_ota_mark_app_valid_cancel_rollback();
    }

#elif defined(ESP8266)
    static bool performOTA(const String& downloadUrl, size_t expectedSize, const String& expectedMD5,
                           OTAProgressCallback progressCb, OTAStatusCallback statusCb) {
        bool isHttps = downloadUrl.startsWith("https://");
        WiFiClient plainClient;
        WiFiClientSecure secureClient;
        WiFiClient* clientPtr = nullptr;

        if (isHttps) {
            secureClient.setInsecure();
            clientPtr = &secureClient;
        } else {
            clientPtr = &plainClient;
        }

        ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

        ESPhttpUpdate.onProgress([progressCb](int current, int total) {
            if (total > 0) {
                int percent = (current * 100) / total;
                if (progressCb) progressCb(percent, current, total);
            }
        });

        if (expectedMD5.length() == 32) {
            ESPhttpUpdate.setMD5sum(expectedMD5.c_str());
        }

        t_httpUpdate_return ret = ESPhttpUpdate.update(*clientPtr, downloadUrl);

        switch (ret) {
            case HTTP_UPDATE_FAILED: {
                String err = "HTTP_UPDATE_FAILED: " + ESPhttpUpdate.getLastErrorString();
                if (statusCb) statusCb(false, err);
                return false;
            }
            case HTTP_UPDATE_NO_UPDATES: {
                if (statusCb) statusCb(false, "HTTP_UPDATE_NO_UPDATES");
                return false;
            }
            case HTTP_UPDATE_OK: {
                if (statusCb) statusCb(true, "OTA Update OK. Rebooting...");
                return true;
            }
        }
        return false;
    }

    static void validateCurrentApp() {}
#endif
};

#endif // HAWA_OTA_H
