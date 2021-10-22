#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "TUNG";
const char *ssid_password = "123456789";

AsyncWebServer server(80);

void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, ssid_password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi failed\n");
    return;
  }
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // route
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(200, "text/plain", "Hello World");
              Serial.println("New client connected!");
            });

  //start server
  server.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
}