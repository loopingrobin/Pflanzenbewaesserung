#ifndef PUMP_H
#define PUMP_H

#include <Arduino.h>

class Pump {
public:
    Pump(uint8_t pin);
    void start(int durationSeconds);
    void update();
    bool isRunning() const;

private:
    uint8_t _pin;
    bool _running;
    unsigned long _startTime;
    unsigned long _duration;
};

#endif