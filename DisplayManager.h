#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class DisplayManager {
public:
    DisplayManager();
    void begin();
    void showMenu(int t1, int d1, int t2, int d2, int t3, int d3, int index);
    void showInfo(int m1, int m2, int m3, float temp, float hum);
    void clear();

private:
    Adafruit_SSD1306 display;
};

#endif