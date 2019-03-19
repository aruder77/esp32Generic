#ifndef FLOWTEMPERATUREREGULATOR_H_
#define FLOWTEMPERATUREREGULATOR_H_

#include <PID_v1.h>


class FlowTemperatureRegulator {
    public:
        FlowTemperatureRegulator();
        int calculateValveTarget(double currentFlowTemperature, double targetFlowTemperature);

    private:
        PID *pidController = 0;

        double kP = 1;
        double kI = 0.05;
        double kD = 0.25;      

        double currentFlowTemperature = 0.0;
        double targetFlowTemperature = 40.0;
        
        double valveTarget = 0;
        double valveCurrent = 0;
};

#endif
