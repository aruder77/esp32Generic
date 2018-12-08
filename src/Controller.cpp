#include "Controller.h"


void progress(DlState state, int percent)
{
  Log.notice("state = %d - percent = %d\n", state, percent);
}

void Controller::receivedCallback(char *topic, byte *payload, unsigned int length)
{
  Log.trace("received mqtt message [%s]\n", topic);

  char data[length + 1];
	for (unsigned int i = 0; i < length; i++) {
	    data[i] = (char)payload[i];
	}
	data[length] = 0;

  Log.verbose("payload: %s\n", data);

  if (strncmp(OTA_TOPIC, topic, strlen(OTA_TOPIC)) == 0)
  {
    memset(url, 0, 100);
    memset(md5_check, 0, 50);
    char *tmp = strstr(data, "url:");
    char *tmp1 = strstr(data, ",");
    if (!tmp || !tmp1) {
      Log.warning("unsupported payload format!\n");
      return;
    }
    memcpy(url, tmp + strlen("url:"), tmp1 - (tmp + strlen("url:")));

    char *tmp2 = strstr(data, "md5:");
    memcpy(md5_check, tmp2 + strlen("md5:"), length - (tmp2 + strlen("md5:") - (char *)&data[0]));

    Log.notice("started fota url: %s\n", url);
    Log.notice("started fota md5: %s\n", md5_check);
    state = Fota_e;
  }
}
void Controller::mqttconnect()
{
  /* Loop until reconnected */
  if (!client->connected())
  {
    Log.notice("MQTT connecting ...\n");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client->connect(clientId.c_str()))
    {
      Log.notice("...connected\n");
      /* subscribe topic */
      client->subscribe(OTA_TOPIC);
      client->publish(OTA_TOPIC, "hello");
    }
    else
    {
      Log.error("failed, status code = %d try again in 5 seconds\n", client->state());
    }
  }
}

void error(char *message)
{
  Log.error(message);
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
void Controller::configModeCallback(WiFiManager *myWiFiManager)
{
  Log.notice("Entered config mode %s\n", WiFi.softAPIP().toString());
  //if you used auto generated SSID, print it
  Log.notice("%s\n", myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}


Controller::Controller()
{
  // put your setup code here, to run once:
  Serial.begin(baud_rate);
  Log.begin(LOG_LEVEL_TRACE, &Serial);

  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);

  pinMode(interruptPin, INPUT_PULLUP);  

  networkControl = NetworkControl::getInstance();
  prefs = Prefs::getInstance();
  ledController = LedController::getInstance();
}

void Controller::loop()
{
  switch (state)
  {
  case Runnning_e:

      // is configuration portal requested?
    if ( digitalRead(interruptPin) == LOW ) {
      ledController->blink();

      //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
      wifiManager.setAPCallback(configModeCallback);

      // id/name, placeholder/prompt, default, length
      WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
      wifiManager.addParameter(&custom_mqtt_server);
      
      if (!wifiManager.startConfigPortal("OnDemandAP", "geheim")) {
        Log.warning("failed to connect and hit timeout\n");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.restart();
        delay(5000);
      }

      strcpy(mqtt_server, custom_mqtt_server.getValue());
      client->setServer(mqtt_server, 1883);      

      prefs->set("mqtt-server", mqtt_server);
      Log.notice("saved mqtt-server to eeprom: %s\n", mqtt_server);

      //if you get here you have connected to the WiFi
      Log.notice("connected...yeey :)\n");
      ledController->on();     
    }

    /* if client was disconnected then try to reconnect again */
    if (!client->connected())
    {
      mqttconnect();
    }
    /* this function will listen for incomming 
      subscribed topic-process-invoke receivedCallback */
    client->loop();
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

    client->publish(OTA_TOPIC, "ok");
    break;
  default:
    break;
  }
}
