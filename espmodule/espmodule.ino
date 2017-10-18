#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <WebSocketsServer.h>
#include <Hash.h>

WebSocketsServer webSocket = WebSocketsServer(81);
bool printDebugToSerial = false;
bool isServer = false;
const char* ssid = "...";
const char* password = "...";
String RobotResponse = "";

/* 
 * Executes a command minded for the wifi module itself
 */
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
      // Sets new Wifi configuration and rests the module

      // type (0: Client, 1: AP)
      // ssid
      // password
      return;
    }
  }
}

/* 
 * Takes a command minded for the robot and passes it on to it.
 * It then waits for a response from the robot before returning
 */
void performRobotCommand(uint8_t num, char* cmd)
{
  Serial.printf("%s\n", cmd);

  /* This section waits for a response from the bot
   * Disabled atm. Bot can choose to broadcast stuff back when needed
   *
  auto lastRead = millis();
  while (millis() - lastRead < 1000)
  {
    while (Serial.available())
    {
      int character = Serial.read();
      RobotResponse.concat((char) character);
      if (character == '\n')
      {
        if (RobotResponse[0] == 'R')
        {
          webSocket.sendTXT(num, &RobotResponse[1]);
          RobotResponse = "";
          return;
        }
        else
        {
          performWifiCommand(&RobotResponse[1]);
          RobotResponse = "";
        }
      }
    }
  }

  webSocket.sendTXT(num, "TIMEOUT");
  */
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t lenght)
{
  switch (type)
  {
    case WStype_ERROR:
    {
      if (printDebugToSerial)
        Serial.print("Error");

      break;
    }
    
    case WStype_DISCONNECTED:
    {
      if (printDebugToSerial)
        Serial.print("Disconnected");

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
      char* command = (char*) &payload[1];
      switch ((char) payload[0])
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
          // Command is for bot
          if (printDebugToSerial)
            Serial.printf("[B] %s\n", command);
          
          performRobotCommand(num, command);
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
      webSocket.sendBIN(num, payload, lenght); // echo data back to browser
      break;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(10);

  if (printDebugToSerial)
    Serial.printf("\n\nIsServer: %d, SSID: %s, Pass: %s\n", isServer, ssid, password);    
    
  if (isServer) {

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, password);
    IPAddress myIP = WiFi.softAPIP();

    if (printDebugToSerial)
    {
      Serial.print("Wifi AP Running\nLocal IP: ");
      Serial.println(myIP);      
    }    
  } 
  else
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
      delay(100);

    if (printDebugToSerial)
    {
      Serial.print("Wifi Client Running\nLocal IP: ");
      Serial.println(WiFi.localIP());
    }
  }
    
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
    RobotResponse.concat((char) character);
    if (character == '\n')
    {
      performWifiCommand(&RobotResponse[0]);
      RobotResponse = "";
      return;
    }
  }
}
