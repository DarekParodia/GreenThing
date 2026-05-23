#include "core/mqtt.h"

#include "core/filesystem.h"

namespace core::mqtt {
    struct mqtt_credentials {
            char     server[40] = MQTT_SERV;
            uint16_t port       = MQTT_PORT;
            char     user[40]   = MQTT_USER;
            char     pass[40]   = MQTT_PASS;
    };

    static size_t                     credentials_size = sizeof(mqtt_credentials);
    static mqtt_credentials           credentials;

    static WiFiClient                 netClient;
    static HADevice                   device;
    static HAMqtt                     mqtt(netClient, device);

    static WiFiManagerParameter      *custom_mqtt_server;
    static WiFiManagerParameter      *custom_mqtt_port;
    static WiFiManagerParameter      *custom_mqtt_user;
    static WiFiManagerParameter      *custom_mqtt_pass;

    static std::string                device_name;
    static std::string                device_config_url;

    static std::vector<EntityBase *> &entityList() {
        static std::vector<EntityBase *> entities;
        return entities;
    }

    void registerEntity(EntityBase *entity) {
        entityList().push_back(entity);
    }

    static std::vector<Switch *> &switchInstances() {
        static std::vector<Switch *> instances;
        return instances;
    }

    static std::vector<NumberInput *> &numberInstances() {
        static std::vector<NumberInput *> instances;
        return instances;
    }

    bool isConnected() {
        return mqtt.isConnected();
    }

    bool publish(const char *topic, const char *payload, bool retained) {
        return mqtt.publish(topic, payload, retained);
    }

    bool subscribe(const char *topic) {
        return mqtt.subscribe(topic);
    }

    void preInit() {
        Serial.println("Adding mqtt parameters to wifi manager");

        core::filesystem::readFile("mqtt.bin", &credentials, &credentials_size);

        char port_str[6];
        snprintf(port_str, sizeof(port_str), "%u", credentials.port);

        custom_mqtt_server = new WiFiManagerParameter("server", "MQTT Server", credentials.server, 40);
        custom_mqtt_port   = new WiFiManagerParameter("port", "MQTT Port", port_str, 6);
        custom_mqtt_user   = new WiFiManagerParameter("user", "MQTT User", credentials.user, 40);
        custom_mqtt_pass   = new WiFiManagerParameter("pass", "MQTT Password", credentials.pass, 40);

        core::wifi::addCustomParameter(custom_mqtt_server);
        core::wifi::addCustomParameter(custom_mqtt_port);
        core::wifi::addCustomParameter(custom_mqtt_user);
        core::wifi::addCustomParameter(custom_mqtt_pass);
    }

    void init() {}

    void postInit() {
        strncpy(credentials.server, custom_mqtt_server->getValue(), sizeof(credentials.server));
        credentials.port = static_cast<uint16_t>(atoi(custom_mqtt_port->getValue()));
        strncpy(credentials.user, custom_mqtt_user->getValue(), sizeof(credentials.user));
        strncpy(credentials.pass, custom_mqtt_pass->getValue(), sizeof(credentials.pass));

        core::filesystem::writeFile("mqtt.bin", &credentials, &credentials_size);

        byte mac[WL_MAC_ADDR_LENGTH];
        WiFi.macAddress(mac);
        device.setUniqueId(mac, sizeof(mac));
        device.enableExtendedUniqueIds();
        device.enableSharedAvailability();
        device.enableLastWill();

        device_name = core::getHostname();
        device.setName(device_name.c_str());
        device.setManufacturer(HOSTNAME);
#if defined(ESP32)
        device.setModel("ESP32");
#elif defined(ESP8266)
        device.setModel("ESP8266");
#else
        device.setModel("Arduino");
#endif
        device.setSoftwareVersion(__DATE__ " " __TIME__);

        device_config_url = core::wifi::getStringIP();
        if(!device_config_url.empty()) {
            device_config_url = "http://" + device_config_url;
            device.setConfigurationUrl(device_config_url.c_str());
        }

        mqtt.setBufferSize(512);

        for(auto *entity : entityList())
            entity->init();

        mqtt.setDiscoveryPrefix("homeassistant");

        if(strlen(credentials.user) == 0)
            mqtt.begin(credentials.server, credentials.port);
        else
            mqtt.begin(credentials.server, credentials.port, credentials.user, credentials.pass);

        device.setAvailability(true);
    }

    void loop() {
        mqtt.loop();
    }

    SensorNumber::SensorNumber(std::string id, double *update_data, unsigned long update_interval, EntityMeta meta, HABaseDeviceType::NumberPrecision precision) :
        id(std::move(id)),
        meta(std::move(meta)),
        update_data(update_data),
        update_interval(update_interval),
        precision(precision) {
        registerEntity(this);
    }

