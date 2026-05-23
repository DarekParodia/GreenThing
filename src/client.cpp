#include "client.h"

#include "I2C_LCD.h"
#include "core/core.h"
#include "core/display/display.h"
#include "core/time.h"
#include "core/wifi.h"
#include "modules/AHT20.h"
#include "modules/base/humidity.h"
#include "modules/base/ultrasonic.h"
#include "modules/button.h"
#include "modules/flow_meter.h"
#include "modules/led.h"
#include "modules/rcwl_1x05.h"
#include "modules/solenoid.h"

#include <Arduino.h>
#include <Wire.h>
#include <string>
#include <time.h>

#define disp core::display::displayInterface

modules::Button    button("button1", 0, false, true);
modules::Solenoid  solenoid("solenoid1", 16, 14, false, 250);
modules::FlowMeter flow_meter("flow_meter1", 13);
// modules::Humidity   humidity("humidity1", A0, true, 0, 1024);
modules::RCWL_1x05 *ultrasonic = new modules::RCWL_1x05("rcwl1x05");
modules::AHT20      aht20("AHT20");

//
byte char_litersPerMinute[] = {
    B01000,
    B01000,
    B01000,
    B01110,
    B00000,
    B01010,
    B10101,
    B10001
};

bool         prevButton                             = false;
int          currentVolMeasurment                   = -1;
double       currentVolume                          = 0.0;

const double containerVolume                        = 1000.0; // Liters
const double containerHeight                        = 100.0;  // Centimeter

// SAFETY
double                    maxLiters                 = 10.0;
const double              secondsAfterCloseCheck    = 15.0;
const double              secondsPerCloseTry        = 7.5;
const double              flowTreshold              = 5.0;
bool                      _alarm                    = false;

unsigned long             lastValveClose            = 0;
unsigned long             lastValveCloseTry         = 0;
double                    closingFlow               = 0;

bool                      prevSolenoid              = false;
bool                      solenoid_desired          = false;
bool                      solenoid_last_desired     = false;
bool                      solenoid_last_actual      = false;

double                    tank_level_percent        = 0.0;
double                    tank_level_liters         = 0.0;
double                    daily_water_used          = 0.0;
double                    uptime_seconds            = 0.0;
double                    wifi_rssi_dbm             = 0.0;
time_t                    last_watering_start_epoch = 0;
time_t                    last_watering_end_epoch   = 0;
double                    seconds_since_watering    = 0.0;

core::mqtt::EntityMeta    volume_alarm_meta         = { "Max Water Per Cycle", "L", "" };
core::mqtt::EntityMeta    volume_alarm_water_meta   = { "Water Used This Cycle", "L", "water" };
core::mqtt::NumberConfig  volume_alarm_config       = { true, true, true, 1.0f, 20.0f, 0.1f, HANumber::ModeSlider, false, false };

core::mqtt::NumberInput  *mqtt_volume_alarm         = new core::mqtt::NumberInput("volume_alarm_value", &maxLiters, volume_alarm_meta, volume_alarm_config);
core::mqtt::SensorNumber *mqtt_volume_alarm_water   = new core::mqtt::SensorNumber("volume_alarm_water_volume", 1000, volume_alarm_water_meta);

core::mqtt::EntityMeta    solenoid_meta             = { "Watering Valve", "", "switch", "mdi:valve" };
core::mqtt::Switch       *mqtt_solenoid_switch      = new core::mqtt::Switch("watering_valve", &solenoid_desired, solenoid_meta);

core::mqtt::EntityMeta    tank_level_pct_meta       = { "Tank Level", "%", "", "mdi:water-percent" };
core::mqtt::EntityMeta    tank_level_l_meta         = { "Tank Level", "L", "water", "mdi:water" };
core::mqtt::SensorNumber *mqtt_tank_level_pct       = new core::mqtt::SensorNumber("tank_level_percent", 5000, tank_level_pct_meta);
core::mqtt::SensorNumber *mqtt_tank_level_liters    = new core::mqtt::SensorNumber("tank_level_liters", 5000, tank_level_l_meta);

core::mqtt::EntityMeta    daily_water_meta          = { "Water Used Today", "L", "water", "mdi:water" };
core::mqtt::SensorNumber *mqtt_daily_water          = new core::mqtt::SensorNumber("water_used_today", 10000, daily_water_meta);

core::mqtt::EntityMeta    flow_alarm_meta           = { "Flow Alarm", "", "problem", "mdi:alert" };
core::mqtt::BinarySensor *mqtt_flow_alarm           = new core::mqtt::BinarySensor("flow_alarm", &_alarm, flow_alarm_meta);

