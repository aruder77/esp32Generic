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
	char topicCopy[strlen(topic) + 1];
	strcpy(topicCopy, topic);
	NetworkControl::getInstance()->messageReceived(topicCopy, data);
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
	Log.notice("NetworkControl Prefs %d\n", prefs);
	ledController = LedController::getInstance();

	// initialize prefs
	prefs->registerConfigParam("clientId", "Client-ID", "esp32Generic", 100, this);
	prefs->registerConfigParam("wpaKey", "WPA-Key", "~CM3CDUx", 100, this);
	prefs->registerConfigParam("mqtt-server", "MQTT-Server", "mqtt-server", 100, this);

	mqttClient->setCallback(callback);

	prefs->get("mqtt-server", mqtt_server);
	Log.notice("loaded mqtt-server from eeprom: %s\n", mqtt_server);
	mqttClient->setServer(mqtt_server, 1883);

	prefs->get("clientId", clientId);
}

NetworkControl::~NetworkControl()
{
}

const char *NetworkControl::getName() {
	return "network";
}

void NetworkControl::afterSetup() {
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

void NetworkControl::subscribeTopic(const char *clientId) {
	char cmndTopic[100];
	sprintf(cmndTopic, "cmnd/%s/#", clientId);
	mqttClient->subscribe(cmndTopic);

	char configTopic[100];
	sprintf(configTopic, "config/%s/#", clientId);
	mqttClient->subscribe(configTopic);

	char statusTopic[100];
	sprintf(statusTopic, "tele/%s/status", clientId);
	send(statusTopic, "Hello, world!");	
}

void NetworkControl::unsubscribeTopic(const char *clientId) {
	char cmndTopic[100];
	sprintf(cmndTopic, "cmnd/%s/#", clientId);
	mqttClient->unsubscribe(cmndTopic);

	char configTopic[100];
	sprintf(configTopic, "config/%s/#", clientId);
	mqttClient->unsubscribe(configTopic);

	char statusTopic[100];
	sprintf(statusTopic, "tele/%s/status", clientId);
	send(statusTopic, "Bye bye!");	
}

void NetworkControl::reconnect()
{
	// Loop until we're reconnected
	if (!mqttClient->connected())
	{
		Log.notice("Attempting MQTT connection...\n");
		// Attempt to connect
		if (mqttClient->connect(clientId))	
		{
			Log.notice("connected\n");
			subscribeTopic(clientId);
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

void NetworkControl::everySecond() {
	reconnect();
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
	Log.notice("MQTT send topic %s: %s\n", topic, message);
	mqttClient->publish(topic, message);
}

void NetworkControl::sendTelemetry(const char *data) {
	char teleTopic[100] = {0};
	sprintf(teleTopic, "tele/%s/status", clientId);
	Log.notice("sending telemetry topic %s with data %s\n", teleTopic, data);
	send(teleTopic, data);
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

	char wpaKey[100];
	prefs->get("wpaKey", wpaKey);
	if (!wifiManager.startConfigPortal(clientId, wpaKey))
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

void NetworkControl::configUpdate(const char *id, const char *value) {
	Log.notice("NetworkControl: config update received: %s, %s\n", id, value);
	if (strcmp(id, "clientId") == 0) {
		unsubscribeTopic(clientId);
		strcpy(clientId, value);
		subscribeTopic(clientId);
	}
}

