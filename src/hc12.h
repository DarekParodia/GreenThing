#ifndef HC12_H
#define HC12_H
#include "Arduino.h"
#include "HardwareSerial.h"
#include "SoftwareSerial.h"
#include "Stream.h"

namespace HC12
{
    enum Baudrate
    {
        BPS_1200 = 1200,
        BPS_2400 = 2400,
        BPS_4800 = 4800,
        BPS_9600 = 9600,
        BPS_19200 = 19200,
        BPS_38400 = 38400,
        BPS_57600 = 57600,
        BPS_115200 = 115200
    };

    enum Channel
    {
        CHANNEL_1 = 1,
        CHANNEL_2 = 2,
        CHANNEL_3 = 3,
        CHANNEL_4 = 4,
        CHANNEL_5 = 5,
        CHANNEL_6 = 6,
        CHANNEL_7 = 7,
        CHANNEL_8 = 8
    };

    enum Mode
    {
        FU1 = 1,
        FU2 = 2,
        FU3 = 3,
        FU4 = 4
    };

    class HC12
    {
    public:
        void begin(Stream &serial, int setPin);
        void setBaudrate(Baudrate baud);
        void setChannel(Channel ch);
        void setMode(Mode mode);
        String getParameters();

    private:
        Stream *serial;
        Baudrate baudrate;
        Channel channel;
        Mode mode;
        int setPin;
    };

} // namespace HC12


#endif // HC12_H