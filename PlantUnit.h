#ifndef PLANTUNIT_H
#define PLANTUNIT_H

#include "MoistureSensor.h"
#include "Pump.h"
#include <EEPROM.h>

class PlantUnit {
public:
    PlantUnit(uint8_t sensorPin, uint8_t pumpPin, int eepromAddr);
    void begin();
    void saveSettings();
    void update();
    void checkAndWater();
    int getMoisture() const;

    int threshold = 400;
    int duration = 5;
    bool triggerAbove = false;
    bool active = true;

private:
    MoistureSensor sensor;
    Pump pump;
    int eepromBase;
};

#endif