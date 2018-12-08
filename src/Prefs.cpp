#include "Prefs.h"

Prefs *Prefs::instance = 0;

Prefs* Prefs::getInstance()
{
    // The only instance
    // Guaranteed to be lazy initialized
    // Guaranteed that it will be destroyed correctly
	if (!Prefs::instance) {
		Prefs::instance = new Prefs();
	}
	return Prefs::instance;
}

Prefs::Prefs() {
}

void Prefs::set(char *key, char *value) {
    preferences.begin("espGeneric", false);
    preferences.putString(key, value);
    preferences.end();
} 

void Prefs::get(char *key, char *destinationBuffer) {
    preferences.begin("espGeneric", true);
    strcpy(destinationBuffer, preferences.getString(key).c_str());
    preferences.end();    
}

void Prefs::registerConfigParam(char *id, char *prompt, char *defaultValue, int length, Module *module) {
    configIds[numberOfConfigItems] = id;
    modules[numberOfConfigItems] = module;
    numberOfConfigItems++;

    NetworkControl::getInstance()->registerConfigParam(id, prompt, defaultValue, length);
}

void Prefs::configUpdate(char *id, char *value) {
    set(id, value);
    Module *module = getModuleForConfigId(id);
    module->configUpdate(id, value);
}

Module *Prefs::getModuleForConfigId(char *configId) {
    for (int i = 0; i < numberOfConfigItems; i++) {
        if (strncmp(configId, configIds[i], strlen(configId))) {
            return modules[i];
        }
    }
}