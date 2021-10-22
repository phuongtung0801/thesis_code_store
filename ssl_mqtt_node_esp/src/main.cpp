

#include <Arduino.h>
#include <painlessMesh.h>     //thu vien mang mesh
#include <PubSubClient.h>     //thu vien mqtt
#include <WiFiClientSecure.h> //thu vien connect wifi
#include <WiFiClient.h>
#define MESH_PREFIX "tung"
#define MESH_PASSWORD "phuongtung0801"
#define MESH_PORT 5555

#define STATION_SSID "TUNG"
#define STATION_PASSWORD "123456789"
//WiFiEventHandler wifiConnectHandler;
//WiFiEventHandler wifiDisconnectHandler;

#define HOSTNAME "MQTT_Bridge"

unsigned long t1 = 0;
const unsigned long eventInterval = 5000;
unsigned long previousTime = 0;

const char *mqtt_server = "phuongtung08081.tk";
const char *mqtt_username = "test_user";
const char *mqtt_password = "test_password";
const char *clientID = "TUNG";
const char *endTopic = "phuongtung0801/LWT";
const int port = 8883;
const char *ca_pem = \  
"-----BEGIN CERTIFICATE-----\n"
                     "MIIDvTCCAqWgAwIBAgIUfquovTolUXqgafOfX1yfZG2Bs7YwDQYJKoZIhvcNAQEL\n"
                     "BQAwbjELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM\n"
                     "GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEnMCUGCSqGSIb3DQEJARYYcGh1b25n\n"
                     "dHVuZzA4MDFAZ21haWwuY29tMB4XDTIxMTAwMjEyMjgxMVoXDTMxMDkzMDEyMjgx\n"
                     "MVowbjELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM\n"
                     "GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEnMCUGCSqGSIb3DQEJARYYcGh1b25n\n"
                     "dHVuZzA4MDFAZ21haWwuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n"
                     "AQEAlGvKQP7AVR0X7XqDcFkWj+HFa0cwqeqMHcKLwCRKs5q8mWQK4/cZPft0/h94\n"
                     "16Nv6wL8EsXOFA2xTTKo4B/CuoRUZp6ZIXjrh4fWXvZreRB5NKpMidD2SCT+vsm5\n"
                     "2/zZmcjDWbuoSuHJgJdfd93baGvQ4qBpUoLe9r0ithHdqwlQ1KMGyeUnq8qFJ5Mv\n"
                     "PZDIh5vQ0Pjny9Hq6XnC1nRD+Dd+6QqcYbpvjNGwteikI26GnPgfzcsaJaMHv6K6\n"
                     "XpRuD0pqS31qxnSyLunFs2JTde7Njm97t33OFKgdzkHp0uTIpEtkYfyNlsTPYM1S\n"
                     "E9dZkEjM+OX6O7YdilOXawoBkwIDAQABo1MwUTAdBgNVHQ4EFgQULiZzbQ3/Lbpn\n"
                     "QeYzeY6aPrGBeP8wHwYDVR0jBBgwFoAULiZzbQ3/LbpnQeYzeY6aPrGBeP8wDwYD\n"
                     "VR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEATetVwl3T22yRbSEZZxjK\n"
                     "dgOfpKCI8BL/JcgFhGY0Kt1nh+/1zmmcM++Tec2E/zQWO0HbGJcUuzErWtX9J6r6\n"
                     "rLGf22AwRj9BSeYlaMhJV70Y5faezBI9RnyvHG9vU3CCbWiECXr+aJ0+YQB8FSLd\n"
                     "0+XXsGGUf+PF+SkVurhGZHtJk/uCXburz4+iVZGnP7i1QTS9j/jAICZ+//Bpe4nw\n"
                     "5tzKa9z8rwWaNg9Prp0kGD0b1z1qoOKYjdV2wCgQBGoAf6sWXvZDFd76Ugk5N5lK\n"
                     "Ikf7+L1wztzLHPkwIMHmpPtBNkWJsgPJGIFjQ1qhPqzCc8Zaubzdrn0nX6qw3/j2\n"
                     "gA==\n"
                     "-----END CERTIFICATE-----\n";
