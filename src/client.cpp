#include "client.h"

#include <Arduino.h>
#include <Wire.h>
#include <string>

#include "mod/button.h"
#include "mod/solenoid.h"
#include "mod/led.h"
#include "mod/flow_meter.h"
#include "mod/humidity.h"
#include "mod/ultrasonic.h"
#include "mod/rcwl_1x05.h"
#include "mod/AHT20.h"

#include "core/core.h"
#include "core/time.h"
#include "core/wifi/wifi.h"
#include "core/display/display.h"

#include <Wire.h>
#include "I2C_LCD.h"

#define disp core::display::displayInterface

mod::Button button("button1", 0, false, true);           
mod::Solenoid solenoid("solenoid1", 16, 14, false, 250); 
mod::FlowMeter flow_meter("flow_meter1", 13);            
mod::Humidity humidity("humidity1", A0, true, 0, 1024);       
mod::Ultrasonic *ultrasonic = new mod::RCWL_1x05("rcwl1x05"); 
mod::AHT20 aht20("AHT20");

// byte char_litersPerMinute[] = {
//     B01000,
//     B01000,
//     B01000,
//     B01110,
//     B00000,
//     B01010,
//     B10101,
//     B10001};

bool prevButton = false;
int currentVolMeasurment = -1;
double currentVolume = 0.0;

void wateringCycleOn(){
    currentVolume = 0.0;
    solenoid.open();
}

void wateringCycleOff(){
    solenoid.close();
}

namespace client
{
    void setup()
    {
        core::addModule(&button);
        core::addModule(&solenoid);
        core::addModule(&flow_meter);
        // core::addModule(&humidity);
        core::addModule(ultrasonic);
        core::addModule(&aht20);

        currentVolMeasurment = flow_meter.startVolumeMeasurment(&currentVolume);
        wateringCycleOff();

        disp->clear();
    }

    void loop()
    {
        if(button.isPressed() != prevButton){
            prevButton = button.isPressed();

            if(button.isPressed()){
                wateringCycleOn();
            } else{
                wateringCycleOff();
            }
        }
    }

    void userLoop()
    {
        // // lcd.clear();
        // // Display IP
        // // lcd.flush();
        // lcd.setCursor(0, 0);
        // if (core::wifi::isConnected())
        //     lcd.print(core::wifi::getStringIP().c_str());
        // else
        //     lcd.print("Connecting...");

        // // Print Current Time
        // int hour = core::time->getHour(true);
        // int minute = core::time->getMinute();

        // std::string hour_s = hour < 10 ? "0" + std::to_string(hour) : std::to_string(hour);
        // std::string minute_s = minute < 10 ? "0" + std::to_string(minute) : std::to_string(minute);

        // lcd.setCursor(15, 0);
        // lcd.print(hour_s.c_str());
        // // lcd.setCursor(17, 0);
        // lcd.print(':');
        // lcd.print(minute_s.c_str());

        // // Print Flow Data
        // lcd.setCursor(0, 1);
        // lcd.printf("F:%.1f", flow_meter.getFlowRate());
        // lcd.write(0);
        // lcd.print("   ");
        // // lcd.moveCursorLeft(2U);

        // // Print Volume Data
        // lcd.setCursor(10, 1);
        // lcd.printf("V:%.1fL", currentVolume);
        // lcd.print("   ");
        // // lcd.moveCursorLeft(2U);

        // // Print Ground Humidity
        // // lcd.setCursor(0, 2);
        // // lcd.printf("HG:%.1f", humidity.getHumidity());
        // // lcd.print('%');
        // // lcd.print(' ');
        // // lcd.print(' ');
        // // lcd.moveCursorLeft(2U);

        // // Print Ultrasonic
        // lcd.setCursor(10, 2);
        // lcd.printf("D:%.2fcm", ultrasonic->getDistance());
        // lcd.print("   ");

        // // Print Solenoid
        // lcd.setCursor(0, 3);
        // lcd.print("Zawor: ");
        // if(solenoid.isOpen()){
        //     lcd.print("Wl ");
        // } else {
        //     lcd.print("Wyl");
        // }

    }

    void render(){
        disp->clear();
        disp->setCursor(0, 0);
        char buf[32];
        snprintf(buf, sizeof(buf), "Temperature: %.2f", aht20.getTemperature());
        disp->setText(buf);

        disp->setCursor(0, 1);
        snprintf(buf, sizeof(buf), "Humidity: %.2f", aht20.getHumidity());
        disp->setText(buf);

    }
}