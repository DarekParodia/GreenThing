#include "I2C_LCD.h"
#include "client.h"
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

#define disp core::display::displayInterface

modules::Button     button("button1", 0, false, true);
modules::Solenoid   solenoid("solenoid1", 16, 14, false, 250);
modules::FlowMeter  flow_meter("flow_meter1", 13);
modules::Humidity   humidity("humidity1", A0, true, 0, 1024);
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

bool         prevButton              = false;
int          currentVolMeasurment    = -1;
double       currentVolume           = 0.0;

const double containerVolume         = 1000.0; // Liters
const double containerHeight         = 100.0;  // Centimeter

// SAFETY
const double  maxLiters              = 10.0;
const double  secondsAfterCloseCheck = 15.0;
const double  secondsPerCloseTry     = 7.5;
const double  flowTreshold           = 5.0;
bool          _alarm                 = false;

unsigned long lastValveClose         = 0;
unsigned long lastValveCloseTry      = 0;
double        closingFlow            = 0;

bool          prevSolenoid           = false;

// s
void alarm() {
    _alarm = true;
}

void dealarm() {
    _alarm = false;
}

void alarmCheck() {
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

void wateringCycleOn() {
    currentVolume = 0.0;
    solenoid.open();
}

void wateringCycleOff() {
    closingFlow = flow_meter.getFlowRate();
    solenoid.close();
    button.changeState();
    lastValveClose = millis();
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
        disp->clear();
        // disp->setBacklight(false);
    }

    void loop() {
        if(button.isPressed() != prevButton) {
            prevButton = button.isPressed();

            if(button.isPressed())
                wateringCycleOn();
            else
                wateringCycleOff();
        }

        ultrasonic->setTemperature(aht20.getTemperature());
        alarmCheck();

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
    }
} // namespace client