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

	Prefs *prefs = Prefs::getInstance();

	short numberOfModulesSubscribed = 0;
    const char *subscribedCommands[MAX_NUMBER_OF_SUBSCRIBED_MODULES];
	NetworkModule* modules[MAX_NUMBER_OF_SUBSCRIBED_MODULES];

	void notifyModules(const char *commandName, const char *message);
};

#endif