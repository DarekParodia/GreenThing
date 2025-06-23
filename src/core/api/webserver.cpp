#include "webserver.h"
#include "core/api/api.h"
#include "core/api/datapoint.h"
#include <vector>

namespace core::api
{

#if defined(ESP32)
    WebServer *server = nullptr;
#elif defined(ESP8266)
    ESP8266WebServer *server = nullptr;
#endif

    void handleRoot()
    {
        // Handle the root URL
        std::vector<DataPoint *> dataPoints = core::api::getDataPoints();
        // Create a JSON object to hold the data points
        DynamicJsonDocument doc(1024);
        JsonArray dataPointsArray = doc.createNestedArray("dataPoints");
        for (auto &dataPoint : dataPoints)
        {
            dataPoint->addToJsonArray(dataPointsArray);
        }
        std::string jsonString;
        serializeJson(doc, jsonString);
// Send the JSON response
#if defined(ESP32)
#elif defined(ESP8266)
        server->send(200, "application/json", jsonString.c_str());
#endif
    }
    void handleNotFound()
    {
// Handle not found URL
#if defined(ESP32)
#elif defined(ESP8266)
        server->send(404, "text/plain", "Not found");
#endif
    }

    WebServer::WebServer()
    {
        // Constructor implementation
    }

    WebServer::~WebServer()
    {
        // Destructor implementation
    }

    void WebServer::init()
    {
        // Initialize the web server
#if defined(ESP32)

#elif defined(ESP8266)
        server = new ESP8266WebServer(webserver_port);
        server->on("/", HTTP_GET, handleRoot);
        server->onNotFound(handleNotFound);
        server->begin();
        Serial.println("Web server started");
#endif
    }

    void WebServer::loop()
    {
        // Handle client requests
        if (server)
        {
#if defined(ESP32)
#elif defined(ESP8266)
            server->handleClient();
#endif
        }
    }
}