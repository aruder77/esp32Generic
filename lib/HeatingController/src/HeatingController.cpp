#include "HeatingController.h"

HeatingController::HeatingController() {
    prefs->registerConfigParam("atFuehlerPin", "Außentemperatur-Fühler-Pin", "34", 3, this);
}

const char *HeatingController::getName() {
    return "HeatingController";
}

void HeatingController::setup() {
    atFuehlerPin = prefs->getInt("atFuehlerPin");
}

void HeatingController::configUpdate(const char *id, const char *value) {
}

void HeatingController::everySecond() {
    uint16_t adc = analogRead(atFuehlerPin);
    Log.notice("ADC value: %d\n", adc);
}
