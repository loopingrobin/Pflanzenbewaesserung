#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <DHT.h>
#include "MoistureSensor.h"
#include "Pump.h"
#include "DisplayManager.h"
#include "ClimateSensor.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Pins
const int buttonDisplay = 8;
const int buttonUp = 9;
const int buttonDown = 10;
const int buttonSelect = 11;

// Objekte
MoistureSensor sensor1(A0);
MoistureSensor sensor2(A1);
MoistureSensor sensor3(A2);
Pump pump1(5);
Pump pump2(4);
Pump pump3(3);
ClimateSensor climate(6);
DisplayManager screen;

// Zustände
int threshold1 = 400, threshold2 = 400, threshold3 = 400;
int pumpDuration1 = 5, pumpDuration2 = 5, pumpDuration3 = 5;
unsigned long lastCheck = 0;
bool displayActive = false, infoScreen = false;
unsigned long displayTimeout = 0;
int menuIndex = 0;

// Entprellung
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

// Test-Modi
enum TestMode {
    TEST_NONE,
    TEST_BUTTONS,
    TEST_MOISTURE,
    TEST_PUMPS,
    TEST_DISPLAY,
    TEST_CLIMATE
};

TestMode currentTest = TEST_NONE; // Hier gewünschten Testmodus setzen

void testButtons() {
    screen.begin();
    screen.clear();
    screen.showMenu(0, 0, 0, 0, 0, 0, 0);
    while (true) {
        screen.clear();
        screen.showMenu(digitalRead(buttonDisplay), digitalRead(buttonUp),
                        digitalRead(buttonDown), digitalRead(buttonSelect), 0, 0, 0);
        delay(300);
    }
}

void testMoisture() {
    screen.begin();
    while (true) {
        int m1 = sensor1.read();
        int m2 = sensor2.read();
        screen.showInfo(m1, m2, 0, 0, 0);
        delay(1000);
    }
}

void testPumps() {
    pump1.start(1);
    delay(2000);
    pump2.start(1);
    delay(5000);
}

void testDisplay() {
    screen.begin();
    screen.showMenu(123, 4, 456, 7, 2, 345, 890);
    delay(2000);
    screen.showInfo(345, 567, 22.3, 58.9, 781.2);
    delay(3000);
    screen.clear();
}

void testClimate() {
    climate.begin();
    screen.begin();
    while (true) {
        float t = climate.getTemperature();
        float h = climate.getHumidity();
        screen.showInfo(0, 0, 0, t, h);
        delay(2000);
    }
}

// Kalibrierung
bool calibrateMode = false;

void setup() {
    pinMode(buttonDisplay, INPUT_PULLUP);
    pinMode(buttonUp, INPUT_PULLUP);
    pinMode(buttonDown, INPUT_PULLUP);
    pinMode(buttonSelect, INPUT_PULLUP);

    climate.begin();
    screen.begin();

    EEPROM.get(0, threshold1);
    EEPROM.get(4, threshold2);
    EEPROM.get(8, threshold3);
    EEPROM.get(12, pumpDuration1);
    EEPROM.get(16, pumpDuration2);
    EEPROM.get(20, pumpDuration3);
}

void loop() {
    // Testmodi
    switch (currentTest) {
        case TEST_BUTTONS: testButtons(); break;
        case TEST_MOISTURE: testMoisture(); break;
        case TEST_PUMPS: testPumps(); break;
        case TEST_DISPLAY: testDisplay(); break;
        case TEST_CLIMATE: testClimate(); break;
        default: break;
    }

    if (millis() - lastCheck > 120000) { // alle 2 Minuten
        lastCheck = millis();
        checkMoistureLevels();
    }

    pump1.update();
    pump2.update();
    pump3.update();

    handleDisplay();
    handleButtons();
}

void checkMoistureLevels() {
    int m1 = sensor1.read();
    int m2 = sensor2.read();
    int m3 = sensor3.read();
    if (m1 < threshold1 && !pump1.isRunning()) pump1.start(pumpDuration1);
    if (m2 < threshold2 && !pump2.isRunning()) pump2.start(pumpDuration2);
    if (m3 < threshold3 && !pump3.isRunning()) pump3.start(pumpDuration3);
}

void handleDisplay() {
    if (digitalRead(buttonDisplay) == LOW && millis() - lastButtonPress > debounceDelay) {
        displayActive = true;
        displayTimeout = millis();
        infoScreen = !infoScreen;
        lastButtonPress = millis();
    }
    if (displayActive) {
        if (infoScreen) {
            screen.showInfo(sensor1.read(), sensor2.read(), sensor3.read(), climate.getTemperature(), climate.getHumidity());
        } else {
            screen.showMenu(threshold1, pumpDuration1, threshold2, pumpDuration2, threshold3, pumpDuration3, menuIndex);
        }
        if (millis() - displayTimeout > 10000) {
            displayActive = false;
            screen.clear();
        }
    }
}

void handleButtons() {
    if (millis() - lastButtonPress < debounceDelay) return;

    if (digitalRead(buttonUp) == LOW) {
        if (calibrateMode) calibrateSensor(true);
        else adjustMenuValue(true);
        lastButtonPress = millis();
    }
    if (digitalRead(buttonDown) == LOW) {
        if (calibrateMode) calibrateSensor(false);
        else adjustMenuValue(false);
        lastButtonPress = millis();
    }
    if (digitalRead(buttonSelect) == LOW) {
        if (!calibrateMode) {
            menuIndex = (menuIndex + 1) % 6;
        } else {
            calibrateMode = false;
        }
        displayTimeout = millis();
        lastButtonPress = millis();
    }
}

void adjustMenuValue(bool up) {
    int change = up ? 10 : -10;
    switch (menuIndex) {
        case 0: threshold1 += change; break;
        case 1: pumpDuration1 += (up ? 1 : -1); break;
        case 2: threshold2 += change; break;
        case 3: pumpDuration2 += (up ? 1 : -1); break;
        case 4: threshold3 += change; break;
        case 5: pumpDuration3 += (up ? 1 : -1); break;
    }
    saveSettings();
    displayTimeout = millis();
}

void calibrateSensor(bool dry) {
    int value1 = sensor1.read();
    int value2 = sensor2.read();
    int value3 = sensor3.read();
    if (dry) {
        threshold1 = value1 + 50;
        threshold2 = value2 + 50;
        threshold3 = value3 + 50;
    } else {
        threshold1 = value1 - 50;
        threshold2 = value2 - 50;
        threshold3 = value3 - 50;
    }
    saveSettings();
}

void saveSettings() {
    EEPROM.put(0, threshold1);
    EEPROM.put(4, threshold2);
    EEPROM.put(8, threshold3);
    EEPROM.put(12, pumpDuration1);
    EEPROM.put(16, pumpDuration2);
    EEPROM.put(20, pumpDuration3);
}
