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
    EEPROM.put(eepromBase + 0, threshold);
    EEPROM.put(eepromBase + 4, duration);
    EEPROM.put(eepromBase + 8, triggerAbove);
    EEPROM.put(eepromBase + 9, active);
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
