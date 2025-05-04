#include "hc12.h"

namespace HC12
{
    void HC12::begin(Stream &serial, int setPin)
    {
        this->serial = &serial;
        this->setPin = setPin;
        pinMode(setPin, OUTPUT);
        digitalWrite(setPin, LOW); // Set the SET pin to LOW to enter command mode
        delay(100);
    }

    void HC12::setBaudrate(Baudrate baud)
    {
        if (baudrate == baud)
        {
            return; // No need to change the baudrate
        }
        baudrate = baud;
        digitalWrite(setPin, HIGH); // Set the SET pin to HIGH to exit command mode
        delay(100);
        serial->print("AT+B");
        serial->print(baud);
        serial->println();
        delay(100);
        digitalWrite(setPin, LOW); // Set the SET pin to LOW to enter command mode again
    }

    void HC12::setChannel(Channel ch)
    {
        if (channel == ch)
        {
            return; // No need to change the channel
        }
        channel = ch;
        digitalWrite(setPin, HIGH); // Set the SET pin to HIGH to exit command mode
        delay(100);
        serial->print("AT+C");
        serial->print(channel);
        serial->println();
        delay(100);
        digitalWrite(setPin, LOW); // Set the SET pin to LOW to enter command mode again
    }

    void HC12::setMode(Mode mode)
    {
        if (this->mode == mode)
        {
            return; // No need to change the mode
        }
        this->mode = mode;
        digitalWrite(setPin, HIGH); // Set the SET pin to HIGH to exit command mode
        delay(100);
        serial->print("AT+FU");
        serial->print(mode);
        serial->println();
        delay(100);
        digitalWrite(setPin, LOW); // Set the SET pin to LOW to enter command mode again
    }

    String HC12::getParameters()
    {
        digitalWrite(setPin, HIGH); // Set the SET pin to HIGH to exit command mode
        delay(100);
        serial->println("AT+RX");
        delay(100);
        String response = "";
        while (serial->available())
        {
            response += (char)serial->read();
        }
        digitalWrite(setPin, LOW); // Set the SET pin to LOW to enter command mode again
        return response;
    }
} // namespace HC12