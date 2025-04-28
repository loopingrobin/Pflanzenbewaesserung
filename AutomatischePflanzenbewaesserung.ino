#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Klassen
class MoistureSensor {
public:
    MoistureSensor(uint8_t pin) : _pin(pin) {}
    int read() { return analogRead(_pin); }
private:
    uint8_t _pin;
};

class Pump {
public:
    Pump(uint8_t pin) : _pin(pin), _running(false), _startTime(0), _duration(0) {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, HIGH); // Invertiert
    }
    void start(int durationSeconds) {
        _duration = durationSeconds * 1000;
        _startTime = millis();
        _running = true;
        digitalWrite(_pin, LOW); // Invertiert: Einschalten
    }
    void update() {
        if (_running && millis() - _startTime >= _duration) {
            digitalWrite(_pin, HIGH); // Invertiert: Ausschalten
            _running = false;
        }
    }
    bool isRunning() const {
        return _running;
    }
private:
    uint8_t _pin;
    bool _running;
    unsigned long _startTime;
    unsigned long _duration;
};

class ClimateSensor {
public:
    ClimateSensor(uint8_t pin) : dht(pin, DHT11) {}
    void begin() { dht.begin(); }
    float getTemperature() { return dht.readTemperature(); }
    float getHumidity() { return dht.readHumidity(); }
private:
    DHT dht;
};

class DisplayManager {
public:
    DisplayManager() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}
    void begin() {
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        display.clearDisplay();
    }
    void showMenu(int t1, int d1, int t2, int d2, int index) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.print("T1 "); display.print(t1);
        display.print(" D1:"); display.print(d1);
        display.setCursor(0, 10);
        display.print("T2 "); display.print(t2);
        display.print(" D2:"); display.print(d2);
        display.setCursor(0, 20);
        display.print("Edit: ");
        const char* items[] = {"T1", "D1", "T2", "D2"};
        display.print(items[index]);
        display.display();
    }
    void showInfo(int m1, int m2, float temp, float hum) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.print("F1:"); display.print(m1);
        display.print(" F2:"); display.print(m2);
        display.setCursor(0, 10);
        display.print("T:"); display.print(temp); display.print("C");
        display.setCursor(0, 20);
        display.print("H:"); display.print(hum); display.print("%");
        display.display();
    }
    void clear() {
        display.clearDisplay();
        display.display();
    }
private:
    Adafruit_SSD1306 display;
};

// Pins
const int buttonDisplay = 8;
const int buttonUp = 9;
const int buttonDown = 10;
const int buttonSelect = 11;

// Objekte
MoistureSensor sensor1(A0);
MoistureSensor sensor2(A1);
Pump pump1(5);
Pump pump2(4);
ClimateSensor climate(6);
DisplayManager screen;

// Zustände
int threshold1 = 400, threshold2 = 400;
int pumpDuration1 = 5, pumpDuration2 = 5;
unsigned long lastCheck = 0;
bool displayActive = false, infoScreen = false;
unsigned long displayTimeout = 0;
int menuIndex = 0;

// Entprellung
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

// Test-Modi
enum TestMode {
    TEST_NONE,
    TEST_BUTTONS,
    TEST_MOISTURE,
    TEST_PUMPS,
    TEST_DISPLAY,
    TEST_CLIMATE
};

TestMode currentTest = TEST_NONE; // Hier gewünschten Testmodus setzen

void testButtons() {
    screen.begin();
    screen.clear();
    screen.showMenu(0, 0, 0, 0, 0);
    while (true) {
        screen.clear();
        screen.showMenu(digitalRead(buttonDisplay), digitalRead(buttonUp),
                        digitalRead(buttonDown), digitalRead(buttonSelect), 0);
        delay(300);
    }
}

void testMoisture() {
    screen.begin();
    while (true) {
        int m1 = sensor1.read();
        int m2 = sensor2.read();
        screen.showInfo(m1, m2, 0, 0);
        delay(1000);
    }
}

