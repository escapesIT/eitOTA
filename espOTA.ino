#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>


String software = "act8266Christmas";
const String FirmwareVer = {"0.0"};

String clientID;

//Personal website version
String path = "http://rycgar.com/eitOTA/" + software;

String URL_fw_Version = path + ".version.txt";
//#define URL_fw_Version "http://rycgar.com/eitOTA/httpsUpdate.version.txt"

String URL_fw_Bin = path  + ".ino.nodemcu.bin";
//#define URL_fw_Bin "http://rycgar.com/eitOTA/httpsUpdate.ino.nodemcu.bin"


//Github version (Struggling with this)
/*
String path = "https://github.com/escapesIT/eitOTA/blob/main/" + software;
String URL_fw_Version = path + ".version.txt";
String URL_fw_Bin = path + ".ino.nodemcu.bin";
//#define URL_fw_version "https://github.com/escapesIT/eitOTA/blob/main/espOTA.version.txt"
//#define URL_fw_Bin "https://github.com/escapesIT/eitOTA/blob/main/espOTA.ino.nodemcu.bin"
*/

HTTPClient http;

const char* ssid = "Escapes-2.4";
const char* password = "8185178608";

void FirmwareUpdate()
{

  http.begin(URL_fw_Version);     // check version URL
  delay(100);
  int httpCode = http.GET();            // get data from version file
  delay(1000);
  String payload;
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    payload = http.getString();  // save received version
    Serial.println("Found Firmware Version: " + payload );
  }
  else
  {
    Serial.print("error in downloading version file:");
    Serial.println(httpCode);

  }

  http.end();
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    if (payload.equals(FirmwareVer) )
    {
      Serial.println("Device already on latest firmware version");
    }
    else
    {
      Serial.println("New firmware detected");
      WiFiClient client;

      // The line below is optional. It can be used to blink the LED on the board during flashing
      // The LED will be on during download of one buffer of data from the network. The LED will
      // be off during writing that buffer to flash
      // On a good connection the LED should flash regularly. On a bad connection the LED will be
      // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
      // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
      ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

      Serial.println("Installing new software\n\n");

      t_httpUpdate_return ret = ESPhttpUpdate.update(URL_fw_Bin);

      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;

        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          break;
      }
    }
  }
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 10000;

void repeatedCall() {
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    FirmwareUpdate();
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Version number: ");
  Serial.println(FirmwareVer);
  Serial.println("Will request software: " + software);
  //Set clientID from mac address
  clientID = + "eOS-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientID += macToStr(mac);
  Serial.println("ClientID is: " + clientID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("O");
  }
  Serial.println("\nConnected to WiFi");
  pinMode(LED_BUILTIN, OUTPUT);



}
void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  repeatedCall();
}
