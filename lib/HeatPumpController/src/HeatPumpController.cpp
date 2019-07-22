#include "HeatPumpController.h"

HeatPumpController::HeatPumpController() {
    prefs->registerConfigParam("pumpPin", "Heizungspumpe-Pin", "4", 3, this);
    pumpPin = prefs->getInt("pumpPin");
    pinMode(pumpPin, OUTPUT);
}

void HeatPumpController::on() {
    digitalWrite(pumpPin, HIGH);
}

void HeatPumpController::off() {
    digitalWrite(pumpPin, LOW);    
}

void HeatPumpController::configUpdate(const char *id, const char *value) {
    Log.notice("HeatPumpController config update: %s\n", id);
}