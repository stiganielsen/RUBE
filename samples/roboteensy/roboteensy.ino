String WifiRequest = "";

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(115200);
  
  // Give time to ESP to start and clear incomming buffer
  delay(3000);
  while (Serial1.available())
    Serial1.read();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(20);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(20);
  digitalWrite(LED_BUILTIN, LOW);
}

void wifiResponse(char* response)
{
  Serial1.printf("B%s\n", response);
}

void loop()
{
  while (Serial1.available())
  {
    char character = Serial1.read();
    if (character != '\n')
      WifiRequest.concat(character);
    else
    {
      if (WifiRequest == "ON")
      {
        wifiResponse("Stuff turned on");
        digitalWrite(LED_BUILTIN, HIGH);
      }
      else if (WifiRequest == "OFF")
      {
        wifiResponse("Stuff turned off");
        digitalWrite(LED_BUILTIN, LOW);
      }
      else
      {
        wifiResponse("Unknown command");
      }

      WifiRequest = "";
    }
  }
}

