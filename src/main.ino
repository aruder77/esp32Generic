#include <Arduino.h>

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "HttpFOTA.h"
#include <PubSubClient.h>

typedef enum
{
  Runnning_e = 0x01,
  Fota_e
} SysState;

const char *mqtt_server = "192.168.1.107";

char url[100];
char md5_check[50];
WiFiClient espClient;
PubSubClient client(espClient);
SysState state = Runnning_e;

/* topics */
#define OTA_TOPIC "smarthome/room1/ota"

void progress(DlState state, int percent)
{
  Serial.printf("state = %d - percent = %d\n", state, percent);
}

void receivedCallback(char *topic, byte *payload, unsigned int length)
{

  if (strncmp(OTA_TOPIC, topic, strlen(OTA_TOPIC)) == 0)
  {
    memset(url, 0, 100);
    memset(md5_check, 0, 50);
    char *tmp = strstr((char *)payload, "url:");
    char *tmp1 = strstr((char *)payload, ",");
    memcpy(url, tmp + strlen("url:"), tmp1 - (tmp + strlen("url:")));

    char *tmp2 = strstr((char *)payload, "md5:");
    memcpy(md5_check, tmp2 + strlen("md5:"), length - (tmp2 + strlen("md5:") - (char *)&payload[0]));

    Serial.printf("started fota url: %s\n", url);
    Serial.printf("started fota md5: %s\n", md5_check);
    state = Fota_e;
  }
}
void mqttconnect()
{
  /* Loop until reconnected */
  while (!client.connected())
  {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      /* subscribe topic */
      client.subscribe(OTA_TOPIC);
    }
    else
    {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void error(char *message)
{
  printf("%s\n", message);
}

void startDl(void)
{
}
void endDl(void)
{
}

//for LED status
#include <Ticker.h>
Ticker ticker;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED); // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);    // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, HIGH);

  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);
}

void loop()
{
  switch (state)
  {
  case Runnning_e:
    /* if client was disconnected then try to reconnect again */
    if (!client.connected())
    {
      mqttconnect();
    }
    /* this function will listen for incomming 
      subscribed topic-process-invoke receivedCallback */
    client.loop();
    break;
  case Fota_e:
    DlInfo info;
    info.url = url;
    info.md5 = md5_check;
    info.startDownloadCallback = startDl;
    info.endDownloadCallback = endDl;
    info.progressCallback = progress;
    info.errorCallback = error;
    httpFOTA.start(info);

    client.publish(OTA_TOPIC, "ok");
    break;
  default:
    break;
  }
}
