#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <math.h>

#include "DHT22.h"
#include "rgb_lcd.h"

DHT22 dht(8);
rgb_lcd lcd;

const int chipSelect = 4;

float humidity;
float temperature;

void setup() {
    Serial.begin(115200);

    lcd.begin(16, 2);

    lcd.print("Temp | Humidity");

    dht.begin();

    Serial.print("Start write to card.");

    if (!SD.begin(chipSelect)) {
        Serial.println("No card or failure");

        return;
    }
    Serial.println("Card found");
}

void breath(unsigned char color) {
    for (int i = 0; i < 255; i++) {
        lcd.setPWM(color, i);
        delay(5);
    }

    delay(500);
    for (int i = 254; i >= 0; i--) {
        lcd.setPWM(color, i);
        delay(5);
    }
}

void loop() {
    if (temperature > 30) {
        breath(REG_RED);
    } else if (temperature < 25) {
        breath(REG_GREEN);
    } else {
        breath(REG_BLUE);
    }

    Serial.println("Checking temperature");
    temperature = dht.temperature();
    Serial.println(temperature);
    Serial.println("Checking humidity");
    humidity = dht.humidity();
    Serial.println(humidity);

    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.print("C |");
    lcd.print(humidity);

    String dataString = "";

    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    if (dataFile) {
        dataFile.println(temperature);
        dataFile.close();
    } else {
        Serial.println("error opening datalog.txt");
    }
}

