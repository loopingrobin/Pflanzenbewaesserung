#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "PlantUnit.h"

class DisplayManager {
public:
    int infoPage = 0;   // aktuelle Info-Seite

    DisplayManager();
    void begin();
    void clear();
    void showMenu(PlantUnit* plants[], int count, int index);
    void showInfo(PlantUnit* plants[], int count, float temp, float hum);
    void nextInfoPage(int plantCount);

private:
    Adafruit_SSD1306 display;
};

#endif