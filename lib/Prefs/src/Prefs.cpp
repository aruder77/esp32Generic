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
    prefsItems = new PrefsItems();
} 


void Prefs::set(const char *key, const char *value) {
    preferences.begin("esp32Generic", false);
    preferences.putString(key, value);
    Log.notice("Prefs->set setting value %s for key %s\n", value, key);
    preferences.end();
} 

void Prefs::setInt(const char *key, int value) {
    char strValue[7];
    itoa(value, strValue, 10);
    set(key, strValue);
}

void Prefs::get(const char *key, char *destinationBuffer) {
    preferences.begin("esp32Generic", true);
    strcpy(destinationBuffer, preferences.getString(key).c_str());
    if (strlen(destinationBuffer) == 0) {
        strcpy(destinationBuffer, getPrefsItem(key)->defaultValue);
        Log.notice("Prefs key %s using default value %s\n", key, destinationBuffer);
    }
    Log.notice("Prefs->get returning %s\n", destinationBuffer);
    preferences.end();    
}

int Prefs::getInt(const char *key) {
    char strValue[7];
    get(key, strValue);
    return atoi(strValue);
}

double Prefs::getDouble(const char *key) {
    char strValue[7];
    get(key, strValue);
    return atof(strValue);
}

void Prefs::registerConfigParam(const char *id, const char *prompt, const char *defaultValue, int length, PrefsClient *prefsClient) {
    PrefsItem *prefsItem = new PrefsItem();
    strcpy(prefsItem->id, id);
    strcpy(prefsItem->prompt, prompt);
    strcpy(prefsItem->defaultValue, defaultValue);
    prefsItem->length = length;    
    prefsItem->prefsClient = prefsClient;

    prefsItems->prefsItems[numberOfConfigItems] = prefsItem;

    Log.notice("registering %d with key %s with default value %s\n", numberOfConfigItems, prefsItems->prefsItems[numberOfConfigItems]->id, prefsItems->prefsItems[numberOfConfigItems]->defaultValue);

    numberOfConfigItems++;
    prefsItems->length = numberOfConfigItems;
}


void Prefs::configUpdate(const char *id, const char *value) {
    PrefsClient *prefsClient = getPrefsClientForConfigId(id);
    prefsClient->configUpdate(id, value);
    set(id, value);
}

PrefsClient *Prefs::getPrefsClientForConfigId(const char *configId) {
    for (int i = 0; i < numberOfConfigItems; i++) {
        if (strcmp(configId, prefsItems->prefsItems[i]->id) == 0) {
            return prefsItems->prefsItems[i]->prefsClient;
        }
    }
    return NULL;
}

PrefsItem *Prefs::getPrefsItem(const char *configId) {
    Log.notice("getPrefsItem for key %s\n", configId);
    for (int i = 0; i < numberOfConfigItems; i++) {
        if (strcmp(configId, prefsItems->prefsItems[i]->id) == 0) {
            Log.notice("match! using pref id %s with default value %s\n", configId, prefsItems->prefsItems[i]->defaultValue);
            Log.notice("prefs match: %s, %s\n", prefsItems->prefsItems[i]->id, prefsItems->prefsItems[i]->defaultValue);
            return prefsItems->prefsItems[i];
        }
    }
    return NULL;
}

PrefsItems *Prefs::getPrefsItems() {
    return prefsItems;
}