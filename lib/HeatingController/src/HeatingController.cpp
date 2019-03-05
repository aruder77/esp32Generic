#include "HeatingController.h"

HeatingController::HeatingController() {
    prefs->registerConfigParam("atFuehlerPin", "Außentemperatur-Fühler-Pin", "34", 3, this);
}

void HeatingController::setup() {
    atFuehlerPin = prefs->getInt("atFuehlerPin");
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_6db);
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
    Log.notice("ADC value: %d\n", adc);
}
