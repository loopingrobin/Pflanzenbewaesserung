#include "PlantUnit.h"

PlantUnit::PlantUnit(uint8_t sensorPin, uint8_t pumpPin, int eepromAddr)
  : sensor(sensorPin), pump(pumpPin), eepromBase(eepromAddr) {}

void PlantUnit::begin() {
    EEPROM.get(eepromBase + 0, threshold);
    EEPROM.get(eepromBase + 4, duration);
    EEPROM.get(eepromBase + 8, triggerAbove);
    EEPROM.get(eepromBase + 9, active);
}

void PlantUnit::saveSettings() {
    EEPROM.update(eepromBase + 0, threshold);
    EEPROM.update(eepromBase + 4, duration);
    EEPROM.update(eepromBase + 8, triggerAbove);
    EEPROM.update(eepromBase + 9, active);
}

void PlantUnit::update() {
    if (active) pump.update();
}

void PlantUnit::checkAndWater() {
    if (!active) return;
    int moisture = sensor.read();
    bool condition = triggerAbove ? (moisture > threshold) : (moisture < threshold);
    if (condition && !pump.isRunning()) {
        pump.start(duration);
    }
}

int PlantUnit::getMoisture() const { 
    return sensor.read(); 
}