core::mqtt::EntityMeta    wifi_rssi_meta            = { "WiFi Signal", "dBm", "signal_strength", "mdi:wifi" };
core::mqtt::SensorNumber *mqtt_wifi_rssi            = new core::mqtt::SensorNumber("wifi_rssi", 10000, wifi_rssi_meta);

core::mqtt::EntityMeta    uptime_meta               = { "Power On Duration", "s", "", "mdi:timer" };
core::mqtt::SensorNumber *mqtt_uptime               = new core::mqtt::SensorNumber("uptime_seconds", 10000, uptime_meta);

core::mqtt::EntityMeta    last_start_meta           = { "Last Watering Start", "", "", "mdi:clock-start" };
core::mqtt::EntityMeta    last_end_meta             = { "Last Watering End", "", "", "mdi:clock-end" };
core::mqtt::EntityMeta    last_elapsed_meta         = { "Seconds Since Last Watering", "s", "", "mdi:clock-outline" };
core::mqtt::SensorText   *mqtt_last_start           = new core::mqtt::SensorText("last_watering_start", last_start_meta);
core::mqtt::SensorText   *mqtt_last_end             = new core::mqtt::SensorText("last_watering_end", last_end_meta);
core::mqtt::SensorNumber *mqtt_last_elapsed         = new core::mqtt::SensorNumber("seconds_since_last_watering", 10000, last_elapsed_meta);

// s
void alarm() {
    _alarm = true;
}

void dealarm() {
    _alarm = false;
}

void alarmCheck() {
    mqtt_volume_alarm_water->update(currentVolume);

    // Solenoid not closed
    if(millis() >= lastValveClose + (secondsAfterCloseCheck * 1000.0) &&
       !solenoid.isOpen() && flow_meter.getFlowRate() - closingFlow > flowTreshold)
        alarm();
    else if(currentVolume >= maxLiters && solenoid.isOpen())
        solenoid.close();
    else
        dealarm();
}

double getUsPercentage() {
    return (containerHeight - ultrasonic->getDistance()) / containerHeight;
}

double getUsVolume() {
    return containerVolume * getUsPercentage();
}
// For my personal use case. It calculates water level percentage based on distance to water surface from the top of container

static std::string formatEpoch(time_t epoch) {
    if(epoch < 100000)
        return "unknown";
    struct tm timeinfo;
    if(!localtime_r(&epoch, &timeinfo))
        return "unknown";
    char buf[32] = { 0 };
    if(strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo) == 0)
        return "unknown";
    return std::string(buf);
}

void wateringCycleOn() {
    currentVolume = 0.0;
    solenoid.open();
    last_watering_start_epoch = time(nullptr);
    mqtt_last_start->update(formatEpoch(last_watering_start_epoch), true);
}

void wateringCycleOff() {
    closingFlow = flow_meter.getFlowRate();
    solenoid.close();
    button.setState(false);
    lastValveClose          = millis();
    last_watering_end_epoch = time(nullptr);
    mqtt_last_end->update(formatEpoch(last_watering_end_epoch), true);
}
core::timeout_t *timeout = nullptr;

namespace client {
    void preInit() {
        core::addModule(&button);
        core::addModule(&solenoid);
        core::addModule(&flow_meter);
        // core::addModule(&humidity);
        core::addModule(ultrasonic);
        core::addModule(&aht20);

        currentVolMeasurment = flow_meter.startVolumeMeasurment(&currentVolume);
        delay(200);
        wateringCycleOff();
    }
    void init() {
    }
    void postInit() {
        if(core::display::displayInterface != nullptr)
            disp->clear();
        // disp->setBacklight(false);
    }

