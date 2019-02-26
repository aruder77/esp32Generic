#ifndef PREFS_H_
#define PREFS_H_

#include <Preferences.h>
#include <Module.h>

#define MAX_NUMBER_OF_CONFIG_ITEMS 100

struct PrefsItem {
	char id[50] = {0};
	char prompt[100] = {0};
    char defaultValue[100] = {0};
	int length = 0;
    Module *module;
};

struct PrefsItems {
    PrefsItem *prefsItems[MAX_NUMBER_OF_CONFIG_ITEMS];
    int length;
};

class Prefs {

    public:
        static Prefs* getInstance();    
        
        Prefs();
        void set(const char *key, const char *value);
        void get(const char *key, char *destinationBuffer);

        void registerConfigParam(const char *id, const char *prompt, const char *defaultValue, int length, Module *module);
        void configUpdate(const char *id, const char *value);

        PrefsItems *getPrefsItems();
        PrefsItem *getPrefsItem(const char *configId);

    private:
	    static Prefs* instance;

        Preferences preferences;   

        int numberOfConfigItems = 0;
        PrefsItems *prefsItems;

        Module *getModuleForConfigId(const char *configId);
};

#endif