#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include "rgb_lcd.h"

const int relay_pin = 2;
const int pinTemp = A0;
const int chipSelect = 4;
const int THRESH_TEMP = 28;

float temperature;
int B=3975;
float resistance;
rgb_lcd lcd;

void setup() {
    Serial.begin(115200); 

    pinMode(relay_pin, OUTPUT);

    lcd.begin(16, 2);
    lcd.print("Temperature");

    Serial.print("Start write to card.");

    if (!SD.begin(chipSelect)) {
        Serial.println("No card or failure");
        return;
    }

    Serial.println("Card found");
}

void breath(unsigned char color) {

    for(int i=0; i<255; i++)
    {
        lcd.setPWM(color, i);
        delay(5);
    }

    delay(500);
    for(int i=254; i>=0; i--)
    {
        lcd.setPWM(color, i);
        delay(5);
    }

}

void loop() {
    lcd.setCursor(5,1);
    lcd.write(0xdf);
    lcd.setCursor(6,1);
    lcd.print("C");

    if (temperature > THRESH_TEMP){
        Serial.println("TOO HOT");
        digitalWrite(relay_pin, HIGH);
        breath(REG_RED);
    } else {
        Serial.println("COOL ENOUGH");
        digitalWrite(relay_pin, LOW);
        breath(REG_GREEN);
    }

    int val = analogRead(pinTemp);
    resistance=(float)(1023-val)*10000/val;
    temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;
    Serial.println(temperature);
    lcd.setCursor(0,1);
    lcd.print(temperature);

    String dataString = "";


    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    if (dataFile) {
        dataFile.println(temperature);
        dataFile.close();
    } else {
        Serial.println("error opening datalog.txt");
    }
}
