#ifndef HeatPumpController_H_
#define HeatPumpController_H_

#include <Prefs.h>

class HeatPumpController : public PrefsClient {
    public:
        HeatPumpController();
        void on();
        void off();
        void configUpdate(const char *id, const char *value);    

    private:
        Prefs *prefs = Prefs::getInstance();
        uint8_t pumpPin = 20;
};

#endif