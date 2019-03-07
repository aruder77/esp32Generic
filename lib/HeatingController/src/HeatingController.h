#ifndef HEATING_CONTROLLER_H_
#define HEATING_CONTROLLER_H_

#include <Module.h>
#include <Prefs.h>
#include <ArduinoLog.h>
#include <driver/adc.h>

class HeatingController : public Module, public PrefsClient {

    public:
        HeatingController();

        void setup();
        void loop();
        void everySecond();

        void configUpdate(const char *id, const char *value);

    private:
        static constexpr float RESISTANCE_FACTOR = 2.0073f;
        static constexpr float TEMP_FACTOR = 7.5f;
        static constexpr float TEMP_OFFSET = 821;

        Prefs *prefs = Prefs::getInstance();
        int atFuehlerPin = 34;

        unsigned long timer = 0;

        int afResistence = 0;
        float afTemp = 0.0f;
        int ruefResistence = 0;
        float ruefTemp = 0.0f;
        int vfResistence = 0;
        float vfTemp = 0.0f;
};

#endif