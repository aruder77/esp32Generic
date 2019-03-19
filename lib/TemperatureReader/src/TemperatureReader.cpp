#include "TemperatureReader.h"

TemperatureReader::TemperatureReader() {
    prefs = Prefs::getInstance();
    prefs->registerConfigParam("atFuehlerPin", "Außentemperatur-Fühler-Pin", "34", 3, this);
    prefs->registerConfigParam("vFuehlerPin", "Vorlauf-Temperatur-Fühler-Pin", "33", 3, this);
    prefs->registerConfigParam("rueFuehlerPin", "Rücklauf-Temperatur-Fühler-Pin", "35", 3, this);

    setup();
}

void TemperatureReader::setup() {
    atFuehlerPin = prefs->getInt("atFuehlerPin");
    vFuehlerPin = prefs->getInt("vFuehlerPin");
    rueFuehlerPin = prefs->getInt("rueFuehlerPin");

    //Characterize ADC at particular atten
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, 2200, adc_chars);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_11db);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_11db);

    Log.notice("coeff_a: %d\n", adc_chars->coeff_a);
    Log.notice("coeff_b: %d\n", adc_chars->coeff_b);
    Log.notice("v_ref: %d\n", adc_chars->vref);
    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Log.notice("eFuse Vref\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Log.notice("Two Point\n");
    } else {
        Log.notice("Default\n");
    }    
}

void TemperatureReader::readTemperatures() {
    uint32_t afVoltage = readVoltage(ADC1_CHANNEL_6);
    afResistence = calculateResistence(afVoltage);
    afTemp = calculateTemperature(afResistence);
    char afBuffer[100] = {0};
    sprintf(afBuffer, "ADC value AF: %d, Voltage: %d mV, Resistence: %d Ohm, Temperature: %.1f °C\n", afAdc, afVoltage, afResistence, afTemp);
    Log.notice(afBuffer);

    uint32_t ruefVoltage = readVoltage(ADC1_CHANNEL_5);
    ruefResistence = calculateResistence(ruefVoltage);
    ruefTemp = calculateTemperature(ruefResistence);
    char ruefBuffer[100] = {0};
    sprintf(ruefBuffer, "ADC value RüF: %d Resistence: %d Ohm Temperature: %.1f °C\n", ruefAdc, ruefResistence, ruefTemp);
    Log.notice(ruefBuffer);

    uint32_t vfVoltage = readVoltage(ADC1_CHANNEL_7);
    vfResistence = calculateResistence(vfVoltage);
    vfTemp = calculateTemperature(vfResistence);
    char vfBuffer[100] = {0};
    sprintf(vfBuffer, "ADC value VF: %d Resistence: %d Ohm Temperature: %.1f °C\n", vfAdc, vfResistence, vfTemp);
    Log.notice(vfBuffer);    
}

uint32_t TemperatureReader::readVoltage(adc1_channel_t channel) {
    int adc = adc1_get_raw(channel);
    return esp_adc_cal_raw_to_voltage(adc, adc_chars);
}

uint32_t TemperatureReader::calculateResistence(uint32_t voltage) {
    return voltage * REF_RESISTOR / (REF_VOLTAGE - voltage);
}

uint32_t TemperatureReader::calculateTemperature(uint32_t resistence) {
    return (resistence - TEMP_OFFSET) / TEMP_FACTOR;
}

double TemperatureReader::getOutsideTemperature() {
    return outsideTemperature;
}

double TemperatureReader::getFlowTemperature() {
    return flowTemperature;
}

double TemperatureReader::getReturnTemperature() {
    return returnTemperature;
}

void TemperatureReader::configUpdate(const char *id, const char *value) {
}
