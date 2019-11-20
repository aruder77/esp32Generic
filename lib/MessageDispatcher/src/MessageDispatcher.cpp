/*
  Copyright (C) 2019 Axel Ruder

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
