#include "Pump.h"

Pump::Pump(uint8_t pin) : _pin(pin), _running(false), _startTime(0), _duration(0) {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH); // Invertiert: Pumpe aus
}

void Pump::start(int durationSeconds) {
    _duration = durationSeconds * 1000; // in Millisekunden
    _startTime = millis();
    _running = true;
    digitalWrite(_pin, LOW); // Invertiert: Pumpe ein
}

void Pump::update() {
    if (_running && millis() - _startTime >= _duration) {
        digitalWrite(_pin, HIGH); // Invertiert: Pumpe aus
        _running = false;
    }
}

bool Pump::isRunning() const {
    return _running;
}