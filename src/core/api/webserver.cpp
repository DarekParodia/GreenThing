#include "webserver.h"
#include "core/api/api.h"
#include "core/api/datapoint.h"
#include <vector>

namespace core::api
{
    ESP8266WebServer *server = nullptr;

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
        server->send(200, "application/json", jsonString.c_str());
    }
    void handleNotFound()
    {
        // Handle not found URL
        server->send(404, "text/plain", "Not found");
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
        server = new ESP8266WebServer(webserver_port);
        server->on("/", HTTP_GET, handleRoot);
        server->onNotFound(handleNotFound);
        server->begin();
        Serial.println("Web server started");
    }

    void WebServer::loop()
    {
        // Handle client requests
        if (server)
        {
            server->handleClient();

            
        }
    }
}