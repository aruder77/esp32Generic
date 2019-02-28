#ifndef LED_CONTROLLER_H_
#define LED_CONTROLLER_H_

#include <Arduino.h>
#include <Ticker.h>

class LedController {
    public:
        static LedController *getInstance();

        void blink();
        void blinkFast();
        void blinkSlow();
        void on();
        void off();

    private:
        static LedController *instance;
        static const uint8_t STATUS_LED = GPIO_NUM_2;

        LedController();

        Ticker ticker;

        static void toggle();
};


#endif