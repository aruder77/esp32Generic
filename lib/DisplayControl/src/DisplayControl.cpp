#include "DisplayControl.h"

DisplayControl::DisplayControl() {
    tft = new WROVER_KIT_LCD();
}

void DisplayControl::setup() {
    tft->begin();
    tft->setRotation(1);

    uint32_t id = tft->readId();
    if(id){
        Serial.println("======= WROVER ST7789V Display Test ========");
    } else {
        Serial.println("======= WROVER ILI9341 Display Test ========");
    }
    Serial.println("============================================");
    Serial.printf("Display ID:      0x%06X\n", id);    

    tft->fillScreen(WROVER_BLACK);
    tft->setCursor(0, 0);
    tft->setTextColor(WROVER_WHITE);  
    tft->setTextSize(1);
    tft->fillScreen(WROVER_CYAN);
    tft->println("Hello World!");    
}

const char *DisplayControl::getName() {
    return "DisplayControl";
}