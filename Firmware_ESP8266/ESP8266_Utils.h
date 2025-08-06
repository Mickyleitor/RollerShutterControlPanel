#pragma once

#include <ESP8266WiFi.h>

#include "EEPROM_Utils.h"
#include "basic_defines.h"
#include "persistentVars.h"

#define HTTP_SERVER_PING_ADDRESS                                       "1.1.1.1"
#define HTTP_SERVER_PING_INTERVAL_MS                                     (10000)

#define WIFI_CONNECTION_TIMEOUT_MS                                       (10000)
#define WIFI_SCAN_NOT_SEEN_MAX_COUNT                                         (5)
#define WIFI_SCAN_MINIMUM_RSSI_FOR_TRACKING                                (-80)
#define WIFI_SCAN_MAX_TRACKED_NETWORKS_COUNT                                (10)

#define TEMPERATURE_DEGREE_INVALID                                       (65535)

extern struct Settings settings;

struct WeatherData {
    double TemperatureDegree = TEMPERATURE_DEGREE_INVALID;
} MyWeather;

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
static bool wifiScanInProgress            = false;
static unsigned long lastConnectAttemptMs = 0;

bool ESP8266Utils_Connect_STA(
        const char* ssid,
        const char* password,
        const char* hostname,
        int32_t timeout_ms = 10000) {
    Serial.println("");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED && timeout_ms > 0) {
        delay(100);
        timeout_ms -= 100;
        yield();
    }

    if (timeout_ms <= 0) {
        Serial.println("STA Failed");
        return false;
    }

    Serial.println("");
    Serial.print("Iniciado STA:\t");
    Serial.println(ssid);
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    return true;
}

bool ESP8266Utils_is_STA_connected() { return WiFi.isConnected(); }

bool ESP8266Utils_is_STA_online() {
    if (!ESP8266Utils_is_STA_connected()) {
        return false;
    }
    static bool isOnline;
    static uint32_t timeout_ms = 0;
    if ((millis() - timeout_ms) > HTTP_SERVER_PING_INTERVAL_MS) {
        timeout_ms = millis();
        isOnline   = true;
        WiFiClient client;
        // Ping the server to see if it is online
        client.setTimeout(HTTP_SERVER_REQUEST_TIMEOUT);
        if (!client.connect(HTTP_SERVER_PING_ADDRESS, 80)) {
            isOnline = false;
        }
        client.stop();
    }
    return isOnline;
}

int8_t ESP8266Utils_get_STA_RSSI() { return WiFi.RSSI(); }

String ESP8266Utils_get_STA_SSID() { return WiFi.SSID(); }

bool ESP8266Utils_update_WeatherData(struct Settings* myData) {
    if (!ESP8266Utils_is_STA_online()) {
        Serial.println("No internet connection");
        return false;
    }

    String default_appid = DEFAULT_OPENWEATHERMAP_APPID;
    if ((String(myData->openWeatherMapSettings.appid) == default_appid)
        || (String(myData->openWeatherMapSettings.appid) == String(""))) {
        Serial.println("Invalid Weather API key : " + String(myData->openWeatherMapSettings.appid));
        return false;
    }

    WiFiClient client;
    // Connect to HTTP server
    client.setTimeout(HTTP_SERVER_REQUEST_TIMEOUT);
    if (!client.connect(OPENWEATHERMAP_HOST, OPENWEATHERMAP_PORT)) {
        Serial.println("Connection failed");
        // Disconnect
        client.stop();
        return false;
    }

    // Send HTTP request
    String HTTPrequest = DEFAULT_OPENWEATHERMAP_HTTP_REQUEST(
            myData->openWeatherMapSettings.appid,
            myData->openWeatherMapSettings.lat,
            myData->openWeatherMapSettings.lon);
    client.println(HTTPrequest);
    client.println("Host: " + String(OPENWEATHERMAP_HOST));
    client.println("Connection: close");
    if (client.println() == 0) {
        Serial.println("Failed to send request");
        // Disconnect
        client.stop();
        return false;
    }
    // Skip HTTP headers
    if (!client.find("\r\n\r\n")) {
        Serial.println("Invalid response");
        // Disconnect
        client.stop();
        return false;
    }
    if (client.find("\"temp\":")) {
        double NewTemp = client.readStringUntil(',').toDouble();
        if (NewTemp > 273) {
            NewTemp -= 273.15;
        }
        Serial.print("Temperature: ");
        Serial.println(NewTemp);
        MyWeather.TemperatureDegree = NewTemp;
    } else {
        Serial.println("No Temperature JSON object found");
    }
    if (client.find("\"timezone\":")) {
        long timezoneshift = strtol(client.readStringUntil(',').c_str(), NULL, 10);
        Serial.print("Timezone shift: ");
        Serial.println(timezoneshift);
        struct rtcTime_t rtcTime = { .myTime = time(NULL), .timezoneShift = timezoneshift };
        persistentVars_store_rtcTime(&rtcTime);
    } else {
        Serial.println("No timezoneshift JSON object found");
    }

    // Disconnect
    client.stop();
    return true;
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
    lastConnectAttemptMs = millis();
}

bool ESP8266Utils_isWifiConnected() { return WiFi.status() == WL_CONNECTED; }

int ESP8266Utils_getWifiConnectionPercentage() {
    if (lastConnectAttemptMs == 0) {
        return 0;
    }
    unsigned long elapsed = millis() - lastConnectAttemptMs;
    return (elapsed * 100) / WIFI_CONNECTION_TIMEOUT_MS;
}

void Wifi_handler() {}
