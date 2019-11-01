#include <Arduino.h>
#include <Controller.h>

Controller *controller;

void setup()
{
    controller = Controller::getInstance();
}

void loop()
{
    controller->loop();
}