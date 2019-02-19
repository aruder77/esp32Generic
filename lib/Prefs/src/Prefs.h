#ifndef PREFS_H_
#define PREFS_H_

#include <Preferences.h>
#include <Module.h>

class Prefs {

    public:
        static Prefs* getInstance();    
        
        void set(char *key, char *value);
        void get(char *key, char *destinationBuffer);

        void registerConfigParam(char *id, char *prompt, char *defaultValue, int length, Module *module);
        void configUpdate(char *id, char *value);

    private:
        static const int MAX_NUMBER_OF_CONFIG_ITEMS = 100;
	    static Prefs* instance;

        Preferences preferences;   

        int numberOfConfigItems = 0;
        char *configIds[MAX_NUMBER_OF_CONFIG_ITEMS];
        Module *modules[MAX_NUMBER_OF_CONFIG_ITEMS];

        Module *getModuleForConfigId(char *configId);
};

#endif