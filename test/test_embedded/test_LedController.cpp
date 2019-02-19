#include <unity.h>
#include <Arduino.h>
#include <LedController.h>
#include <MessageDispatcher.h>
#include <NetworkModule.h>

LedController *ledController = LedController::getInstance();

MessageDispatcher *messageDispatcher = new MessageDispatcher();

class TestModule : public NetworkModule {
public:
    virtual void commandReceived(const char *command, const char *payload) {
        strcpy(receivedCommand, command);
        strcpy(receivedPayload, payload);
    }

    virtual const char *getTelemetryData() {
    }

    const char *getReceivedCommand() {
        return receivedCommand;
    }

    const char *getReceivedPayload() {
        return receivedPayload;
    }


private:
    char receivedCommand[100];
    char receivedPayload[100];
};

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_LedController_onOff(void) {
    Serial.println("LED on!");
    ledController->on();
    delay(5000);
    Serial.println("LED off!");
    ledController->off();
    delay(5000);
}

void test_LedController_blink(void) {
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

void test_MessageDispatcher_commands(void) {
    TestModule *testModule1 = new TestModule();
    TestModule *testModule2 = new TestModule();
    messageDispatcher->subscribeToCommand("testCommand1", testModule1);
    messageDispatcher->subscribeToCommand("testCommand12", testModule2);
    messageDispatcher->subscribeToCommand("testCommand3", testModule1);

    messageDispatcher->messageReceived("cmnd/testDevice/testCommand1", "payload1");
    messageDispatcher->messageReceived("cmnd/testDevice/testCommand12", "payload2");

    TEST_ASSERT_EQUAL_STRING("testCommand1", testModule1->getReceivedCommand());
    TEST_ASSERT_EQUAL_STRING("payload1", testModule1->getReceivedPayload());
    TEST_ASSERT_EQUAL_STRING("testCommand12", testModule2->getReceivedCommand());
    TEST_ASSERT_EQUAL_STRING("payload2", testModule2->getReceivedPayload());

    messageDispatcher->messageReceived("cmnd/testDevice/testCommand3", "payload3");
    TEST_ASSERT_EQUAL_STRING("testCommand3", testModule1->getReceivedCommand());
    TEST_ASSERT_EQUAL_STRING("payload3", testModule1->getReceivedPayload());
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(test_LedController_onOff);
    RUN_TEST(test_LedController_blink);
    RUN_TEST(test_NetworkControl_commands);
    UNITY_END();
}

void loop() {
}
