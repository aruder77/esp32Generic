#ifndef MESSAGEDISPATCHER_H_
#define MESSAGEDISPATCHER_H_

#include <NetworkModule.h>
#include <Prefs.h>
#include <ArduinoLog.h>

class MessageDispatcher {

public:
	void messageReceived(const char *topic, const char *message);
	void subscribeToCommand(const char *command, NetworkModule *networkModule);

private:
	static const int MAX_NUMBER_OF_SUBSCRIBED_MODULES = 10;

	Prefs prefs;

	short numberOfModulesSubscribed = 0;
    const char *subscribedCommands[MAX_NUMBER_OF_SUBSCRIBED_MODULES];
	NetworkModule* modules[MAX_NUMBER_OF_SUBSCRIBED_MODULES];

	void notifyModules(const char *commandName, const char *message);
};

#endif