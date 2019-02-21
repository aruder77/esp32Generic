#include <Prefs.h>
#include <NetworkControl.h>


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


void Prefs::set(const char *key, const char *value) {
    preferences.begin("espGeneric", false);
    preferences.putString(key, value);
    preferences.end();
} 

void Prefs::get(const char *key, char *destinationBuffer) {
    preferences.begin("espGeneric", true);
    strcpy(destinationBuffer, preferences.getString(key).c_str());
    preferences.end();    
}

void Prefs::registerConfigParam(const char *id, const char *prompt, const char *defaultValue, int length, Module *module) {
    PrefsItem prefsItem;
    strcpy(prefsItem.id, id);
    strcpy(prefsItem.prompt, prompt);
    strcpy(prefsItem.defaultValue, defaultValue);
    prefsItems[numberOfConfigItems] = prefsItem;
    modules[numberOfConfigItems] = module;
    numberOfConfigItems++;
}


void Prefs::configUpdate(const char *id, const char *value) {
    set(id, value);
    Module *module = getModuleForConfigId(id);
    module->configUpdate(id, value);
}

Module *Prefs::getModuleForConfigId(const char *configId) {
    for (int i = 0; i < numberOfConfigItems; i++) {
        if (strncmp(configId, configIds[i], strlen(configId))) {
            return modules[i];
        }
    }
    return NULL;
}