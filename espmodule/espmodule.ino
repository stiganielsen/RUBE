// ## RESOURCES ##
// http://esp8266.github.io/Arduino/versions/2.0.0/doc/libraries.html
// https://github.com/esp8266/esp8266-wiki/wiki/Boot-Process#esp-boot-modes

#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>

#define MaxLengthSSID 16
#define MaxLengthPassword 20
#define OffsetSSID 1
#define OffsetPassword (OffsetSSID + MaxLengthSSID)

const bool printDebugToSerial = false;
const char* defaultSsid = "hangient";
const char* defaultPassword = "goldberg";

struct wifiConfig {
    char* ssid;
    char* password;
    bool isServer;
};

WebSocketsServer webSocket = WebSocketsServer(81);
struct wifiConfig* WifiConfig;
String RobotResponse = "";

void readWifiConfigFromEEPROM()
{
    WifiConfig->isServer = EEPROM.read(0);

    for (int i = 0; i < MaxLengthSSID; i++)
        WifiConfig->ssid[i] = EEPROM.read(i + OffsetSSID);

    for (int i = 0; i < MaxLengthPassword; i++)
        WifiConfig->password[i] = EEPROM.read(i + OffsetPassword);

    if (printDebugToSerial)
        Serial.printf("IsServer: %d\nSSID: %s\nPassword: %s\n", WifiConfig->isServer, WifiConfig->ssid, WifiConfig->password);
}

void saveWifiConfigToEEPROM()
{
    EEPROM.write(0, WifiConfig->isServer);
    for (int i = 0; i < MaxLengthSSID; i++)
    {
        auto c = WifiConfig->ssid[i];
        EEPROM.write(i + OffsetSSID, c);

        if (c == '\0')
            break;
    }

    for (int i = 0; i < MaxLengthPassword; i++)
    {
        auto c = WifiConfig->password[i];
        EEPROM.write(i + OffsetPassword, c);

        if (c == '\0')
            break;
    }

    EEPROM.commit();
}

void setupStationMode(char* ssid, char* password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    auto startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - 5000) < startTime)
        delay(100);

    if (printDebugToSerial)
    {
        delay(100);
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("Wifi Client Running\nLocal IP: ");
            Serial.println(WiFi.localIP());
        }
        else
            Serial.printf("Wifi station connect timed out with status: %d\n", (int)WiFi.status());
    }
}

void setupAccessPointMode(const char* ssid, const char* password = (const char*)0)
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    if (printDebugToSerial)
    {
        IPAddress myIP = WiFi.softAPIP();
        Serial.print("Wifi AP Running\nLocal IP: ");
        Serial.println(myIP);
        WiFi.printDiag(Serial);
    }
}

void performWifiCommand(char* cmd)
{
    switch (cmd[0])
    {
        case ('B'):
        {
            webSocket.broadcastTXT(&cmd[1]);
            return;
        }

        case ('W'):
        {
            // Sets new Wifi configuration and reset
            // Layout: "W:1/0:ssid:password"
            std::string conf = std::string(&cmd[2]);
            auto isServer = conf.substr(0, 1);
            auto sepPos = conf.find_last_of(':');
            auto ssid = conf.substr(2, sepPos - 2);
            auto password = conf.substr(sepPos);

            if (password.length() > 1)
                password = password.substr(1);
            else
                password = "";

            WifiConfig->isServer = isServer == "1";
            strcpy(WifiConfig->ssid, ssid.c_str());
            strcpy(WifiConfig->password, password.c_str());

            if (printDebugToSerial)
                Serial.printf("IsServer: %d\nSSID: %s\nPassword: %s\n", WifiConfig->isServer, WifiConfig->ssid, WifiConfig->password);

            saveWifiConfigToEEPROM();
            ESP.restart();

            return;
        }

        case ('R'):
        {
            // Return the current wifi config
            String msg = "ACONFIG:";
            msg.concat(WifiConfig->isServer ? "1:" : "0:");
            msg.concat(WifiConfig->ssid);
            msg.concat(":");
            msg.concat(WifiConfig->password);

            webSocket.broadcastTXT(msg.c_str());

            if (printDebugToSerial)
                Serial.println(msg);

            return;
        }
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t lenght)
{
    switch (type)
    {
        case WStype_ERROR:
        {
            if (printDebugToSerial)
                Serial.println("Error");

            break;
        }

        case WStype_DISCONNECTED:
        {
            if (printDebugToSerial)
                Serial.println("Disconnected");

            break;
        }

        case WStype_CONNECTED:
        {
            if (printDebugToSerial)
            {
                Serial.print("Connected: ");
                Serial.println(webSocket.remoteIP(num));
            }

            break;
        }

        case WStype_TEXT:
        {
            char* command = (char*)&payload[1];
            switch ((char)payload[0])
            {
                case 'A':
                {
                    // Command is for Wifi module
                    if (printDebugToSerial)
                        Serial.printf("[A] %s\n", command);

                    performWifiCommand(command);
                    break;
                }

                case 'B':
                {
                    // Command is for attached bot
                    Serial.printf("%s\n", command);
                    break;
                }

                default:
                {
                    webSocket.sendTXT(num, strcat("Unknown command: ", command));
                    break;
                }
            }

            break;
        }

        case WStype_BIN:
        {
            hexdump(payload, lenght);
            webSocket.sendBIN(num, payload, lenght);
            break;
        }
    }
}

void resetToDefault()
{
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 200)
    {
        if (printDebugToSerial)
            Serial.printf("Resetting to default...\n");

        WifiConfig->isServer = true;
        WifiConfig->ssid = (char*)defaultSsid;
        WifiConfig->password = (char*)defaultPassword;
        saveWifiConfigToEEPROM();
        ESP.restart();
    }
    last_interrupt_time = interrupt_time;
}

void setup()
{
    Serial.begin(115200);
    EEPROM.begin(1024);

    if (printDebugToSerial)
        Serial.printf("\nESP8266 booting up...\n");

    WifiConfig = (struct wifiConfig*) malloc(sizeof(wifiConfig));
    WifiConfig->ssid = (char*)malloc(MaxLengthSSID);
    WifiConfig->password = (char*)malloc(MaxLengthPassword);

    readWifiConfigFromEEPROM();
    if (strlen(WifiConfig->ssid) == 0)
    {
        Serial.println("Reset due to length");
        WifiConfig->isServer = true;
        WifiConfig->ssid = (char*)defaultSsid;
        WifiConfig->password = (char*)defaultPassword;
        saveWifiConfigToEEPROM();
    }

    pinMode(2, INPUT_PULLUP);
    attachInterrupt(2, resetToDefault, FALLING);

    // SoftAP seems to only support b/g on the ESP8266 though
    WiFi.setPhyMode(WIFI_PHY_MODE_11N);

    if (WifiConfig->isServer)
        setupAccessPointMode(WifiConfig->ssid, WifiConfig->password);
    else
        setupStationMode(WifiConfig->ssid, WifiConfig->password);

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop()
{
    // Check for received commands from Wifi
    webSocket.loop();

    // Check for received commands from Bot
    while (Serial.available())
    {
        int character = Serial.read();
        if (character == '\n')
        {
            performWifiCommand(&RobotResponse[0]);
            RobotResponse = "";
            return;
        }
        else
            RobotResponse.concat((char)character);
    }
}