    void loop() {
        if(solenoid_desired != solenoid_last_desired) {
            if(solenoid_desired)
                wateringCycleOn();
            else
                wateringCycleOff();
            solenoid_last_desired = solenoid_desired;
        }

        if(button.isPressed() != prevButton) {
            prevButton = button.isPressed();

            if(button.isPressed())
                wateringCycleOn();
            else
                wateringCycleOff();
        }

        ultrasonic->setTemperature(aht20.getTemperature());
        alarmCheck();

        tank_level_percent = getUsPercentage() * 100.0;
        tank_level_liters  = getUsVolume();
        daily_water_used   = flow_meter.getVolume24H();
        uptime_seconds     = millis() / 1000.0;
        wifi_rssi_dbm      = core::wifi::isConnected() ? WiFi.RSSI() : 0.0;
        if(last_watering_end_epoch >= 100000) {
            time_t now = time(nullptr);
            if(now >= last_watering_end_epoch)
                seconds_since_watering = difftime(now, last_watering_end_epoch);
        }

        mqtt_tank_level_pct->update(tank_level_percent);
        mqtt_tank_level_liters->update(tank_level_liters);
        mqtt_daily_water->update(daily_water_used);
        mqtt_wifi_rssi->update(wifi_rssi_dbm);
        mqtt_uptime->update(uptime_seconds);
        mqtt_last_elapsed->update(seconds_since_watering);
        mqtt_flow_alarm->update();

        if(solenoid.isOpen() != solenoid_last_actual) {
            solenoid_last_actual  = solenoid.isOpen();
            solenoid_desired      = solenoid_last_actual;
            solenoid_last_desired = solenoid_last_actual;
            mqtt_solenoid_switch->setState(solenoid_last_actual);
        }

        // backlight
        // if (solenoid.isOpen() != prevSolenoid){
        //     // Solenoid changed
        //     if(solenoid.isOpen()){
        //         // solenoid made open
        //         disp->setBacklight(true);
        //         if(timeout != nullptr){
        //             core::removeTimeout(timeout);
        //             delete timeout;
        //             timeout = nullptr;
        //         }
        //     } else{
        //         // solenoid made close
        //         if(timeout == nullptr)
        //             timeout = new core::timeout_t();
        //         timeout->delay = 15000;
        //         timeout->callback = [](void *custom_pointer)
        //         {
        //             disp->setBacklight(false);
        //         };
        //         core::addTimeout(timeout);
        //     }
        //     prevSolenoid = solenoid.isOpen();
        // }
    }

    void render() {
        if(_alarm && millis() >= lastValveCloseTry + (secondsPerCloseTry * 1000)) {
            solenoid.setState(false);
            lastValveCloseTry = millis();
        }

        // Serial.print(millis());
        // Serial.print(" alarm: ");
        // Serial.println(_alarm);
        // disp->clear();

        // First row (Temperature Humidity and time)
        disp->setCursor(0, 0);
        char buf[32] = { 0 };
        snprintf(buf, sizeof(buf), "%.1fC", aht20.getTemperature());
        disp->setText(buf);

        disp->moveCursor(1);
        snprintf(buf, sizeof(buf), "%.1fH", aht20.getHumidity());
        disp->setText(buf);

        int         hour     = core::time->getHour(true);
        int         minute   = core::time->getMinute();
        std::string hour_s   = hour < 10 ? "0" + std::to_string(hour) : std::to_string(hour);
        std::string minute_s = minute < 10 ? "0" + std::to_string(minute) : std::to_string(minute);
        disp->setCursor(disp->getCols() - 5, 0);
        disp->setText(hour_s);
        disp->setText(":");
        disp->setText(minute_s);

        // Second row (Flow Rate, Volume, US distance)
        disp->setCursor(0, 1);
        snprintf(buf, sizeof(buf), "%.1fL/min", flow_meter.getFlowRate());
        disp->setText(buf);

        snprintf(buf, sizeof(buf), "%.1fL", flow_meter.getVolume24H());
        std::string vol_str = std::string(buf);
        disp->setCursor(disp->getCols() - vol_str.size(), 1);
        disp->setText(vol_str);

        // Third row (US distance, US Percentage)
        disp->setCursor(0, 2);
        snprintf(buf, sizeof(buf), "D:%.1fcm", ultrasonic->getDistance());
        disp->setText(buf);

        // snprintf(buf, sizeof(buf), "%.2f", getUsPercentage());
        // std::string perc_str = std::string(buf);
        // disp->setCursor(disp->getCols() - perc_str.size(), 2);
        // disp->setText(perc_str);

        snprintf(buf, sizeof(buf), "%dL", (int) getUsVolume());
        std::string volume_str = std::string(buf);
        disp->setCursor(disp->getCols() - volume_str.size(), 2);
        disp->setText(volume_str);


        // Fourth row (Solenoid state, US Volume)
        disp->setCursor(0, 3);
        disp->setText("Zawor: ");
        if(solenoid.isOpen())
            disp->setText("Wl ");
        else
            disp->setText("Wyl");

        std::string wifi        = core::wifi::isConnected() ? "W" : " ";
        std::string mqtt        = core::mqtt::isConnected() ? "Q" : " ";

        std::string connections = wifi + " " + mqtt;
        disp->setCursor(disp->getCols() - connections.size(), 3);
        disp->setText(connections);
    }
} // namespace client