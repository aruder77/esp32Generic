/*
 * NetworkControl.cpp
 *
 *  Created on: 22 Jun 2017
 *      Author: axel
 */

#include <NetworkControl.h>
#include <Prefs.h>



NetworkControl *NetworkControl::instance = 0;

void callback(char* topic, byte* payload, unsigned int length) {
	char data[length + 1];
	for (unsigned int i = 0; i < length; i++) {
	    data[i] = (char)payload[i];
	}
	data[length] = 0;
	NetworkControl::getInstance()->messageReceived(topic, data);
}

NetworkControl* NetworkControl::getInstance()
{
    // The only instance
    // Guaranteed to be lazy initialized
    // Guaranteed that it will be destroyed correctly
	if (!NetworkControl::instance) {
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

	mqttClient->setCallback(callback);

	Prefs::getInstance()->get("mqtt-server", mqtt_server);
	Log.notice("loaded mqtt-server from eeprom: %s\n", mqtt_server);
	mqttClient->setServer(mqtt_server, 1883);

  	if (wifiManager.autoConnect()) {
		//keep LED on
		ledController->on();        
  	}

	// Allow the hardware to sort itself out
	delay(1500);
	Log.notice("finished initializing NetworkControl\n");
	reconnect();
}

NetworkControl::~NetworkControl() {
}

void NetworkControl::reconnect() {
	// Loop until we're reconnected
	if (!mqttClient->connected()) {
		Log.notice("Attempting MQTT connection...\n");
		// Attempt to connect
		char mqtt_clientId[50];
		Prefs::getInstance()->get("mqtt-clientId", mqtt_clientId);
		if (mqttClient->connect(mqtt_clientId)) {
			Log.notice("connected\n");
		} else {
			Log.error("failed, rc=%d try again in 5 minutes\n", mqttClient->state());
		}
	}
}


void NetworkControl::loop() {
	mqttClient->loop();
}


char* substr(const char *buff, uint8_t start,uint8_t len, char* substr)
{
    strncpy(substr, buff+start, len);
    substr[len] = 0;
    return substr;
}

void NetworkControl::messageReceived(const char *topic, const char *message) {
	messageDispatcher->messageReceived(topic, message);
}

void NetworkControl::subscribeToCommand(const char *command, NetworkModule *networkModule) {
	messageDispatcher->subscribeToCommand(command, networkModule);
}

void NetworkControl::registerConfigParam(const char *configId, const char *prompt, const char *defaultValue, int length) {
	messageDispatcher->registerConfigParam(configId, prompt, defaultValue, length);
}


void NetworkControl::send(const char *topic, const char *message) {
	mqttClient->publish(topic, message);
}

bool NetworkControl::exists() {
	return instance != 0;
}


bool NetworkControl::isConnected() {
	return mqttClient->connected();
}

//gets called when WiFiManager enters configuration mode
void NetworkControl::configModeCallback(WiFiManager *myWiFiManager)
{
  Log.notice("Entered config mode %s\n", WiFi.softAPIP().toString());
  //if you used auto generated SSID, print it
  Log.notice("%s\n", myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  LedController::getInstance()->blinkFast();
}

void NetworkControl::enterConfigPortal() {
	//set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	wifiManager.setAPCallback(configModeCallback);

	// id/name, placeholder/prompt, default, length
	WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
	wifiManager.addParameter(&custom_mqtt_server);

	for (int i = 0; i < numberOfWifiManagerParams; i++) {
		wifiManager.addParameter(wifiManagerParams[i]);
	}
	
	if (!wifiManager.startConfigPortal("OnDemandAP", "geheim")) {
		Log.warning("failed to connect and hit timeout\n");
		delay(3000);
		//reset and try again, or maybe put it to deep sleep
		ESP.restart();
		delay(5000);
	}

	strcpy(mqtt_server, custom_mqtt_server.getValue());
	mqttClient->setServer(mqtt_server, 1883);      

	prefs->set("mqtt-server", mqtt_server);
	Log.notice("saved mqtt-server to eeprom: %s\n", mqtt_server);	
}

