#include "HeatingController.h"

HeatingController::HeatingController() {
    prefs->registerConfigParam("atFuehlerPin", "Außentemperatur-Fühler-Pin", "34", 3, this);
}

const char *HeatingController::getName() {
    return "HeatingController";
}

void HeatingController::setup() {
    atFuehlerPin = prefs->getInt("atFuehlerPin");
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_6db);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_6db);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_6db);
}

void HeatingController::configUpdate(const char *id, const char *value) {
}

void HeatingController::loop() {
    unsigned long currentMillis = millis();
    if (currentMillis > timer) {
        timer = currentMillis + 1000;
        everySecond();
    }
}

void HeatingController::everySecond() {
    int adc = adc1_get_raw(ADC1_CHANNEL_6);
    afResistence = ((float)(4096-adc))/RESISTANCE_FACTOR;
    afTemp = (afResistence - TEMP_OFFSET) / TEMP_FACTOR;
    char afBuffer[100] = {0};
    sprintf(afBuffer, "ADC value AF: %d Ohm %.1f °C\n", afResistence, afTemp);
    Log.notice(afBuffer);

    int adc1 = adc1_get_raw(ADC1_CHANNEL_5);
    ruefResistence = ((float)(4096-adc1))/RESISTANCE_FACTOR;
    ruefTemp = (ruefResistence - TEMP_OFFSET) / TEMP_FACTOR;
    char ruefBuffer[100] = {0};
    sprintf(afBuffer, "ADC value RüF: %d Ohm %.1f °C\n", ruefResistence, ruefTemp);
    Log.notice(ruefBuffer);

    int adc2 = adc1_get_raw(ADC1_CHANNEL_7);
    vfResistence = ((float)(4096-adc2))/RESISTANCE_FACTOR;
    vfTemp = (vfResistence - TEMP_OFFSET) / TEMP_FACTOR;
    char vfBuffer[100] = {0};
    sprintf(vfBuffer, "ADC value VF: %d Ohm %.1f °C\n", vfResistence, vfTemp);
    Log.notice(vfBuffer);}
