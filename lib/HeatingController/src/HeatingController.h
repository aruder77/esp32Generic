#ifndef HEATING_CONTROLLER_H_
#define HEATING_CONTROLLER_H_

#include <Module.h>
#include <Prefs.h>
#include <ArduinoLog.h>
#include <FlowTemperatureRegulator.h>
#include <TargetFlowTemperatureCalculator.h>
#include <ValveController.h>
#include <TemperatureReader.h>

class HeatingController : public Module, public PrefsClient {

    public:
        HeatingController();

        const char *getName();
        void setup();
        void everySecond();
        void every10Milliseconds();        

        void getTelemetryData(char *targetBuffer);

        void configUpdate(const char *id, const char *value);

    private:

        Prefs *prefs = Prefs::getInstance();

        TemperatureReader *temperatureReader;
        FlowTemperatureRegulator *flowTemperatureRegulator;
        TargetFlowTemperatureCalculator *targetFlowTemperatureCalculator;
        ValveController *valveController;

        uint8_t pumpPin = 20;

        unsigned long timer = 0;
        int loopCounter = 0;

        double targetTemp = 40.0;
        
        double valveTarget = 0;
        double valveCurrent = 0;

};

#endif