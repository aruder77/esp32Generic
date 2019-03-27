#ifndef DISPLAYCONTROL_H_
#define DISPLAYCONTROL_H_

#include <SPI.h>
#include <Module.h>

class DisplayControl : public Module {

    public:
        DisplayControl();

        const char *getName();
        void setup();    

    private:
};

#endif