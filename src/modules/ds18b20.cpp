#include "modules/ds18b20.h"

namespace modules {
    DS18B20::DS18B20(std::string name, int pin) :
        modules::Module(name) {
        this->pin = pin;
    }
    DS18B20::DS18B20(std::string name, unsigned long update_frequency, int pin) :
        modules::Module(name, update_frequency) {
        this->pin = pin;
    }
    DS18B20::~DS18B20() {
        delete this->oneWire;
        delete this->sensor;
    }

    void DS18B20::init() {
        this->oneWire = new OneWire(this->pin);
        this->sensor  = new DallasTemperature(this->oneWire);
        this->sensor->begin();
    }

    void DS18B20::loop() {
        this->trigger();
        this->measure();
#ifdef USE_MQTT
        Temperature::mqtt_data->update(this->temperature, true);
#endif
    }

    void DS18B20::userLoop() {
    }

    void DS18B20::trigger() {
        this->sensor->requestTemperatures();
    }
    void DS18B20::measure() {
        float tempC       = sensor->getTempCByIndex(0); // Read first sensor
        this->temperature = (double) tempC;
    }
} // namespace modules