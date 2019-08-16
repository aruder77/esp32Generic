#include <MessageDispatcher.h>

void MessageDispatcher::messageReceived(const char *topic, const char *message) {
	Log.notice("Message received [%s]: %s\n", topic, message);

	if (strncmp(topic, "cmnd/", 5) == 0) {
		const char *commandName = strchr(topic + 5, '/') + 1;
		notifyModules(commandName, message);
	} else if (strncmp(topic, "config/", 7) == 0) {
		char *parameterName = strchr(topic + 7, '/') + 1;
		prefs->configUpdate(parameterName, message);
	}
}

void MessageDispatcher::notifyModules(const char *commandName, const char *message) {
	for (int i = 0; i < numberOfModulesSubscribed; i++) {
		if (strncmp(commandName, subscribedCommands[i], strlen(subscribedCommands[i])) == 0) {
			modules[i]->commandReceived(commandName, message);
		}
	}
}

void MessageDispatcher::subscribeToCommand(const char *command, NetworkModule *networkModule) {
	subscribedCommands[numberOfModulesSubscribed] = command;
	modules[numberOfModulesSubscribed++] = networkModule;
}
