#ifndef TEMERATUREREADER_H_
#define TEMERATUREREADER_H_

#include <Prefs.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <ArduinoLog.h>

class TemperatureReader : public PrefsClient {

    public:
        TemperatureReader();
        void setup();
        void readTemperatures();
        double getOutsideTemperature();
        double getFlowTemperature();
        double getReturnTemperature();

        void configUpdate(const char *id, const char *value);    

    private:
        static constexpr float RESISTANCE_FACTOR = 1.429f;
        static constexpr float TEMP_FACTOR = 7.5f;
        static constexpr float TEMP_OFFSET = 821;
        static const int REF_RESISTOR = 1515;
        static const int REF_VOLTAGE = 3309;

        Prefs *prefs;

        uint8_t atFuehlerPin = 34;
        uint8_t vFuehlerPin = 33;
        uint8_t rueFuehlerPin = 35;

        esp_adc_cal_characteristics_t *adc_chars;

        int afAdc = 0;
        int afResistence = 0;
        float afTemp = 0.0f;
        int ruefAdc = 0;
        int ruefResistence = 0;
        float ruefTemp = 0.0f;
        int vfAdc = 0;
        int vfResistence = 0;
        double vfTemp = 0.0f;

        double outsideTemperature = 0.0;
        double flowTemperature = 0.0;
        double returnTemperature = 0.0;

        uint32_t readVoltage(adc1_channel_t channel);
        uint32_t calculateResistence(uint32_t voltage);
        uint32_t calculateTemperature(uint32_t resistence);        
};

#endif