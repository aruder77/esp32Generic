#ifndef DISPLAYCONTROL_H_
#define DISPLAYCONTROL_H_

#include <SPI.h>
#include <Module.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>


class DisplayControl : public Module {

    public:
	    virtual ~DisplayControl();
        static DisplayControl* getInstance();        

        const char *getName();
        void setup();    

        void displayFlowTemperature(double flowTemperature);
        void displayReturnTemperature(double returnTemperature);
        void displayOutsideTemperature(double outsideTemperature);
        void displayValvePosition(int valvePosition);
        void displayPumpState(bool valveState);
        void displayVersion(char *version);
        void displayIPAddress(char *ipAddress);

    private:
	    static DisplayControl* instance;
        TFT_eSPI tft = TFT_eSPI(); 
};

#endif