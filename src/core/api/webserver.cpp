#include "webserver.h"
#include "core/api/api.h"

namespace core::api
{
    ESP8266WebServer *server = nullptr;

    void handleRoot()
    {
        // Handle the root URL
        server->send(200, "text/plain", "Hello, world!");
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