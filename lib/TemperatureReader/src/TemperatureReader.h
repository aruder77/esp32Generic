#ifndef TEMERATUREREADER_H_
#define TEMERATUREREADER_H_

#include <Prefs.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <ArduinoLog.h>
#include <Filters.h>

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
        static constexpr float OUTSIDE_TEMP_FACTOR = 3.96;
        static constexpr float OUTSIDE_TEMP_OFFSET = 1002;
        static constexpr float FLOW_TEMP_FACTOR = 3.78;
        static constexpr float FLOW_TEMP_OFFSET = 1004;
        static constexpr float RETURN_TEMP_FACTOR = 5.0;
        static constexpr float RETURN_TEMP_OFFSET = 929;
        static const int REF_RESISTOR = 3883;
        static const int REF_VOLTAGE = 3309;        
        static const int JITTER_TOLERANCE = 1;
        static constexpr float LOW_PASS_FREQUENCY = 0.1;

        Prefs *prefs;
        FilterOnePole *afFilter;
        FilterOnePole *ruefFilter;
        FilterOnePole *vfFilter;

        uint8_t atFuehlerPin = 34;
        uint8_t vFuehlerPin = 32;
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

        int outsideTemperatureCount = 0;
        double outsideTemperature[10] = {0.0};
        int flowTemperatureCount = 0;
        double flowTemperature[10] = {0.0};
        int returnTemperatureCount = 0;
        double returnTemperature[10] = {0.0};

        uint32_t readVoltage(adc1_channel_t channel);
        uint32_t calculateResistence(uint32_t voltage);
        double calculateTemperature(uint32_t resistence, double offset, double factor);  
        double calcOversampledTemperature(double samplingData[]);      
};

#endif