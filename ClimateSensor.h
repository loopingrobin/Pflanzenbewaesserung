#ifndef CLIMATE_SENSOR_H
#define CLIMATE_SENSOR_H

#include <Arduino.h>
#include <DHT.h>

class ClimateSensor {
public:
    ClimateSensor(uint8_t pin);
    void begin();
    float getTemperature();
    float getHumidity();

private:
    DHT dht;
};

#endif
