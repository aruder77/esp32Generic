#include "TemperatureReader.h"

TemperatureReader::TemperatureReader() {
    prefs = Prefs::getInstance();
    prefs->registerConfigParam("atFuehlerPin", "Außentemperatur-Fühler-Pin", "34", 3, this);
    prefs->registerConfigParam("vFuehlerPin", "Vorlauf-Temperatur-Fühler-Pin", "32", 3, this);
    prefs->registerConfigParam("rueFuehlerPin", "Rücklauf-Temperatur-Fühler-Pin", "35", 3, this);

    setup();
}

void TemperatureReader::setup() {
    atFuehlerPin = prefs->getInt("atFuehlerPin");
    vFuehlerPin = prefs->getInt("vFuehlerPin");
    rueFuehlerPin = prefs->getInt("rueFuehlerPin");

    //Characterize ADC at particular atten
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_0db, ADC_WIDTH_BIT_12, 3300, adc_chars);

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_0db);
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_0db);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_0db);

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

    afFilter = new FilterOnePole(LOWPASS, LOW_PASS_FREQUENCY);
    ruefFilter = new FilterOnePole(LOWPASS, LOW_PASS_FREQUENCY);
    vfFilter = new FilterOnePole(LOWPASS, LOW_PASS_FREQUENCY);
}

void TemperatureReader::readTemperatures() {
    uint32_t afVoltage = readVoltage(ADC1_CHANNEL_6);
    afFilter->input(afVoltage);

    uint32_t ruefVoltage = readVoltage(ADC1_CHANNEL_4);
    ruefFilter->input(ruefVoltage);

    uint32_t vfVoltage = readVoltage(ADC1_CHANNEL_7);
    vfFilter->input(vfVoltage);
}

uint32_t TemperatureReader::readVoltage(adc1_channel_t channel) {
    int adc = adc1_get_raw(channel);
    return esp_adc_cal_raw_to_voltage(adc, adc_chars);
}

uint32_t TemperatureReader::calculateResistence(uint32_t voltage) {
    return voltage * REF_RESISTOR / (REF_VOLTAGE - voltage);
}

double TemperatureReader::calculateTemperature(uint32_t resistence, double offset, double factor) {
    return (resistence - offset) / factor;
}


double TemperatureReader::getOutsideTemperature() {
    afResistence = calculateResistence(afFilter->output());
    return calculateTemperature(afResistence, OUTSIDE_TEMP_OFFSET, OUTSIDE_TEMP_FACTOR);
}

double TemperatureReader::getFlowTemperature() {
    vfResistence = calculateResistence(vfFilter->output());
    return calculateTemperature(vfResistence, OUTSIDE_TEMP_OFFSET, OUTSIDE_TEMP_FACTOR);
}

double TemperatureReader::getReturnTemperature() {
    ruefResistence = calculateResistence(ruefFilter->output());
    return calculateTemperature(ruefResistence, OUTSIDE_TEMP_OFFSET, OUTSIDE_TEMP_FACTOR);
}

void TemperatureReader::configUpdate(const char *id, const char *value) {
}
