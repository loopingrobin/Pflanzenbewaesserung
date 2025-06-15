#include "DisplayManager.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

DisplayManager::DisplayManager() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}

void DisplayManager::begin() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
}

void DisplayManager::showMenu(PlantUnit* plants[], int count, int index) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    for (int i = 0; i < count; i++) {
        display.setCursor(0, i * 10);
        display.print("T"); display.print(i+1); display.print(":");
        display.print(plants[i]->threshold);
        display.print(" D"); display.print(i+1); display.print(":");
        display.print(plants[i]->duration);
        display.print(plants[i]->active ? " A" : " I");
    }
    display.setCursor(90, 20);
    display.print("E:"); display.print(index);
    display.display();
}

void DisplayManager::showInfo(PlantUnit* plants[], int count, float temp, float hum) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // DHT11 Werte immer oben
    display.setCursor(0,0);
    display.print(F("Temp: "));
    display.print(temp);
    display.print(F(" C"));
    display.setCursor(0,10);
    display.print(F("Humidity: "));
    display.print(hum);
    display.print(F(" %"));

    // 2 Pflanzen pro Seite anzeigen
    int start = infoPage * 2;
    int end = start + 2;
    int y = 20;

    for (int i = start; i < end && i < count; i++) {
      display.setCursor(0, y);
      display.print(F("Plant "));
      display.print(i+1);
      display.print(F(": "));
      display.print(plants[i]->getMoisture());
      y += 10;
    }

    display.display();
}

void DisplayManager::nextInfoPage(int plantCount) {
    int maxPage = (plantCount + 1) / 2 - 1; // 2 Pflanzen pro Seite
    infoPage = (infoPage >= maxPage) ? 0 : infoPage + 1;
}

void DisplayManager::clear() {
    display.clearDisplay();
    display.display();
}
