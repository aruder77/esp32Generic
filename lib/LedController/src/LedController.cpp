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
    prefs->registerConfigParam("StatusLedPin", "Status-LED Pin", "2", 3, this);

    statusLedPin = prefs->getInt("StatusLedPin");

    //set led pin as output
    pinMode(statusLedPin, OUTPUT);
}

void LedController::toggle()
{
    //toggle state
    int pin = LedController::getInstance()->statusLedPin;
    int state = digitalRead(pin); // get the current state of GPIO1 pin
    digitalWrite(pin, !state);    // set pin to the opposite state
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
    digitalWrite(statusLedPin, HIGH);  
}

void LedController::off() {
    ticker.detach();
    digitalWrite(statusLedPin, LOW);  
}

void LedController::configUpdate(const char *key, const char *value) {
    statusLedPin = atoi(value);
}