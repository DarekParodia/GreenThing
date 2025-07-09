#include "display.h"
#include "client.h"

namespace core::display{
    constexpr unsigned long renderDelay =  1000 / REFRESHRATE;
    unsigned long lastRenderTime = 0;
    unsigned long frameCount = 0;
    core::display::interface::DisplayInterface *displayInterface = nullptr;

    void setup(){
        displayInterface = new core::display::interface::LCD_I2C(20, 4);
        displayInterface->init();
    };
    void loop(){
        if(millis() >= lastRenderTime + renderDelay){
            frameCount++;
            client::render();
            displayInterface->render();
            lastRenderTime = millis();
        }
    };
}