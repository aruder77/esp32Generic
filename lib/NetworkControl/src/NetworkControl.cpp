/*
 * NetworkControl.cpp
 *
 *  Created on: 22 Jun 2017
 *      Author: axel
 */

#include <NetworkControl.h>
#include <Prefs.h>

NetworkControl *NetworkControl::instance = 0;

void callback(char *topic, byte *payload, unsigned int length)
{
	char data[length + 1];
	for (unsigned int i = 0; i < length; i++)
	{
		data[i] = (char)payload[i];
	}
	data[length] = 0;
	NetworkControl::getInstance()->messageReceived(topic, data);
}

NetworkControl *NetworkControl::getInstance()
{
	// The only instance
	// Guaranteed to be lazy initialized
	// Guaranteed that it will be destroyed correctly
	if (!NetworkControl::instance)
	{
		NetworkControl::instance = new NetworkControl();
	}
	return NetworkControl::instance;
}

NetworkControl::NetworkControl() {
	Log.notice("Initializing NetworkControl\n");

	WiFiClient *espClient = new WiFiClient();
	mqttClient = new PubSubClient(*espClient);

	prefs = Prefs::getInstance();
	ledController = LedController::getInstance();

	// initialize prefs
	prefs->registerConfigParam("clientId", "Client-ID", "esp32Generic", 100, this);
	prefs->registerConfigParam("mqtt-server", "MQTT-Server", "mqtt-server", 100, this);

	mqttClient->setCallback(callback);

	prefs->get("mqtt-server", mqtt_server);
	Log.notice("loaded mqtt-server from eeprom: %s\n", mqtt_server);
	mqttClient->setServer(mqtt_server, 1883);

	ledController->blinkFast();
	if (wifiManager.autoConnect())
	{
		//keep LED on
		ledController->on();
	}

	// Allow the hardware to sort itself out
	delay(1500);
	Log.notice("finished initializing NetworkControl\n");
	reconnect();
}

NetworkControl::~NetworkControl()
{
}

void NetworkControl::reconnect()
{
	// Loop until we're reconnected
	if (!mqttClient->connected())
	{
		Log.notice("Attempting MQTT connection...\n");
		// Attempt to connect
		char clientId[50] = {0};
		prefs->get("clientId", clientId);
		if (mqttClient->connect(clientId))	
		{
			Log.notice("connected\n");
			mqttClient->subscribe("cmnd/esp32generic/#");
			mqttClient->subscribe("config/esp32generic/#");
			send("tele/esp32generic/status", "Hello, world!");
		}
		else
		{
			Log.error("failed, rc=%d try again in 5 minutes\n", mqttClient->state());
		}
	}
}

void NetworkControl::loop()
{
	mqttClient->loop();
}

char *substr(const char *buff, uint8_t start, uint8_t len, char *substr)
{
	strncpy(substr, buff + start, len);
	substr[len] = 0;
	return substr;
}

void NetworkControl::messageReceived(const char *topic, const char *message)
{
	messageDispatcher->messageReceived(topic, message);
}

void NetworkControl::subscribeToCommand(const char *command, NetworkModule *networkModule)
{
	messageDispatcher->subscribeToCommand(command, networkModule);
}

void NetworkControl::send(const char *topic, const char *message)
{
	mqttClient->publish(topic, message);
}

bool NetworkControl::exists()
{
	return instance != 0;
}

bool NetworkControl::isConnected()
{
	return mqttClient->connected();
}

//gets called when WiFiManager enters configuration mode
void NetworkControl::configModeCallback(WiFiManager *myWiFiManager)
{
	Log.notice("Entered config mode %s\n", WiFi.softAPIP().toString());
	//if you used auto generated SSID, print it
	Log.notice("%s\n", myWiFiManager->getConfigPortalSSID());
	//entered config mode, make led blink
	LedController::getInstance()->blink();
}

//gets called when WiFiManager saves config
void NetworkControl::saveConfigCallback() {
}

void NetworkControl::enterConfigPortal()
{
	Log.notice("entering config portal mode...\n");
	ledController->blink();

	//set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	wifiManager.setAPCallback(configModeCallback);
	wifiManager.setSaveConfigCallback(saveConfigCallback);

	PrefsItems *prefsItems = prefs->getPrefsItems();

	Log.notice("number of config items: %d\n", prefsItems->length);
	wifiParamCount = prefsItems->length;
	params = new WiFiManagerParameter*[wifiParamCount];

	for (int i = 0; i < wifiParamCount; i++)
	{
		Log.notice("adding config item %s\n", prefsItems->prefsItems[i]->id);
		params[i] = new WiFiManagerParameter(prefsItems->prefsItems[i]->id, prefsItems->prefsItems[i]->prompt, prefsItems->prefsItems[i]->defaultValue, prefsItems->prefsItems[i]->length);
		wifiManager.addParameter(params[i]);
	}

	char clientId[50];
	prefs->get("clientId", clientId);
	if (!wifiManager.startConfigPortal("axelsEsp", "geheim"))
	{
		Log.warning("failed to connect and hit timeout\n");
		delay(3000);
		//reset and try again, or maybe put it to deep sleep
		ESP.restart();
		delay(5000);
	}

	for (int i = 0; i < wifiParamCount; i++) {
		Log.notice("saving wifiParam[%d] to prefs: %s, %s\n", i, params[i]->getID(), params[i]->getValue());
		prefs->set(params[i]->getID(), params[i]->getValue());
	}

	char buffer[100];
	prefs->get("mqtt-server", buffer);
	mqttClient->setServer(buffer, 1883);

	Log.notice("saved mqtt-server to eeprom: %s\n", buffer);

	ledController->on();     
}
