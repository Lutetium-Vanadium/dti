#include "DHT22.h";

void DHT22::begin() {
    pinMode(m_pin, INPUT_PULLUP);
    m_last_read = millis() - MIN_INTERVAL;
    m_last_success = true;
}

bool DHT22::handshake() {
    pinMode(m_pin, INPUT_PULLUP);
    delay(1);

    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, LOW);
    pinMode(m_pin, INPUT);
    int old_read = digitalRead(m_pin);
    pinMode(m_pin, OUTPUT);

    delayMicroseconds(1100);
    digitalWrite(m_pin, HIGH);

    pinMode(m_pin, INPUT_PULLUP);

    int old_read2 = digitalRead(m_pin);

    // Should be within 40 us
    if (pulseIn(m_pin, LOW, 45) == TIMEDOUT) {
        Serial.print("DHT no go low; ded; OLD: ");
        Serial.print(old_read);
        Serial.print(";  OLD2: ");
        Serial.print(old_read2);
        Serial.print(";  NEW: ");
        Serial.println(digitalRead(m_pin));
        return false;
    }

    // Should be within 80 us
    if (pulseIn(m_pin, HIGH, 85) == TIMEDOUT) {
        Serial.println("DHT no go high; ded");
        return false;
    }

    // Should be within 80 us
    if (pulseIn(m_pin, LOW, 85) == TIMEDOUT) {
        Serial.println("DHT no go low again; ded");
        return false;
    }
}

bool DHT22::read() {
    unsigned long cur_time = millis();

    if (cur_time > m_last_read && cur_time - m_last_read <= MIN_INTERVAL) {
        return m_last_success;
    }

    m_last_read = cur_time;

    // nointerrupts();

    if (!handshake()) {
        Serial.println("Handshake failed");
        return false;
    }

    uint8_t data[5] = {0, 0, 0, 0, 0};

    for (int i = 0; i < 40; ++i) {
        // Should be only 50us
        if (pulseIn(m_pin, HIGH, 55) == TIMEDOUT) {
            Serial.print("DHT did not wait 50 us");
            return false;
        }

        // Shouldn't exceed 70us
        unsigned long pull_low_time = pulseIn(m_pin, LOW, 75);

        if (pull_low_time == TIMEDOUT) {
            Serial.print("DHT did not pull low");
            return false;
        }

        // DHT has a variable length pulse to denote the bit. 26-28us denotes
        // 0, and ~70us denotes 1.
        if (pull_low_time <= 30) {
            data[i / 8] &= ~(1 << (i % 8));
        } else {
            data[i / 8] |= 1 << (i % 8);
        }
    }

    // interrupts();

    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        Serial.print("DHT checksum failed");
        return false;
    }

    m_humid = ((float)data[0] + (float)data[1]) / 10;
    m_temp = ((float)data[2] + (float)data[3]) / 10;

    return true;
}

float DHT22::temperature() {
    if (!read()) {
        m_last_success = false;
        Serial.println("DHT failed to read");
        return -1;
    }

    return m_temp;
}

float DHT22::humidity() {
    if (!read()) {
        m_last_success = false;
        Serial.println("DHT failed to read");
        return -1;
    }

    return m_humid;
}
