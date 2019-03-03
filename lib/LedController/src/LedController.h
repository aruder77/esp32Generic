#ifndef LED_CONTROLLER_H_
#define LED_CONTROLLER_H_

#include <Arduino.h>
#include <Ticker.h>
#include <Prefs.h>

class LedController : public PrefsClient {
    public:
        static LedController *getInstance();

        void blink();
        void blinkFast();
        void blinkSlow();
        void on();
        void off();

        void configUpdate(const char *key, const char *value);

    private:
        static LedController *instance;

        LedController();

        Ticker ticker;
        Prefs *prefs = Prefs::getInstance();
        uint8_t statusLedPin;

        static void toggle();
};


#endif