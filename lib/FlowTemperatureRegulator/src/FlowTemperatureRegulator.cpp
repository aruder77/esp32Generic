#include "FlowTemperatureRegulator.h"

FlowTemperatureRegulator::FlowTemperatureRegulator() {
    pidController = new PID(&currentFlowTemperature, &valveTarget, &targetFlowTemperature, kP, kI, kD, DIRECT);
    pidController->SetOutputLimits(0.0, 100.0);
    pidController->SetSampleTime(59000);
    pidController->SetMode(AUTOMATIC);
}


double FlowTemperatureRegulator::calculateValveTarget(double currentFlowTemperature, double targetFlowTemperature) {
    this->currentFlowTemperature = currentFlowTemperature;
    this->targetFlowTemperature = targetFlowTemperature;

    pidController->Compute();
    char buffer[100];
    sprintf(buffer, "FlowTemperatureRegulator: Current: %.1f, Target: %.1f, ValveTarget: %.1f\n", this->currentFlowTemperature, this->targetFlowTemperature, valveTarget);
    Log.notice(buffer);
    return valveTarget;
}

void FlowTemperatureRegulator::setTunings(double kp, double tn) {
    kP = kp;
    kI = kp/tn;
    pidController->SetTunings(kP, kI, kD, DIRECT);
}