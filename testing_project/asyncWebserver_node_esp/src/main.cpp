#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "TUNG";
const char *password = "123456789";

IPAddress local_IP(192, 168, 137, 2);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

AsyncWebServer server(80);

void setup()
{

  Serial.begin(9600);
  Serial.println();
  // Connect to Wi-Fi
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA fialed to config");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // route
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(SPIFFS, "/index.html", String());
              Serial.println("New client connected!");
            });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(SPIFFS, "/style.css", "text/css");
              Serial.println("CSS sent!");
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
  // while (dir.next())
  // {
  //   str += dir.fileName();
  //   str += " / ";
  //   str += dir.fileSize();
  //   str += "\r\n";
  // }
  // Serial.print("Remains files: ");
  // Serial.print(str);

  // if (!SPIFFS.remove("/style.css"))
  // {
  //   Serial.println("Failed to delete file");
  // }
  // else
  // {
  //   Serial.println("Deleted style.css");
  // }

  // if (!SPIFFS.remove("/index.html"))
  // {
  //   Serial.println("Failed to delete file");
  // }
  // else
  // {
  //   Serial.println("Deleted index.html");
  // }

  while (dir.next())
  {
    str += dir.fileName();
    str += " / ";
    str += dir.fileSize();
    str += "\r\n";
  }
  Serial.print("Remains files: ");
  Serial.print(str);
}

void loop() {}
