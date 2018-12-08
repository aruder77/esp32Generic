#include "LedController.h"

LedController *LedController::instance = 0;

LedController* LedController::getInstance()
{
    // The only instance
    // Guaranteed to be lazy initialized
    // Guaranteed that it will be destroyed correctly
	if (!LedController::instance) {
		LedController::instance = new LedController();
	}
	return LedController::instance;
}

void LedController::toggle()
{
    //toggle state
    int state = digitalRead(BUILTIN_LED); // get the current state of GPIO1 pin
    digitalWrite(BUILTIN_LED, !state);    // set pin to the opposite state
}

void LedController::blink() {
    ticker.attach(0.6, toggle);    
}

void LedController::blinkFast() {
    ticker.attach(0.1, toggle);
}

void LedController::blinkSlow() {
    ticker.attach(1.5, toggle);
}

void LedController::on() {
    if (ticker.active) {
        ticker.detach();
    }
    digitalWrite(BUILTIN_LED, HIGH);  
}

void LedController::off() {
    if (ticker.active) {
        ticker.detach();
    }
    digitalWrite(BUILTIN_LED, LOW);  
}