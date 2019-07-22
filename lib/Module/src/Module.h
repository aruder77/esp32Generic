#ifndef MODULE_H_
#define MODULE_H_

#include <Prefs.h>

class Module {

    public:
        virtual const char *getName();
        virtual void setup();
        virtual void afterSetup();
        virtual void loop();
        virtual void every10Milliseconds();
        virtual void every50Milliseconds();
        virtual void every100Milliseconds();
        virtual void everySecond();
        virtual void messageReceived(const char *topic, const char *payload);
        virtual void getTelemetryData(char *targetBuffer);
        virtual void configUpdate(const char *id, const char *value);

};

#endif
