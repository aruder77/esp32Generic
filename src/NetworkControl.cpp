/*
 * NetworkControl.cpp
 *
 *  Created on: 22 Jun 2017
 *      Author: axel
 */

#include "NetworkControl.h"
#include "Prefs.h"



NetworkControl *NetworkControl::instance = 0;

void callback(char* topic, byte* payload, unsigned int length) {
	char data[length + 1];
	for (unsigned int i = 0; i < length; i++) {
	    data[i] = (char)payload[i];
	}
	data[length] = 0;
	NetworkControl::getInstance()->messageReceived(topic, data);
}

NetworkControl::NetworkControl() {
	Log.notice("Initializing NetworkControl\n");

  	WiFiClient *espClient = new WiFiClient();
  	mqttClient = new PubSubClient(*espClient);

	mqttClient->setCallback(callback);

	Prefs::getInstance()->get("mqtt-server", mqtt_server);
	Log.notice("loaded mqtt-server from eeprom: %s\n", mqtt_server);
  	mqttClient->setServer(mqtt_server, 1883);

  	if (wifiManager.autoConnect()) {
    	//keep LED on
    	digitalWrite(BUILTIN_LED, HIGH);        
  	}

	// Allow the hardware to sort itself out
	delay(1500);
	Log.notice("finished initializing NetworkControl\n");
	reconnect();
}

NetworkControl::~NetworkControl() {
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


void NetworkControl::loop() {
	if (!mqttClient->loop() && loop_counter > RECONNECT_LOOP_COUNT) {
		reconnect();
		loop_counter = 0;
	}
	loop_counter++;
}


char* substr(const char *buff, uint8_t start,uint8_t len, char* substr)
{
    strncpy(substr, buff+start, len);
    substr[len] = 0;
    return substr;
}

void NetworkControl::messageReceived(const char *topic, const char *message) {
	Log.notice("Message received [%s]: %s\n", topic, message);
}


void NetworkControl::send(const char *topic, const char *message) {
	mqttClient->publish(topic, message);
}

bool NetworkControl::exists() {
	return instance != 0;
}

void NetworkControl::reconnect() {
  // Loop until we're reconnected
  if (!mqttClient->connected()) {
	Log.notice("Attempting MQTT connection...\n")
    // Attempt to connect
    if (mqttClient->connect("arduinoClient")) {
	  Log.notice("connected\n");
      mqttClient->subscribe("ventilation/input/#");
      mqttClient->publish("ventilation/logging", "Ventilation client connected.");
    } else {
	  Log.error("failed, rc=%d try again in 5 minutes\n", mqttClient->state());
    }
  }
}

bool NetworkControl::isConnected() {
	return mqttClient->connected();
}