    SensorNumber::SensorNumber(std::string id, unsigned long update_interval, EntityMeta meta, HABaseDeviceType::NumberPrecision precision) :
        SensorNumber(std::move(id), nullptr, update_interval, std::move(meta), precision) {}

    void SensorNumber::init() {
        if(sensor != nullptr)
            return;

        sensor = new HASensorNumber(id.c_str(), precision);
        applyMeta();

        if(update_data != nullptr) {
            data        = *update_data;
            initialized = true;
            sensor->setCurrentValue(static_cast<float>(data));
        }
    }

    void SensorNumber::update() {
        if(update_data == nullptr)
            return;
        update(*update_data);
    }

    void SensorNumber::update(double value, bool force) {
        if(force) {
            data        = value;
            initialized = true;
            last_update = millis();
            publishValue(value, true);
            return;
        }

        if(shouldPublish(value))
            publishValue(value, false);
    }

    void SensorNumber::applyMeta() {
        if(!meta.name.empty())
            sensor->setName(meta.name.c_str());
        if(!meta.unit_of_measurement.empty())
            sensor->setUnitOfMeasurement(meta.unit_of_measurement.c_str());
        if(!meta.device_class.empty())
            sensor->setDeviceClass(meta.device_class.c_str());
        if(!meta.icon.empty())
            sensor->setIcon(meta.icon.c_str());
        if(!meta.state_class.empty())
            sensor->setStateClass(meta.state_class.c_str());
    }

    void SensorNumber::publishValue(double value, bool force) {
        if(sensor == nullptr)
            return;

        if(isConnected())
            sensor->setValue(static_cast<float>(value), force);
        else
            sensor->setCurrentValue(static_cast<float>(value));
    }

    bool SensorNumber::shouldPublish(double value) {
        static constexpr double EPSILON = 0.0001;

        if(!initialized) {
            data        = value;
            initialized = true;
            last_update = millis();
            return true;
        }

        if(fabs(value - data) <= EPSILON)
            return false;

        if(update_interval == 0) {
            data = value;
            return true;
        }

        unsigned long now = millis();
        if(now - last_update >= update_interval) {
            data        = value;
            last_update = now;
            return true;
        }

        return false;
    }

    SensorText::SensorText(std::string id, EntityMeta meta) :
        id(std::move(id)),
        meta(std::move(meta)) {
        registerEntity(this);
    }

    void SensorText::init() {
        if(sensor != nullptr)
            return;

        sensor = new HASensor(id.c_str());
        applyMeta();

        if(initialized && !data.empty())
            publishValue(data, true);
    }

    void SensorText::update() {
        if(initialized)
            publishValue(data, true);
    }

    void SensorText::update(const std::string &value, bool force) {
        if(force) {
            data        = value;
            initialized = true;
            publishValue(value, true);
            return;
        }

        if(shouldPublish(value))
            publishValue(value, false);
    }

    void SensorText::applyMeta() {
        if(!meta.name.empty())
            sensor->setName(meta.name.c_str());
        if(!meta.unit_of_measurement.empty())
            sensor->setUnitOfMeasurement(meta.unit_of_measurement.c_str());
        if(!meta.device_class.empty())
            sensor->setDeviceClass(meta.device_class.c_str());
        if(!meta.icon.empty())
            sensor->setIcon(meta.icon.c_str());
    }

    void SensorText::publishValue(const std::string &value, bool force) {
        if(sensor == nullptr)
            return;

        if(force || isConnected())
            sensor->setValue(value.c_str());
    }

    bool SensorText::shouldPublish(const std::string &value) {
        if(!initialized) {
            data        = value;
            initialized = true;
            return true;
        }

        if(value != data) {
            data = value;
            return true;
        }

        return false;
    }

    NumberInput::NumberInput(std::string id, double *value, EntityMeta meta, NumberConfig config, HABaseDeviceType::NumberPrecision precision) :
        id(std::move(id)),
        meta(std::move(meta)),
        config(config),
        value_ptr(value),
        precision(precision) {
        registerEntity(this);
    }

    void NumberInput::init() {
        if(number != nullptr)
            return;

        number = new HANumber(id.c_str(), precision);
        applyMeta();
        applyConfig();
        registerInstance(this);
        number->onCommand(handleCommand);

        if(value_ptr != nullptr) {
            current_value = *value_ptr;
            number->setCurrentState(static_cast<float>(current_value));
        }
    }

    void NumberInput::update() {
        if(value_ptr == nullptr)
            return;
        update(*value_ptr);
    }

