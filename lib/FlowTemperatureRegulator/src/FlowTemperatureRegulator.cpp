#include "FlowTemperatureRegulator.h"

FlowTemperatureRegulator::FlowTemperatureRegulator() {
    pidController = new PID(&currentFlowTemperature, &valveTarget, &targetFlowTemperature, kP, kI, kD, DIRECT);
}


int FlowTemperatureRegulator::calculateValveTarget(double currentFlowTemperature, double targetFlowTemperature) {
    this->currentFlowTemperature = currentFlowTemperature;
    this->targetFlowTemperature = targetFlowTemperature;

    pidController->Compute();
}