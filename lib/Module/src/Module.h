#ifndef MODULE_H_
#define MODULE_H_

class Module {

    public:
        virtual void setup();
        virtual void loop();
        virtual void everyMillisecond();
        virtual void every10Milliseconds();
        virtual void every50Milliseconds();
        virtual void every100Milliseconds();
        virtual void everySecond();
        virtual void messageReceived(const char *topic, const char *payload);
        virtual const char *getTelemetryData();
        virtual void configUpdate(const char *id, const char *value);

};

#endif
