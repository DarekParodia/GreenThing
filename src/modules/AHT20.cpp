#include "AHT20.h"

#include <Arduino.h>
#include <Wire.h>
#include <cmath>

namespace modules {
    AHT20::AHT20(std::string name) :
        modules::Module(name),
        modules::Humidity(name, -1, false, 0, 100) // -1 pin, not used
    {}
    AHT20::AHT20(std::string name, unsigned long update_frequency) :
        modules::Module(name),
        modules::Humidity(name, -1, false, 0, 100) // -1 pin, not used
    {}

    AHT20::~AHT20() {}

    void AHT20::init() {
        this->failed = false;
        // Check if AHT20 responds on I2C address 0x38
        Wire.beginTransmission(0x38);
        uint8_t error = Wire.endTransmission();
        if(error != 0)
            this->failed = true;
    }

    void AHT20::loop() {
        // Wire.setClock(400000);
        if(failed) return;
        // this->measure();
        // this->trigger();

        // Wire.setClock(I2C_SPEED);
    }

    void AHT20::userLoop() {
        if(failed) return;
        this->measure();
        this->trigger();
#ifdef USE_MQTT
        Temperature::mqtt_data->update(this->temperature, true);
        Humidity::mqtt_data->update(this->humidity, true);
#endif
    }
    void AHT20::trigger() {
        // Send measurement command to AHT20 (0xAC, 0x33, 0x00)
        Wire.beginTransmission(0x38);
        Wire.write(0xAC);
        Wire.write(0x33);
        Wire.write(0x00);
        Wire.endTransmission();
    }

    void AHT20::measure() {
        const uint8_t address = 0x38;
        uint8_t       data[6] = { 0 };
        Wire.requestFrom(address, 6u);
        if(Wire.available() == 6) {
            for(int i = 0; i < 6; ++i)
                data[i] = Wire.read();
            // Only parse if status bit 7 == 0 (data ready)
            if((data[0] & 0x80) == 0) {
                // Parse humidity (20 bits)
                uint32_t hum_raw  = ((uint32_t) data[1] << 12) | ((uint32_t) data[2] << 4) | (data[3] >> 4);
                // Parse temperature (20 bits)
                uint32_t temp_raw = (((uint32_t) data[3] & 0x0F) << 16) | ((uint32_t) data[4] << 8) | data[5];
                // Convert to physical values
                float humidity    = (hum_raw * 100.0f) / 1048576.0f;
                float temperature = (temp_raw * 200.0f) / 1048576.0f - 50.0f;
                // Clamp values to valid range
                if(humidity < 0) return;
                if(humidity > 100) return;
                if(temperature < -40) return;
                if(temperature > 85) return;
                this->humidity    = humidity;
                this->temperature = temperature;
            }
        }
        // If data is not ready, just return without updating values
    }
} // namespace modules