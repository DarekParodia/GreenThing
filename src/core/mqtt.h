#pragma once

#include "core/core.h"
#include "core/wifi.h"

#if defined(ESP32)
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

#include <ArduinoHA.h>
#include <WiFiManager.h>
#include <cmath>
#include <string>
#include <vector>

namespace core::mqtt {
    struct EntityMeta {
            std::string name;
            std::string unit_of_measurement;
            std::string device_class;
            std::string icon;
            std::string state_class;
    };

    struct NumberConfig {
            bool           has_min    = false;
            bool           has_max    = false;
            bool           has_step   = false;
            float          min        = 0.0f;
            float          max        = 0.0f;
            float          step       = 1.0f;
            HANumber::Mode mode       = HANumber::ModeAuto;
            bool           optimistic = false;
            bool           retain     = false;
    };

    class EntityBase {
        public:
            virtual ~EntityBase() = default;
            virtual void init()   = 0;
            virtual void update() = 0;
    };

    void registerEntity(EntityBase *entity);

    class SensorNumber : public EntityBase {
        public:
            SensorNumber(std::string id, double *update_data, unsigned long update_interval, EntityMeta meta = {}, HABaseDeviceType::NumberPrecision precision = HABaseDeviceType::PrecisionP1);
            SensorNumber(std::string id, unsigned long update_interval, EntityMeta meta = {}, HABaseDeviceType::NumberPrecision precision = HABaseDeviceType::PrecisionP1);

            void update(double value, bool force = false);
            void update() override;
            void init() override;

        private:
            void                              applyMeta();
            void                              publishValue(double value, bool force);
            bool                              shouldPublish(double value);

            std::string                       id;
            EntityMeta                        meta;
            HASensorNumber                   *sensor          = nullptr;
            double                            data            = 0.0;
            double                           *update_data     = nullptr;
            unsigned long                     update_interval = 0;
            unsigned long                     last_update     = 0;
            bool                              initialized     = false;
            HABaseDeviceType::NumberPrecision precision       = HABaseDeviceType::PrecisionP1;
    };

    class SensorText : public EntityBase {
        public:
            SensorText(std::string id, EntityMeta meta = {});

            void update(const std::string &value, bool force = false);
            void update() override;
            void init() override;

        private:
            void        applyMeta();
            void        publishValue(const std::string &value, bool force);
            bool        shouldPublish(const std::string &value);

            std::string id;
            EntityMeta  meta;
            HASensor   *sensor = nullptr;
            std::string data;
            bool        initialized = false;
    };

    class NumberInput : public EntityBase {
        public:
            NumberInput(std::string id, double *value, EntityMeta meta = {}, NumberConfig config = {}, HABaseDeviceType::NumberPrecision precision = HABaseDeviceType::PrecisionP1);

            void update(double value, bool force = false);
            void update() override;
            void init() override;

        private:
            static void                       handleCommand(HANumeric number, HANumber *sender);
            static NumberInput               *findInstance(HANumber *sender);
            static void                       registerInstance(NumberInput *instance);

            void                              applyMeta();
            void                              applyConfig();

            std::string                       id;
            EntityMeta                        meta;
            NumberConfig                      config;
            HANumber                         *number        = nullptr;
            double                            current_value = 0.0;
            double                           *value_ptr     = nullptr;
            HABaseDeviceType::NumberPrecision precision     = HABaseDeviceType::PrecisionP1;
    };

    class Switch : public EntityBase {
        public:
            Switch(std::string id, bool *state, EntityMeta meta = {});

            void setState(bool state, bool force = false);
            void update() override;
            void init() override;

        private:
            static void    handleCommand(bool state, HASwitch *sender);
            static Switch *findInstance(HASwitch *sender);
            static void    registerInstance(Switch *instance);

            void           applyMeta();

            std::string    id;
            EntityMeta     meta;
            HASwitch      *sw            = nullptr;
            bool           current_state = false;
            bool          *state_ptr     = nullptr;
    };

    class BinarySensor : public EntityBase {
        public:
            BinarySensor(std::string id, bool *state, EntityMeta meta = {});

            void setState(bool state, bool force = false);
            void update() override;
            void init() override;

        private:
            void            applyMeta();

            std::string     id;
            EntityMeta      meta;
            HABinarySensor *sensor        = nullptr;
            bool            current_state = false;
            bool           *state_ptr     = nullptr;
    };

    bool isConnected();
    bool publish(const char *topic, const char *payload, bool retained = false);
    bool subscribe(const char *topic);

    void preInit();
    void init();
    void postInit();
    void loop();
} // namespace core::mqtt