void testPumps() {
    pump1.start(1);
    delay(2000);
    pump2.start(1);
    delay(5000);
}

void testDisplay() {
    screen.begin();
    screen.showMenu(123, 4, 456, 7, 2);
    delay(2000);
    screen.showInfo(345, 567, 22.3, 58.9);
    delay(3000);
    screen.clear();
}

void testClimate() {
    climate.begin();
    screen.begin();
    while (true) {
        float t = climate.getTemperature();
        float h = climate.getHumidity();
        screen.showInfo(0, 0, t, h);
        delay(2000);
    }
}

void setup() {
    pinMode(buttonDisplay, INPUT_PULLUP);
    pinMode(buttonUp, INPUT_PULLUP);
    pinMode(buttonDown, INPUT_PULLUP);
    pinMode(buttonSelect, INPUT_PULLUP);

    climate.begin();
    screen.begin();

    EEPROM.get(0, threshold1);
    EEPROM.get(4, threshold2);
    EEPROM.get(8, pumpDuration1);
    EEPROM.get(12, pumpDuration2);
}

void loop() {
    // Testmodi
    switch (currentTest) {
        case TEST_BUTTONS: testButtons(); break;
        case TEST_MOISTURE: testMoisture(); break;
        case TEST_PUMPS: testPumps(); break;
        case TEST_DISPLAY: testDisplay(); break;
        case TEST_CLIMATE: testClimate(); break;
        default: break;
    }

    if (millis() - lastCheck > 120000) { // alle 2 Minuten
        lastCheck = millis();
        checkMoistureLevels();
    }

    pump1.update();
    pump2.update();

    handleDisplay();
    handleButtons();
}

void checkMoistureLevels() {
    int m1 = sensor1.read();
    int m2 = sensor2.read();
    if (m1 < threshold1 && !pump1.isRunning()) pump1.start(pumpDuration1);
    if (m2 < threshold2 && !pump2.isRunning()) pump2.start(pumpDuration2);
}

void handleDisplay() {
    if (digitalRead(buttonDisplay) == LOW && millis() - lastButtonPress > debounceDelay) {
        displayActive = true;
        displayTimeout = millis();
        infoScreen = !infoScreen;
        lastButtonPress = millis();
    }
    if (displayActive) {
        if (infoScreen) {
            screen.showInfo(sensor1.read(), sensor2.read(), climate.getTemperature(), climate.getHumidity());
        } else {
            screen.showMenu(threshold1, pumpDuration1, threshold2, pumpDuration2, menuIndex);
        }
        if (millis() - displayTimeout > 10000) {
            displayActive = false;
            screen.clear();
        }
    }
}

void handleButtons() {
    if (millis() - lastButtonPress < debounceDelay) return;

    if (digitalRead(buttonUp) == LOW) {
        switch (menuIndex) {
            case 0: threshold1 += 10; break;
            case 1: pumpDuration1 += 1; break;
            case 2: threshold2 += 10; break;
            case 3: pumpDuration2 += 1; break;
        }
        saveSettings();
        displayTimeout = millis();
        lastButtonPress = millis();
    }
    if (digitalRead(buttonDown) == LOW) {
        switch (menuIndex) {
            case 0: threshold1 -= 10; break;
            case 1: pumpDuration1 -= 1; break;
            case 2: threshold2 -= 10; break;
            case 3: pumpDuration2 -= 1; break;
        }
        saveSettings();
        displayTimeout = millis();
        lastButtonPress = millis();
    }
    if (digitalRead(buttonSelect) == LOW) {
        menuIndex = (menuIndex + 1) % 4;
        displayTimeout = millis();
        lastButtonPress = millis();
    }
}

void saveSettings() {
    EEPROM.put(0, threshold1);
    EEPROM.put(4, threshold2);
    EEPROM.put(8, pumpDuration1);
    EEPROM.put(12, pumpDuration2);
}
