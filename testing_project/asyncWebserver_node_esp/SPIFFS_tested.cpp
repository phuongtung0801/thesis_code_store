#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "TUNG";
const char *password = "123456789";

AsyncWebServer server(80);

void setup()
{

  Serial.begin(9600);
  Serial.println();
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // route
  server.on("/tung", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(SPIFFS, "/index.html", String());
              Serial.println("New client connected!");
            });

  //start server
  server.begin();
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  String str = "";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    str += dir.fileName();
    str += " / ";
    str += dir.fileSize();
    str += "\r\n";
  }
  Serial.print("Remains files: ");
  Serial.print(str);

  // if (!SPIFFS.remove("/app.html"))
  // {
  //   Serial.println("Failed to delete file");
  // }

  // while (dir.next())
  // {
  //   str += dir.fileName();
  //   str += " / ";
  //   str += dir.fileSize();
  //   str += "\r\n";
  // }
  // Serial.print("Remains files: ");
  // Serial.print(str);
}

void loop() {}
