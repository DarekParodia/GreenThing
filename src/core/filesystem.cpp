#include "core/filesystem.h"

namespace core::filesystem {
    // filesystem functions
    int readFile(std::string path, void *buffer, size_t *len) {
        File file = LittleFS.open(path.c_str(), "r");
        if(!file)
            return -1; // File open error
        size_t toRead    = *len;
        size_t bytesRead = file.read((uint8_t *) buffer, toRead);
        file.close();
        if(bytesRead == 0)
            return -2; // Read error or empty file
        *len = bytesRead;
        return 0;
    }

    int writeFile(std::string path, void *buffer, size_t *len) {
        File file = LittleFS.open(path.c_str(), "w");
        if(!file)
            return -1; // File open error
        size_t toWrite      = *len;
        size_t bytesWritten = file.write((const uint8_t *) buffer, toWrite);
        file.close();
        if(bytesWritten != toWrite)
            return -2; // Write error
        *len = bytesWritten;
        return 0;
    }

    int appendFile(std::string path, void *buffer, size_t *len) {
        File file = LittleFS.open(path.c_str(), "a");
        if(!file)
            return -1; // File open error
        size_t toWrite      = *len;
        size_t bytesWritten = file.write((const uint8_t *) buffer, toWrite);
        file.close();
        if(bytesWritten != toWrite)
            return -2; // Write error
        *len = bytesWritten;
        return 0;
    }

    // simple
    std::string readFile(std::string path) {
        File file = LittleFS.open(path.c_str(), "r");
        if(!file)
            return std::string();
        std::string result;
        while(file.available())
            result += (char) file.read();
        file.close();
        return result;
    }

    int writeFile(std::string path, std::string data) {
        File file = LittleFS.open(path.c_str(), "w");
        if(!file)
            return -1;
        size_t bytesWritten = file.write((const uint8_t *) data.c_str(), data.size());
        file.close();
        if(bytesWritten != data.size())
            return -2;
        return 0;
    }

    int appendFile(std::string path, std::string data) {
        File file = LittleFS.open(path.c_str(), "a");
        if(!file)
            return -1;
        size_t bytesWritten = file.write((const uint8_t *) data.c_str(), data.size());
        file.close();
        if(bytesWritten != data.size())
            return -2;
        return 0;
    }

    // core functions
    void preInit() {
        // Init filesystem
        if(!LittleFS.begin()) {
            Serial.println("Failed to mount LittleFS");
            return;
        }
    }
    void init() {}
    void postInit() {}

    void loop() {}
} // namespace core::filesystem