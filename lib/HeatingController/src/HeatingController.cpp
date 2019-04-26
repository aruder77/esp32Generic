#include "HeatingController.h"

HeatingController::HeatingController() {
}

const char *HeatingController::getName() {
    return "heating";
}

void HeatingController::setup() { 
    temperatureReader = new TemperatureReader();
    flowTemperatureRegulator = new FlowTemperatureRegulator();
    targetFlowTemperatureCalculator = new TargetFlowTemperatureCalculator();
    valveController = new ValveController();
    heatPumpController = new HeatPumpController();

    // always on for now
    heatPumpController->on();
}

void HeatingController::configUpdate(const char *id, const char *value) {
}


void HeatingController::everySecond() {
    // every minute
    if (loopCounter == 10) {
        double outsideTemperature = temperatureReader->getOutsideTemperature();
        double flowTemperature = temperatureReader->getFlowTemperature();

        double targetFlowTemperature = targetFlowTemperatureCalculator->calculateTargetFlowTemperature(outsideTemperature);
        double valveTarget = flowTemperatureRegulator->calculateValveTarget(flowTemperature, targetFlowTemperature);
        char printStr[100];
        sprintf(printStr, "Ventil aktuell: %.1f, Ziel: %.1f\n", valveCurrent, valveTarget);
        Log.notice(printStr);
        valveController->setTargetValvePosition(valveTarget);
        valveCurrent = valveTarget;

        loopCounter = 0;
    }
    loopCounter++;
}

void HeatingController::every10Milliseconds() {
    valveController->every10Milliseconds();
    temperatureReader->readTemperatures();
}

void HeatingController::getTelemetryData(char *targetBuffer) {
    double outsideTemperature = temperatureReader->getOutsideTemperature();
    double flowTemperature = temperatureReader->getFlowTemperature();
    double returnTemperature = temperatureReader->getReturnTemperature();

    char telemetryData[100] = {0};
    sprintf(telemetryData, "{\"outside\":%.1f,\"return\":%.1f,\"flow\":%.1f}", outsideTemperature, returnTemperature, flowTemperature);
    strcpy(targetBuffer, telemetryData);
}