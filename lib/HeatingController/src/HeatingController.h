#ifndef HEATING_CONTROLLER_H_
#define HEATING_CONTROLLER_H_

#include <Module.h>
#include <Prefs.h>
#include <ArduinoLog.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

class HeatingController : public Module, public PrefsClient {

    public:
        HeatingController();

        const char *getName();
        void setup();
        void everySecond();

        void getTelemetryData(char *targetBuffer);

        void configUpdate(const char *id, const char *value);

    private:
        static constexpr float RESISTANCE_FACTOR = 1.429f;
        static constexpr float TEMP_FACTOR = 7.5f;
        static constexpr float TEMP_OFFSET = 821;
        static const int REF_RESISTOR = 1515;
        static const int REF_VOLTAGE = 3309;

        Prefs *prefs = Prefs::getInstance();
        int atFuehlerPin = 34;

        esp_adc_cal_characteristics_t *adc_chars;

        unsigned long timer = 0;
        int loopCounter = 0;

        int afAdc = 0;
        int afResistence = 0;
        float afTemp = 0.0f;
        int ruefAdc = 0;
        int ruefResistence = 0;
        float ruefTemp = 0.0f;
        int vfAdc = 0;
        int vfResistence = 0;
        float vfTemp = 0.0f;

        uint32_t readVoltage(adc1_channel_t channel);
        uint32_t calculateResistence(uint32_t voltage);
        uint32_t calculateTemperature(uint32_t resistence);
};

#endif