#include <Arduino.h>

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "HttpFOTA.h"
#include <PubSubClient.h>
#include <Ticker.h>

/* topics */
#define OTA_TOPIC "smarthome/room1/ota"

const char *mqtt_server = "192.168.178.27";
const int baud_rate = 115200;
const uint8_t interruptPin = A4;
char *root_ca = \
  "-----BEGIN CERTIFICATE-----\n"\
  "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n"\
  "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"\
  "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n"\
  "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n"\
  "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"\
  "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n"\
  "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n"\
  "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n"\
  "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n"\
  "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n"\
  "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n"\
  "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n"\
  "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n"\
  "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n"\
  "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n"\
  "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n"\
  "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n"\
  "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n"\
  "-----END CERTIFICATE-----";

typedef enum
{
  Runnning_e = 0x01,
  Fota_e
} SysState;

Ticker ticker;
WiFiManager wifiManager;

WiFiClient espClient;
PubSubClient client(espClient);


char url[100];
char md5_check[50];
SysState state = Runnning_e;
bool resetting = false;


void progress(DlState state, int percent)
{
  Serial.printf("state = %d - percent = %d\n", state, percent);
}

void receivedCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.printf("received mqtt message [%s]\n", topic);
  Serial.printf("length: %d\n", length);

  char data[length + 1];
	for (unsigned int i = 0; i < length; i++) {
	    data[i] = (char)payload[i];
	}
	data[length] = 0;

  Serial.printf("payload: %s\n", data);

  if (strncmp(OTA_TOPIC, topic, strlen(OTA_TOPIC)) == 0)
  {
    memset(url, 0, 100);
    memset(md5_check, 0, 50);
    char *tmp = strstr(data, "url:");
    char *tmp1 = strstr(data, ",");
    if (!tmp || !tmp1) {
      Serial.printf("unsupported payload format!\n");
      return;
    }
    memcpy(url, tmp + strlen("url:"), tmp1 - (tmp + strlen("url:")));

    char *tmp2 = strstr(data, "md5:");
    memcpy(md5_check, tmp2 + strlen("md5:"), length - (tmp2 + strlen("md5:") - (char *)&data[0]));

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
      client.publish(OTA_TOPIC, "hello");
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

void reset()
{
  if (!resetting) { 
    resetting = true;
    Serial.println("resetting controller!");
    wifiManager.resetSettings();
  }
}

void setup()
{
  resetting = false;

  // put your setup code here, to run once:
  Serial.begin(baud_rate);

  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  // id/name, placeholder/prompt, default, length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
  wifiManager.addParameter(&custom_mqtt_server);

  // setup reset pin
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), reset, ONLOW);

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
    info.caCert = root_ca;
    httpFOTA.start(info);

    client.publish(OTA_TOPIC, "ok");
    break;
  default:
    break;
  }
}
