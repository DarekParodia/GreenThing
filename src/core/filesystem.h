#pragma once

#include <FS.h>
#include <LittleFS.h>
#include <stdint.h>
#include <string>

namespace core::filesystem {
    // filesystem functions
    int readFile(std::string path, void *buffer, size_t *len);
    int writeFile(std::string path, void *buffer, size_t *len);
    int appendFile(std::string path, void *buffer, size_t *len);

    // simple
    std::string readFile(std::string path);
    int         writeFile(std::string path, std::string data);
    int         appendFile(std::string path, std::string data);

    // core functions
    void preInit();
    void init();
    void postInit();

    void loop();
} // namespace core::filesystem