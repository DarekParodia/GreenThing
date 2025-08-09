#include "core/core.h"
#include "core/time.h"
#include "core/wifi.h"

#include <Arduino.h>

namespace core {
    static std::vector<modules::Module *> modules;
    static std::vector<timeout_t *>       timeouts;

    const int                             loopTime            = 500;                                 // Delay in microseconds
    const double                          loopsPerSecond      = 1000000.0 / (double) core::loopTime; // Number of loops per second
    const double                          loopsPerMillisecond = 1000.0 / (double) core::loopTime;    // Number of loops per millisecond
    unsigned long                         loopCount           = 0;                                   // Loop count

    unsigned long                         lastLoopTime        = 0;
    unsigned long                         currentLoopTime     = 0;
    unsigned long                         deltaTime           = 0; // Time in microseconds since the last loop

    // user loop
    const int     userLoopTime                                = 1000000;                             // Delay in microseconds
    const double  userLoopsPerSecond                          = 1000000.0 / (double) core::loopTime; // Number of loops per second
    const double  userLoopsPerMillisecond                     = 1000.0 / (double) core::loopTime;    // Number of loops per millisecond
    unsigned long userLoopCount                               = 0;                                   // Loop count

    unsigned long lastUserLoopTime                            = 0;
    unsigned long currentUserLoopTime                         = 0;
    unsigned long userDeltaTime                               = 0; // Time in microseconds since the last loop

    std::string   hostname;
    char          chipIdStr[10];

    // Time
    const char *ntpServer          = "pool.ntp.org";
    const long  gmtOffset_sec      = 3600; // Adjust as needed
    const int   daylightOffset_sec = 0;
    const char *timezone           = "CET-1CEST,M3.5.0/2,M10.5.0/3"; // Central European Time

    ESP32Time  *time               = new ESP32Time(0);

    bool        syncNTP() {
        if(!core::wifi::isConnected()) return false;

        // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        configTzTime(timezone, ntpServer);

        Serial.println("Attempting NTP sync...");
        struct tm timeinfo;
        if(getLocalTime(&timeinfo)) {
            core::time->setTimeStruct(timeinfo);
            Serial.println("Time synced via NTP.");
            return true;
        }
        Serial.println("Failed to sync time via NTP.");
        return false;
    }

    void calculateDeltaTime() {
        currentLoopTime = micros();
        deltaTime       = currentLoopTime - lastLoopTime;

        // Ensure the loop time is consistent
        if(deltaTime < loopTime)
            delayMicroseconds(loopTime - deltaTime);
        lastLoopTime = micros();
    }

    void calculateUserDeltaTime() {
        currentUserLoopTime = micros();
        userDeltaTime       = currentUserLoopTime - lastUserLoopTime;
        lastUserLoopTime    = micros();
    }

    void executeTimeouts(int maxExecutionTime = -1) {
        unsigned long startTime = micros();
        for(auto &timeout : timeouts) {
            unsigned long currentTime = micros();
            if(maxExecutionTime > 0 && (currentTime - startTime) >= static_cast<unsigned long>(maxExecutionTime))
                break; // Exit if the maximum execution time is reached
            if(currentTime - timeout->startTime * 1000 >= timeout->delay * 1000) {
                timeout->callback(timeout->custom_pointer);
                timeouts.erase(std::remove(timeouts.begin(), timeouts.end(), timeout), timeouts.end());
            }
        }
    }

    void setup() {
// Setup Hostname
#if defined(ESP32)
        // Use lower 24 bits of MAC address as chip ID (similar to ESP8266 getChipId)
        uint64_t chipId = ESP.getEfuseMac();
        sprintf(chipIdStr, "%06X", (uint32_t) (chipId & 0xFFFFFF)); // Convert to hex string
#elif defined(ESP8266)
        sprintf(chipIdStr, "%06X", ESP.getChipId()); // Convert chip ID to hex string
#endif
        hostname = std::string(HOSTNAME) + "_" + getChipId();

        Serial.print("Board Hostname: ");
        Serial.println(hostname.c_str());

        // Setup Time.h
        const char *date = __DATE__; // e.g. "Jul  3 2025"
        const char *time = __TIME__; // e.g. "15:24:30"

        // Parse date
        char monthStr[4];
        int  day, year;
        sscanf(date, "%3s %d %d", monthStr, &day, &year);

        // Convert month string to number
        int month;
        if(!strcmp(monthStr, "Jan"))
            month = 0;
        else if(!strcmp(monthStr, "Feb"))
            month = 1;
        else if(!strcmp(monthStr, "Mar"))
            month = 2;
        else if(!strcmp(monthStr, "Apr"))
            month = 3;
        else if(!strcmp(monthStr, "May"))
            month = 4;
        else if(!strcmp(monthStr, "Jun"))
            month = 5;
        else if(!strcmp(monthStr, "Jul"))
            month = 6;
        else if(!strcmp(monthStr, "Aug"))
            month = 7;
        else if(!strcmp(monthStr, "Sep"))
            month = 8;
        else if(!strcmp(monthStr, "Oct"))
            month = 9;
        else if(!strcmp(monthStr, "Nov"))
            month = 10;
        else if(!strcmp(monthStr, "Dec"))
            month = 11;
        else
            month = 0; // default fallback

        // Parse time
        // int hour, min, sec;
        // sscanf(time, "%d:%d:%d", &hour, &min, &sec);
        // Disabled because i think that it should be set to midnight

        // Set RTC
        core::time->setTime(0, 0, 0, day, month, year);

        // Setup NTP
        syncNTP();
    }

    void loop() {
        loopCount++;
        calculateDeltaTime();
        executeTimeouts();
        for(auto &module : modules) {
            module->startLoop();
            module->loop();
            module->endLoop();
        }

        if(micros() >= userLoopTime + lastUserLoopTime)
            userLoop();
    }
    void userLoop() {
        userLoopCount++;
        calculateUserDeltaTime();
        for(auto &module : modules) {
            module->startUserLoop();
            module->userLoop();
            module->endUserLoop();

            // Serial.print(module->getName().c_str());
            // Serial.printf(": %d us|", module->getLoopDelta());
        }
        // Serial.println("");
    }
    void delayMicroseconds(unsigned long delay) {
        unsigned long startTime = micros();
        executeTimeouts(delay);
        while(micros() - startTime < delay)
            yield(); // Prevent watchdog reset
    }

    void delay(unsigned long delay) {
        unsigned long startTime = micros();
        executeTimeouts(delay * 1000);
        while(micros() - startTime < delay * 1000)
            yield(); // Prevent watchdog reset
    }
    void addModule(modules::Module *module) {
        modules.push_back(module);
        module->init();
    }

    void addTimeout(timeout_t *timeout) {
        timeout->startTime = millis();
        timeouts.push_back(timeout);
    }

    void removeTimeout(timeout_t *timeout) {
        timeouts.erase(std::remove(timeouts.begin(), timeouts.end(), timeout), timeouts.end());
    }

    void removeModule(modules::Module *module) {
        modules.erase(std::remove(modules.begin(), modules.end(), module), modules.end());
    }

    unsigned long getDeltaTime() {
        return deltaTime;
    }

    std::vector<modules::Module *> getModules() {
        return modules;
    }

    std::string getHostname() {
        return hostname;
    }

    std::string getChipId() {
        return std::string(chipIdStr);
    }
} // namespace core