const char *fingerprint = "AA:75:54:46:08:29:36:05:B8:D1:F5:87:21:AD:E0:D1:42:3A:BE:69";
// Prototypes
void receivedCallback(const uint32_t &from, const String &msg);
void mqttCallback(char *topic, byte *payload, unsigned int length);

IPAddress getlocalIP();

IPAddress myIP(0, 0, 0, 0);
//IPAddress mqttBroker("m2m.eclipse.org");

painlessMesh mesh;
//WiFiClient wifiClient;
WiFiClientSecure wifiClient;
//wifiClient.setInsecure();

//tạo một instance để kết nối. Sử dụng cú pháp PubSubClient (server, port, [callback], client, [stream])
//PubSubClient mqttClient(wifiClient);
PubSubClient mqttClient(mqtt_server, port, mqttCallback, wifiClient);

void setup()
{
  Serial.begin(9600);

  WiFi.begin(STATION_SSID, STATION_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  //Serial.println("Connected to the WiFi network");

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6);
  mesh.onReceive(&receivedCallback);

  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  //set SSL/TLS certificate
  wifiClient.setFingerprint(fingerprint);
  mqttClient.setServer(mqtt_server, port);
  mqttClient.setCallback(mqttCallback);
  //mqttClient.connect(clientID, "", "", "theEndTopic", 1, true, "offline");
  if (mqttClient.connect(
          clientID, mqtt_username, mqtt_password, endTopic, 1, true, "Sensor disconnected from mqtt"))
  {
    Serial.println("Connected to MQTT Broker!");
    mqttClient.publish(endTopic, "Sensor connected!", true);
  }
  else
  {
    Serial.println("Connection to MQTT Broker failed...");
  }

  mqttClient.subscribe("tungtran");
  mqttClient.subscribe(endTopic);
}

void loop()
{
  mesh.update();
  mqttClient.loop();

  /* Updates frequently */
  unsigned long currentTime = millis();

  //wifi checking
  if (currentTime - previousTime >= eventInterval)
  {
    switch (WiFi.status())
    {
    case WL_CONNECTED:
      //Serial.println("Wifi Connection successfully established");
      if (!mqttClient.connected())
      {
        Serial.println("Reconnecting to MQTT...");
        if (mqttClient.connect(
                clientID, mqtt_username, mqtt_password, endTopic, 1, true, "Sensor disconnected from mqtt"))
        {
          Serial.println("Reconnect to MQTT success!");
          mqttClient.setServer(mqtt_server, port);
          mqttClient.setCallback(mqttCallback);
          mqttClient.subscribe("tungtran");
          mqttClient.publish(endTopic, "Sensor connected!", true);
        }
        else
        {
          Serial.println("MQTT connection is lost!");
        }
      }
      break;

    default:
      Serial.println("Failed to connect Wifi");
      break;
    }
    //Update timing for next round
    previousTime = currentTime;
  }

  if (myIP != getlocalIP())
  {
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
}

void receivedCallback(const uint32_t &from, const String &msg)
{
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  //String topic = "painlessMesh/from/2131961461";
  String topic = "painlessMesh/from/" + String(from);
  mqttClient.publish(topic.c_str(), msg.c_str());
  Serial.println("Data received");
}

void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{

  String cmd = "";

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    cmd += (char)payload[i];
  }
  Serial.println(cmd);
  if (cmd == "on")
  {
    mesh.sendSingle(2131961461, cmd);
    mesh.sendSingle(110231767, cmd);
  }
  if (cmd == "off")
  {
    mesh.sendSingle(2131961461, cmd);
    mesh.sendSingle(110231767, cmd);
  }
}
//end mqttCallback

IPAddress getlocalIP()
{
  return IPAddress(mesh.getStationIP());
}
