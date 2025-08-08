#pragma once

#include <ESP8266WiFi.h>

#include "EEPROM_Utils.h"
#include "basic_defines.h"
#include "persistentVars.h"

#define WIFI_CONNECTION_TIMEOUT_MS                                       (10000)
#define WIFI_CONNECTION_INTERVAL_MS                             (60 * 60 * 1000) // 60 min
#define WIFI_CONNECTION_MAX_CONSECUTIVE_ATTEMPTS                             (5)

#define WEATHER_UPDATE_INTERVAL_MS                              (30 * 60 * 1000) // 30 min
#define WEATHER_UPDATE_MAX_CONSECUTIVE_ATTEMPTS                             (10)
#define WEATHER_SERVER_REQUEST_TIMEOUT                                    (2000)

#define WIFI_SCAN_NOT_SEEN_MAX_COUNT                                         (5)
#define WIFI_SCAN_MINIMUM_RSSI_FOR_TRACKING                                (-80)
#define WIFI_SCAN_MAX_TRACKED_NETWORKS_COUNT                                (10)

#define TEMPERATURE_DEGREE_INVALID                                      (0xFFFF)

enum WifiErrorType {
    WIFI_ERROR_NONE                       = (0 << 0),
    WIFI_ERROR_CONNECTION_FAILED_MASK     = (1 << 0),
    WIFI_ERROR_WEATHER_UPDATE_FAILED_MASK = (1 << 1),
};

typedef uint8_t WifiErrorType_t;

extern struct Settings settings;

struct WifiNetworkInfo {
    String ssid;
    String bssid;
    int channel;
    String encryption;
    float avgRssi;
    int rssiSamples;
    int notSeenCount    = 0;
    bool seenInThisScan = false;

    // Constructor por defecto necesario para vector::resize
    WifiNetworkInfo() = default;

    WifiNetworkInfo(String ssid_, String bssid_, int channel_, String encryption_, int rssi_)
        : ssid(ssid_),
          bssid(bssid_),
          channel(channel_),
          encryption(encryption_),
          avgRssi(rssi_),
          rssiSamples(1),
          notSeenCount(0),
          seenInThisScan(true) {}
};

static std::vector<WifiNetworkInfo> wifiNetworks;
static bool wifiScanInProgress           = false;
static unsigned long lastConnectUpdateMs = WIFI_CONNECTION_TIMEOUT_MS;
static unsigned long lastWeatherUpdateMs = WEATHER_UPDATE_INTERVAL_MS;
static double TemperatureDegree          = TEMPERATURE_DEGREE_INVALID;
static bool wifiError                    = false;
static bool weatherError                 = false;

bool ESP8266Utils_get_TemperatureDegree(double* temperature) {
    if ((temperature == nullptr) || (TemperatureDegree == TEMPERATURE_DEGREE_INVALID)) {
        return false; // Invalid temperature or null pointer
    }
    *temperature = TemperatureDegree;
    return true;
}

bool ESP8266Utils_update_WeatherData(void) {
    bool success = false;

    Serial.println("Updating weather data...");

    if (String(settings.openWeatherMapSettings.appid) == String("")) {
        Serial.println("Empty Weather API key");
    } else {
        WiFiClient client;
        client.setTimeout(WEATHER_SERVER_REQUEST_TIMEOUT);

        if (!client.connect(OPENWEATHERMAP_HOST, OPENWEATHERMAP_PORT)) {
            Serial.println("Connection failed");
        } else {
            String HTTPrequest = OPENWEATHERMAP_HTTP_REQUEST(
                    settings.openWeatherMapSettings.appid,
                    settings.openWeatherMapSettings.lat,
                    settings.openWeatherMapSettings.lon);
            client.println(HTTPrequest);
            client.println("Host: " + String(OPENWEATHERMAP_HOST));
            client.println("Connection: close");

            if (client.println() == 0) {
                Serial.println("Failed to send request");
            } else if (!client.find("\r\n\r\n")) {
                Serial.println("Invalid response");
            } else if (client.find("\"temp\":")) {
                double NewTemp = client.readStringUntil(',').toDouble();
                if (NewTemp > 273) {
                    NewTemp -= 273.15;
                }
                Serial.print("Temperature: ");
                Serial.println(NewTemp);
                TemperatureDegree = NewTemp;

                if (client.find("\"timezone\":")) {
                    long timezoneshift = strtol(client.readStringUntil(',').c_str(), NULL, 10);
                    Serial.print("Timezone shift: ");
                    Serial.println(timezoneshift);
                    struct rtcTime_t rtcTime
                            = { .myTime = time(NULL), .timezoneShift = timezoneshift };
                    persistentVars_store_rtcTime(&rtcTime);
                    success = true;
                }
            }
        }
        client.stop();
    }
    lastWeatherUpdateMs = millis();
    return success;
}

