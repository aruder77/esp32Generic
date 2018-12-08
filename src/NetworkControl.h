/*
 * NetworkControl.h
 *
 *  Created on: 22 Jun 2017
 *      Author: axel
 */

#ifndef NETWORKCONTROL_H_
#define NETWORKCONTROL_H_

#include <SPI.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoLog.h>
#include <WiFi.h>
#include <WiFiManager.h>

#define RECONNECT_LOOP_COUNT 100


class NetworkControl {
public:
	virtual ~NetworkControl();
    static NetworkControl* getInstance();
    static bool exists();

	void loop();
	void messageReceived(const char *topic, const char *message);
	void send(const char *topic, const char *message);
	bool isConnected();

	void registerConfigParam(char *configId, char *prompt, char *defaultValue, int length);

private:
	static NetworkControl* instance;

	PubSubClient* mqttClient;
    WiFiManager wifiManager;


	int loop_counter = 0;
	char mqtt_server[100];

	NetworkControl();
	void reconnect();
};

#endif /* NETWORKCONTROL_H_ */
