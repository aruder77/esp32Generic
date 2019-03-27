#ifndef FLOWTEMPERATUREREGULATOR_H_
#define FLOWTEMPERATUREREGULATOR_H_

#include <PID_v1.h>
#include <ArduinoLog.h>


class FlowTemperatureRegulator {
    public:
        FlowTemperatureRegulator();
        double calculateValveTarget(double currentFlowTemperature, double targetFlowTemperature);

    private:
        PID *pidController = 0;

        double kP = 2.0;
        double tN = 120.0;
        double kI = kP/tN;
        double kD = 0.0;      

        double currentFlowTemperature = 20.0;
        double targetFlowTemperature = 40.0;
        
        double valveTarget = 0.0;
        double valveCurrent = 0.0;
};

#endif