void ESP8266Utils_clearWifiNetworksList() {
    wifiNetworks.clear();
    Serial.println("Lista de redes WiFi borrada completamente.");
}

// Compara por RSSI promedio, descendente
bool compareByRssiDesc(const WifiNetworkInfo& a, const WifiNetworkInfo& b) {
    return a.avgRssi > b.avgRssi;
}

// Elimina redes no vistas en los últimos 5 escaneos
void cleanNetworksNotSeen() {
    for (auto it = wifiNetworks.begin(); it != wifiNetworks.end();) {
        if (!it->seenInThisScan) {
            it->notSeenCount++;
        } else {
            it->notSeenCount = 0;
        }

        if (it->notSeenCount >= WIFI_SCAN_NOT_SEEN_MAX_COUNT) {
            it = wifiNetworks.erase(it);
        } else {
            ++it;
        }
    }
}

// Inserta o actualiza una red escaneada
void updateOrInsertNetwork(
        const String& ssid,
        const String& bssid,
        int channel,
        const String& encryption,
        int rssi) {
    for (auto& net : wifiNetworks) {
        if (net.bssid == bssid) {
            net.avgRssi = (net.avgRssi * net.rssiSamples + rssi) / (net.rssiSamples + 1);
            net.rssiSamples++;
            net.seenInThisScan = true;
            net.notSeenCount   = 0;
            return;
        }
    }

    // Filtro para evitar agregar redes volátiles de 1 sola muestra
    if (rssi < WIFI_SCAN_MINIMUM_RSSI_FOR_TRACKING) {
        // Si la red es débil y no estaba antes, ignorarla
        return;
    }

    wifiNetworks.emplace_back(ssid, bssid, channel, encryption, rssi);
}

// Inicia el escaneo WiFi si no está en curso
void ESP8266Utils_startWifiScanIfNeeded(void) {
    if (!wifiScanInProgress) {
        if (WiFi.status() == WL_CONNECTED) {
            WiFi.disconnect();
        }
        WiFi.mode(WIFI_STA);
        WiFi.scanNetworks(true); // Async scan
        wifiScanInProgress = true;
        Serial.println("Started async WiFi scan...");
    }
}

// Verifica si ya terminó el escaneo y actualiza la lista
void ESP8266Utils_checkScanResults() {
    if (wifiScanInProgress) {
        int n = WiFi.scanComplete();
        if (n == WIFI_SCAN_RUNNING) {
            return;
        } else if (n < 0) {
            Serial.println("Scan failed");
            wifiScanInProgress = false;
            return;
        }

        wifiScanInProgress = false;

        // Marcar todas como no vistas inicialmente
        for (auto& net : wifiNetworks) {
            net.seenInThisScan = false;
        }

        for (int i = 0; i < n; ++i) {
            String ssid           = WiFi.SSID(i);
            int32_t rssi          = WiFi.RSSI(i);
            String bssid          = WiFi.BSSIDstr(i);
            uint8_t channel       = WiFi.channel(i);
            String encryptionType = WiFi.encryptionType(i) == ENC_TYPE_NONE ? "None" : "Encrypted";

            updateOrInsertNetwork(ssid, bssid, channel, encryptionType, rssi);
        }

        cleanNetworksNotSeen();

        std::sort(wifiNetworks.begin(), wifiNetworks.end(), compareByRssiDesc);

        if (wifiNetworks.size() > WIFI_SCAN_MAX_TRACKED_NETWORKS_COUNT) {
            wifiNetworks.resize(WIFI_SCAN_MAX_TRACKED_NETWORKS_COUNT);
        }

        Serial.println("Lista de redes ordenada por RSSI promedio:");
        for (size_t i = 0; i < wifiNetworks.size(); ++i) {
            const auto& net = wifiNetworks[i];
            Serial.printf(
                    "%d: %s (%0.1f dBm avg, %d muestras) [%s] Canal: %d, Encriptación: %s\n",
                    int(i + 1),
                    net.ssid.c_str(),
                    net.avgRssi,
                    net.rssiSamples,
                    net.bssid.c_str(),
                    net.channel,
                    net.encryption.c_str());
        }
    }
}

bool ESP8266Utils_getSsidAtIndex(int index, String& outSsid) {
    if (index < 0 || index >= static_cast<int>(wifiNetworks.size())) {
        return false;
    }
    outSsid = wifiNetworks[index].ssid;
    return true;
}

