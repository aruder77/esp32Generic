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
#include <NetworkModule.h>
#include <LedController.h>
#include <Prefs.h>

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

	void registerConfigParam(const char *configId, const char *prompt, const char *defaultValue, int length);
	void subscribeToCommand(const char *command, NetworkModule *networkModule);

	void enterConfigPortal();

private:
	static NetworkControl* instance;
	static const int MAX_NUMBER_OF_SUBSCRIBED_MODULES = 10;

	PubSubClient* mqttClient;
    WiFiManager wifiManager;

	Prefs* prefs;
	LedController* ledController;

	int loop_counter = 0;
	char mqtt_server[100];

	short numberOfWifiManagerParams = 0;
	WiFiManagerParameter* wifiManagerParams[20];

	short numberOfModulesSubscribed = 0;
    const char *subscribedCommands[MAX_NUMBER_OF_SUBSCRIBED_MODULES];
	NetworkModule* modules[MAX_NUMBER_OF_SUBSCRIBED_MODULES];

	NetworkControl();
	void reconnect();

	static void configModeCallback(WiFiManager *myWiFiManager);
	void notifyModules(const char *commandName, const char *message);
};

#endif /* NETWORKCONTROL_H_ */
