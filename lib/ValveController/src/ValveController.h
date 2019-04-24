#ifndef VALVECONTROLLER_H_
#define VALVECONTROLLER_H_

#include <Prefs.h>
#include <math.h>
#include <ArduinoLog.h>


class ValveController : public PrefsClient {

    public:
        ValveController();
        void setup();
        void every10Milliseconds();
        void setTargetValvePosition(int targetValvePosition);

        void configUpdate(const char *id, const char *value);    

    private:
        static const int VALVE_ONE_PERCENT_OPEN_CYCLES = 45;

        Prefs *prefs = Prefs::getInstance();

        uint8_t openPin = 19;
        uint8_t closePin = 18;

        int motorAdjustCounter = 0;

        double valveCurrent = 0.0;
        int valveTarget = 0;
};

#endif