int ESP8266Utils_getTrackedNetworkCount() { return static_cast<int>(wifiNetworks.size()); }

int ESP8266Utils_getIndexBySsid(const String& targetSsid) {
    for (size_t i = 0; i < wifiNetworks.size(); ++i) {
        if (wifiNetworks[i].ssid == targetSsid) {
            return static_cast<int>(i);
        }
    }
    return -1; // No encontrado
}

void ESP8266Utils_connectToWifi(const String& ssid, const String& password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    lastConnectUpdateMs = millis();
}

bool ESP8266Utils_isWifiConnected(void) { return WiFi.status() == WL_CONNECTED; }

int ESP8266Utils_getWifiConnectionPercentage(void) {
    if (lastConnectUpdateMs == 0) {
        return 0;
    }
    unsigned long elapsed = millis() - lastConnectUpdateMs;
    return (elapsed * 100) / WIFI_CONNECTION_TIMEOUT_MS;
}

bool ESP8266Utils_getWifiConnectAttemptTmo(void) {
    if (lastConnectUpdateMs == 0) {
        return true; // No hay intento previo, se considera un timeout
    }
    return (millis() - lastConnectUpdateMs) >= WIFI_CONNECTION_INTERVAL_MS;
}

bool ESP8266Utils_getWifiWeatherAttemptTmo(void) {
    if (lastWeatherUpdateMs == 0) {
        return true; // No hay intento previo, se considera un timeout
    }
    return (millis() - lastWeatherUpdateMs) >= WEATHER_UPDATE_INTERVAL_MS;
}

WifiErrorType_t ESP8266Utils_get_errors(void) {
    WifiErrorType_t error_bitmask = WIFI_ERROR_NONE;
    if (wifiError) {
        error_bitmask |= WIFI_ERROR_CONNECTION_FAILED_MASK;
    }
    if (weatherError) {
        error_bitmask |= WIFI_ERROR_WEATHER_UPDATE_FAILED_MASK;
    }
    return error_bitmask;
}

void Wifi_handler(bool inConfigMode) {
    // Persistent state between calls
    static unsigned long connectAttemptCount = 0;
    static unsigned long weatherUpdateCount  = 0;
    static bool backFromConfigMode           = false; // This is to force immediate reconnection
                                                      // attempts after leaving config mode
    if (inConfigMode) {
        connectAttemptCount = 0;
        weatherUpdateCount  = 0;
        backFromConfigMode  = true; // Force immediate actions after config mode
        return;
    }

    bool wifiEnabled   = settings.wifiSettings.wifi_enabled;
    bool wifiConnected = ESP8266Utils_isWifiConnected();
    bool WifiTmo       = ESP8266Utils_getWifiConnectAttemptTmo() || backFromConfigMode;
    bool WeatherTmo    = ESP8266Utils_getWifiWeatherAttemptTmo() || backFromConfigMode;

    // ===== WiFi auto connection handling =====
    if (wifiEnabled && wifiConnected && connectAttemptCount > 0) {
        Serial.println("Connected to WiFi");
        connectAttemptCount = 0;
        wifiError           = false;
    } else if ((wifiEnabled && !wifiConnected) && WifiTmo) {
        if (++connectAttemptCount < WIFI_CONNECTION_MAX_CONSECUTIVE_ATTEMPTS) {
            Serial.println("Attempt " + String(connectAttemptCount) + " to connect to WiFi...");
            ESP8266Utils_connectToWifi(
                    settings.wifiSettings.ssid_sta,
                    settings.wifiSettings.password_sta);
        } else {
            wifiError = true;
        }
    } else if (!wifiEnabled && wifiConnected) {
        Serial.println("Disconnecting WiFi...");
        WiFi.disconnect();
        connectAttemptCount = 0;
        wifiError           = false;
    }

    // ===== Weather update handling =====
    if (wifiEnabled && wifiConnected && WeatherTmo) {
        if (ESP8266Utils_update_WeatherData()) {
            weatherUpdateCount = 0;
            weatherError       = false;
        } else if (++weatherUpdateCount > WEATHER_UPDATE_MAX_CONSECUTIVE_ATTEMPTS) {
            TemperatureDegree = TEMPERATURE_DEGREE_INVALID;
            Serial.println("Attempt " + String(weatherUpdateCount) + " to update weather failed.");
            weatherError = true;
        }
    } else if (!wifiConnected) {
        weatherUpdateCount = 0;
        TemperatureDegree  = TEMPERATURE_DEGREE_INVALID;
        weatherError       = false;
    }

    backFromConfigMode = false; // Indicate we are no longer back from config mode
}

