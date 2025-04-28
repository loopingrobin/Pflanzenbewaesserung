#include "MoistureSensor.h"

MoistureSensor::MoistureSensor(uint8_t pin) : _pin(pin) {}

int MoistureSensor::read() {
    return analogRead(_pin);
}