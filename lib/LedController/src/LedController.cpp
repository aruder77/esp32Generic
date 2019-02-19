#include <LedController.h>

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

LedController::LedController() {
    //set led pin as output
    pinMode(STATUS_LED, OUTPUT);
}

void LedController::toggle()
{
    //toggle state
    int state = digitalRead(STATUS_LED); // get the current state of GPIO1 pin
    digitalWrite(STATUS_LED, !state);    // set pin to the opposite state
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
    ticker.detach();
    digitalWrite(STATUS_LED, HIGH);  
}

void LedController::off() {
    ticker.detach();
    digitalWrite(STATUS_LED, LOW);  
}