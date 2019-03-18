#ifndef DISPLAYCONTROL_H_
#define DISPLAYCONTROL_H_

#include <SPI.h>
#include <Module.h>
#include <Adafruit_GFX.h>
#include <WROVER_KIT_LCD.h>

class DisplayControl : public Module {

    public:
        DisplayControl();

        const char *getName();
        void setup();    

    private:
        WROVER_KIT_LCD *tft;
};

#endif