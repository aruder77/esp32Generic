#ifndef PREFS_H_
#define PREFS_H_

#include <Preferences.h>
#include <Module.h>

struct PrefsItem {
	char id[50];
	char prompt[100];
    char defaultValue[100];
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

        PrefsItem** getPrefsItems();

    private:
        static const int MAX_NUMBER_OF_CONFIG_ITEMS = 100;
	    static Prefs* instance;

        Preferences preferences;   

        int numberOfConfigItems = 0;
        PrefsItem prefsItems[MAX_NUMBER_OF_CONFIG_ITEMS];
        Module *modules[MAX_NUMBER_OF_CONFIG_ITEMS];

        Module *getModuleForConfigId(const char *configId);
};

#endif