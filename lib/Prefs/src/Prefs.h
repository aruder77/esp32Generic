#ifndef PREFS_H_
#define PREFS_H_

#include <Preferences.h>
#include <Module.h>

#define MAX_NUMBER_OF_CONFIG_ITEMS 100

class PrefsClient {
    public: 
        virtual void configUpdate(const char *id, const char *value);    
};

struct PrefsItem {
	char id[50] = {0};
	char prompt[100] = {0};
    char defaultValue[100] = {0};
	int length = 0;
    PrefsClient *prefsClient;
};

struct PrefsItems {
    PrefsItem *prefsItems[MAX_NUMBER_OF_CONFIG_ITEMS];
    int length;
};

/**
 * PrefsClients should:
 * - register prefs in constructor
 * - get initial pref values in setup
 * - update own pref value in configUpdate
 */
class Prefs {

    public:
        static Prefs* getInstance();    
        
        Prefs();
        void set(const char *key, const char *value);
        void setInt(const char *key, int value);
        void get(const char *key, char *destinationBuffer);
        int getInt(const char *key);
        double getDouble(const char *key);

        void registerConfigParam(const char *id, const char *prompt, const char *defaultValue, int length, PrefsClient *prefsClient);
        void configUpdate(const char *id, const char *value);

        PrefsItems *getPrefsItems();
        PrefsItem *getPrefsItem(const char *configId);

    private:
	    static Prefs* instance;

        Preferences preferences;   

        int numberOfConfigItems = 0;
        PrefsItems *prefsItems;

        PrefsClient *getPrefsClientForConfigId(const char *configId);
};



#endif