    void NumberInput::update(double value, bool force) {
        current_value = value;
        if(value_ptr != nullptr)
            *value_ptr = value;

        if(number == nullptr)
            return;

        if(isConnected())
            number->setState(static_cast<float>(value), force);
        else
            number->setCurrentState(static_cast<float>(value));
    }

    void NumberInput::applyMeta() {
        if(!meta.name.empty())
            number->setName(meta.name.c_str());
        if(!meta.unit_of_measurement.empty())
            number->setUnitOfMeasurement(meta.unit_of_measurement.c_str());
        if(!meta.device_class.empty())
            number->setDeviceClass(meta.device_class.c_str());
        if(!meta.icon.empty())
            number->setIcon(meta.icon.c_str());
    }

    void NumberInput::applyConfig() {
        number->setMode(config.mode);
        number->setOptimistic(config.optimistic);
        number->setRetain(config.retain);

        if(config.has_min)
            number->setMin(config.min);
        if(config.has_max)
            number->setMax(config.max);
        if(config.has_step)
            number->setStep(config.step);
    }

    void NumberInput::handleCommand(HANumeric numberValue, HANumber *sender) {
        NumberInput *instance = findInstance(sender);
        if(instance == nullptr)
            return;

        if(!numberValue.isSet())
            return;

        double value            = numberValue.toFloat();
        instance->current_value = value;
        if(instance->value_ptr != nullptr)
            *instance->value_ptr = value;

        sender->setState(numberValue);
    }

    NumberInput *NumberInput::findInstance(HANumber *sender) {
        for(auto *instance : numberInstances())
            if(instance->number == sender)
                return instance;

        return nullptr;
    }

    void NumberInput::registerInstance(NumberInput *instance) {
        numberInstances().push_back(instance);
    }

    Switch::Switch(std::string id, bool *state, EntityMeta meta) :
        id(std::move(id)),
        meta(std::move(meta)),
        state_ptr(state) {
        registerEntity(this);
    }

    void Switch::init() {
        if(sw != nullptr)
            return;

        sw = new HASwitch(id.c_str());
        applyMeta();
        registerInstance(this);
        sw->onCommand(handleCommand);

        if(state_ptr != nullptr)
            current_state = *state_ptr;
        sw->setCurrentState(current_state);
    }

    void Switch::update() {
        if(state_ptr == nullptr)
            return;
        setState(*state_ptr);
    }

    void Switch::setState(bool state, bool force) {
        current_state = state;
        if(state_ptr != nullptr)
            *state_ptr = state;

        if(sw == nullptr)
            return;

        if(isConnected())
            sw->setState(state, force);
        else
            sw->setCurrentState(state);
    }

    void Switch::applyMeta() {
        if(!meta.name.empty())
            sw->setName(meta.name.c_str());
        if(!meta.device_class.empty())
            sw->setDeviceClass(meta.device_class.c_str());
        if(!meta.icon.empty())
            sw->setIcon(meta.icon.c_str());
    }

    void Switch::handleCommand(bool state, HASwitch *sender) {
        Switch *instance = findInstance(sender);
        if(instance == nullptr)
            return;

        instance->current_state = state;
        if(instance->state_ptr != nullptr)
            *instance->state_ptr = state;

        sender->setState(state);
    }

    Switch *Switch::findInstance(HASwitch *sender) {
        for(auto *instance : switchInstances())
            if(instance->sw == sender)
                return instance;

        return nullptr;
    }

    void Switch::registerInstance(Switch *instance) {
        switchInstances().push_back(instance);
    }

    BinarySensor::BinarySensor(std::string id, bool *state, EntityMeta meta) :
        id(std::move(id)),
        meta(std::move(meta)),
        state_ptr(state) {
        registerEntity(this);
    }

    void BinarySensor::init() {
        if(sensor != nullptr)
            return;

        sensor = new HABinarySensor(id.c_str());
        applyMeta();

        if(state_ptr != nullptr)
            current_state = *state_ptr;
        sensor->setCurrentState(current_state);
    }

    void BinarySensor::update() {
        if(state_ptr == nullptr)
            return;
        setState(*state_ptr);
    }

    void BinarySensor::setState(bool state, bool force) {
        current_state = state;
        if(state_ptr != nullptr)
            *state_ptr = state;

        if(sensor == nullptr)
            return;

        if(isConnected())
            sensor->setState(state, force);
        else
            sensor->setCurrentState(state);
    }

    void BinarySensor::applyMeta() {
        if(!meta.name.empty())
            sensor->setName(meta.name.c_str());
        if(!meta.device_class.empty())
            sensor->setDeviceClass(meta.device_class.c_str());
        if(!meta.icon.empty())
            sensor->setIcon(meta.icon.c_str());
    }
} // namespace core::mqtt
