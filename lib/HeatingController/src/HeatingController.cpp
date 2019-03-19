#include "HeatingController.h"

HeatingController::HeatingController() {
    prefs->registerConfigParam("pumpPin", "Heizungspumpe-Pin", "20", 3, this);
}

const char *HeatingController::getName() {
    return "heating";
}

void HeatingController::setup() {
    pumpPin = prefs->getInt("pumpPin");
 
    temperatureReader = new TemperatureReader();
    flowTemperatureRegulator = new FlowTemperatureRegulator();
    targetFlowTemperatureCalculator = new TargetFlowTemperatureCalculator();
    valveController = new ValveController();
}

void HeatingController::configUpdate(const char *id, const char *value) {
}


void HeatingController::everySecond() {
    if (loopCounter == 60) {
        temperatureReader->readTemperatures();
        double outsideTemperature = temperatureReader->getOutsideTemperature();
        double flowTemperature = temperatureReader->getFlowTemperature();

        double targetFlowTemperature = targetFlowTemperatureCalculator->calculateTargetFlowTemperature(outsideTemperature);
        int valveTarget = flowTemperatureRegulator->calculateValveTarget(flowTemperature, targetFlowTemperature);
        valveController->setTargetValvePosition(valveTarget);

        loopCounter = 0;
    }
    loopCounter++;
}

void HeatingController::every10Milliseconds() {
    valveController->every10Milliseconds();
}

void HeatingController::getTelemetryData(char *targetBuffer) {
    double outsideTemperature = temperatureReader->getOutsideTemperature();
    double flowTemperature = temperatureReader->getFlowTemperature();
    double returnTemperature = temperatureReader->getReturnTemperature();

    char telemetryData[100] = {0};
    sprintf(telemetryData, "{outsid:%.1f,return:%.1f,flow:%.1f}", outsideTemperature, returnTemperature, flowTemperature);
    strcpy(targetBuffer, telemetryData);
}