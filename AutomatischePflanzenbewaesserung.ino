#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include "PlantUnit.h"
#include "DisplayManager.h"
#include "ClimateSensor.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Pins
const int buttonNext = 7;
const int buttonDisplay = 8;
const int buttonUp = 9;
const int buttonDown = 10;
const int buttonSelect = 11;

// Objekte
PlantUnit plant1(A0, 5, 0);
PlantUnit plant2(A1, 4, 16);
PlantUnit plant3(A2, 3, 32);
PlantUnit plant4(A3, 2, 48);
ClimateSensor climate(6);
DisplayManager screen;
PlantUnit* plants[] = { &plant1, &plant2, &plant3, &plant4 };

// Zustände
unsigned long lastCheck = 0;
bool displayActive = false, infoScreen = false;
unsigned long displayTimeout = 0;
int menuIndex = 0;

// Entprellung
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

void setup() {
    pinMode(buttonNext, INPUT_PULLUP);
    pinMode(buttonDisplay, INPUT_PULLUP);
    pinMode(buttonUp, INPUT_PULLUP);
    pinMode(buttonDown, INPUT_PULLUP);
    pinMode(buttonSelect, INPUT_PULLUP);

    pinMode(LED_BUILTIN, OUTPUT);

    climate.begin();
    screen.begin();

    for (auto plant : plants) plant->begin();
}

void loop() {
    // if (millis() - lastCheck > 120000) { // alle 8 Minuten
    if (millis() - lastCheck > 30000) { // alle 2 Minuten
        lastCheck = millis();
        for (auto plant : plants) plant->checkAndWater();
    }

    handleDisplay();
    handleButtons();
}

void handleDisplay() {
    if (digitalRead(buttonDisplay) == LOW && millis() - lastButtonPress > debounceDelay) {
        if (!displayActive) {
            infoScreen = true;  // Info-Screen setzen, wenn Display gerade ausgeschaltet war
        } else {
            infoScreen = !infoScreen;
        }
        displayActive = true;
        displayTimeout = millis();
        lastButtonPress = millis();
    }
    if (displayActive) {
        if (infoScreen) {
            screen.showInfo(plants, 4, climate.getTemperature(), climate.getHumidity());
        } else {
            screen.showMenu(plants, 4, menuIndex);
        }
        if (millis() - displayTimeout > 10000) {
            displayActive = false;
            screen.clear();
        }
    }
}

void handleButtons() {
    if (millis() - lastButtonPress < debounceDelay) return;

    if (digitalRead(buttonNext) == LOW) {
        screen.nextInfoPage(sizeof(plants)); 
    }
    if (digitalRead(buttonUp) == LOW) {
        int plantIndex = menuIndex / 4;
        int paramIndex = menuIndex % 4;
        if (paramIndex == 0) plants[plantIndex]->threshold += 10;
        if (paramIndex == 1) plants[plantIndex]->duration += 1;
        if (paramIndex == 2) plants[plantIndex]->triggerAbove = !plants[plantIndex]->triggerAbove;
        if (paramIndex == 3) plants[plantIndex]->active = !plants[plantIndex]->active;
        plants[plantIndex]->saveSettings();
        displayTimeout = millis();
        lastButtonPress = millis();
    }
    if (digitalRead(buttonDown) == LOW) {
        int plantIndex = menuIndex / 4;
        int paramIndex = menuIndex % 4;
        if (paramIndex == 0) plants[plantIndex]->threshold -= 10;
        if (paramIndex == 1 && plants[plantIndex]->duration > 1) plants[plantIndex]->duration -= 1;
        if (paramIndex == 2) plants[plantIndex]->triggerAbove = !plants[plantIndex]->triggerAbove;
        if (paramIndex == 3) plants[plantIndex]->active = !plants[plantIndex]->active;
        plants[plantIndex]->saveSettings();
        displayTimeout = millis();
        lastButtonPress = millis();
    }
    if (digitalRead(buttonSelect) == LOW) {
        menuIndex = (menuIndex + 1) % (sizeof(plants) / sizeof(plants[0]) * 4);
        displayTimeout = millis();
        lastButtonPress = millis();
    }
}
