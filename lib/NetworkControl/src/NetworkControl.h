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
#include <MessageDispatcher.h>

#define RECONNECT_LOOP_COUNT 100


class NetworkControl : public Module, public PrefsClient {
public:
	virtual ~NetworkControl();
    static NetworkControl* getInstance();
    static bool exists();

	const char *getName();
	void afterSetup();
	void loop();
	void everySecond();	
	
	void messageReceived(const char *topic, const char *message);
	void send(const char *topic, const char *message);
	void sendTelemetry(const char *data);
	void sendStat(const char *subTopic, const char *message);
	bool isConnected();

	void subscribeToCommand(const char *command, NetworkModule *networkModule);

	void enterConfigPortal();
    void configUpdate(const char *id, const char *value);

	void reset();

private:
	static NetworkControl* instance;

	PubSubClient* mqttClient;
    WiFiManager wifiManager;

	Prefs* prefs;
	LedController* ledController;
	MessageDispatcher* messageDispatcher = new MessageDispatcher();

	int loop_counter = 0;
	char mqtt_server[100] = {0};
	char clientId[20] = {0};

	WiFiManagerParameter **params;
	int wifiParamCount;

	NetworkControl();
	void reconnect();
	void subscribeTopic(const char *clientId);
	void unsubscribeTopic(const char *clientId);

	void addWifiManagerParameter();

	static void configModeCallback(WiFiManager *myWiFiManager);
	static void saveConfigCallback();
};

#endif /* NETWORKCONTROL_H_ */
