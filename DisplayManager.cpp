#include "DisplayManager.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

DisplayManager::DisplayManager() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}

void DisplayManager::begin() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
}

void DisplayManager::clear() {
    display.clearDisplay();
    display.display();
}

void DisplayManager::showMenu(PlantUnit* plants[], int count, int menuIndex) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    int plantIndex = menuIndex / 4;
    int paramIndex = menuIndex % 4;
    PlantUnit* p = plants[plantIndex];

    display.setCursor(0, 0);
    display.print("Pflanze "); display.print(plantIndex + 1);

    display.setCursor(0, 10);
    display.print("Sch: ");
    display.print(p->threshold);
    if (paramIndex == 0) display.print(" <");

    display.setCursor(0, 20);
    display.print("Dau: ");
    display.print(p->duration);
    if (paramIndex == 2) display.print(" <");

    display.setCursor(64, 10);
    // display.print("Tri: ");
    display.print(p->triggerAbove ? ">" : "<");
    if (paramIndex == 1) display.print(" <");

    display.setCursor(64, 20);
    // display.print("Akt: ");
    display.print(p->active ? "Aktiv" : "Aus");
    if (paramIndex == 3) display.print(" <");

    display.display();
}

void DisplayManager::showInfo(PlantUnit* plants[], int count, float temp, float hum) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    if (count >= 2) {
        display.setCursor(0, 0);
        display.print("F1:"); display.print(plants[0]->getMoisture());
        if (plants[1]->active) { display.print(" F2:"); display.print(plants[1]->getMoisture()); }
    }
    if (count >= 4) {
        display.setCursor(0, 10);
        if (plants[2]->active) { display.print("F3:"); display.print(plants[2]->getMoisture()); }
        if (plants[3]->active) { display.print(" F4:"); display.print(plants[3]->getMoisture()); }
    }
    display.setCursor(0, 20);
    display.print("T:"); display.print(temp, 1); display.print("C ");
    display.print("H:"); display.print(hum, 0); display.print("%");

    display.display();
}

void DisplayManager::nextInfoPage(int plantCount) {
    infoPage = (infoPage >= plantCount) ? 0 : infoPage + 1;
}
