#ifndef PREFS_H_
#define PREFS_H_

#include <Preferences.h>
#include <Module.h>

class Prefs {

    public:
        static Prefs* getInstance();    
        
        void set(const char *key, const char *value);
        void get(const char *key, char *destinationBuffer);

        void registerConfigParam(const char *id, const char *prompt, const char *defaultValue, int length, Module *module);
        void configUpdate(const char *id, const char *value);

    private:
        static const int MAX_NUMBER_OF_CONFIG_ITEMS = 100;
	    static Prefs* instance;

        Preferences preferences;   

        int numberOfConfigItems = 0;
        const char *configIds[MAX_NUMBER_OF_CONFIG_ITEMS];
        Module *modules[MAX_NUMBER_OF_CONFIG_ITEMS];

        Module *getModuleForConfigId(const char *configId);
};

#endif