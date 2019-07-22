#include <Module.h>

void Module::setup() {
}

void Module::afterSetup() {
}

void Module::loop() {
}

void Module::every10Milliseconds() {
}

void Module::every50Milliseconds() {
}

void Module::every100Milliseconds() {
}

void Module::everySecond() {
}

void Module::messageReceived(const char *topic, const char *payload) {
}

void Module::getTelemetryData(char *targetBuffer) {
    sprintf(targetBuffer, "\"\"");
}

void Module::configUpdate(const char *id, const char *value) {
}