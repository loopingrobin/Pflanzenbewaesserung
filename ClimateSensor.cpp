#include "ClimateSensor.h"

ClimateSensor::ClimateSensor(uint8_t pin) : dht(pin, DHT11) {}

void ClimateSensor::begin() {
    dht.begin();
}

float ClimateSensor::getTemperature() {
    return dht.readTemperature();
}

float ClimateSensor::getHumidity() {
    return dht.readHumidity();
}
