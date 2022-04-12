#pragma once

#include <Arduino.h>

const unsigned long MIN_INTERVAL = 2000;
const unsigned long TIMEDOUT = 0;

class DHT22 {
   private:
    uint8_t m_pin;
    float m_temp;
    float m_humid;
    bool m_last_success;
    unsigned long m_last_read;

    bool handshake();
    bool read();

   public:
    DHT22(uint8_t pin) { m_pin = pin; }

    void begin();
    float temperature();
    float humidity();
};
