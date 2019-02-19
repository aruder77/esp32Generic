#include <unity.h>
#include <Arduino.h>
#include <LedController.h>

LedController *ledController = LedController::getInstance();

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_onOff(void) {
    Serial.println("LED on!");
    ledController->on();
    delay(5000);
    Serial.println("LED off!");
    ledController->off();
    delay(5000);
}

void test_blink(void) {
    Serial.println("LED blink slow!");
    ledController->blinkSlow();
    delay(5000);
    Serial.println("LED blink!");
    ledController->blink();
    delay(5000);
    Serial.println("LED blink fast!");
    ledController->blinkFast();
    delay(5000);
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(test_onOff);
    RUN_TEST(test_blink);
    UNITY_END();
}

void loop() {
}