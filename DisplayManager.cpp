#include "DisplayManager.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

DisplayManager::DisplayManager() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}

void DisplayManager::begin() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
}

void DisplayManager::showMenu(int t1, int d1, int t2, int d2, int t3, int d3, int index) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("T1:"); display.print(t1); display.print(" D1:"); display.print(d1);
    display.setCursor(0, 10);
    display.print("T2:"); display.print(t2); display.print(" D2:"); display.print(d2);
    display.setCursor(0, 20);
    display.print("T3:"); display.print(t3); display.print(" D3:"); display.print(d3);
    display.setCursor(90, 20);
    const char* items[] = {"T1", "D1", "T2", "D2", "T3", "D3"};
    display.print(items[index]);
    display.display();
}

void DisplayManager::showInfo(int m1, int m2, int m3, float temp, float hum) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("F1:"); display.print(m1); display.print(" F2:"); display.print(m2);
    display.setCursor(0, 10);
    display.print("F3:"); display.print(m3);
    display.setCursor(64, 10);
    display.print("T:"); display.print(temp, 1); display.print("C");
    display.setCursor(0, 20);
    display.print("H:"); display.print(hum, 1); display.print("%");
    display.display();
}

void DisplayManager::clear() {
    display.clearDisplay();
    display.display();
}
