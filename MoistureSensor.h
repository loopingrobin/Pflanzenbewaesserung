#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#include <Arduino.h>

class MoistureSensor {
public:
    MoistureSensor(uint8_t pin);
    int read();

private:
    uint8_t _pin;
};

#endif