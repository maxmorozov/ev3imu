#ifndef __EV3_COMMAND_INFO_H
#define __EV3_COMMAND_INFO_H

namespace ev3 {

    //Message format:
    //
    //Data info format: CCCNNNNN - CCC - event code, NNNNN - event source number
    //Scale info format: CCCDDSSS - CCC - event code, DD - device number, SSS - sensitivity range number
    //Mode info format: CCCMMMMM - CCC - event code, MMMMM - mode number
    //Eeprom info format: CCCDDSSS - CCC - event code, DD - device number, SSS - sensitivity range number

    enum EventKind {
        DataEvent = 0,
        ScaleEvent = 0x20,
        ModeEvent = 0x40,
        ResetEvent = 0x60,
        StartEvent = 0x80,
        StopEvent = 0xA0,
        RequestEvent = 0xC0,
        EepromEvent = 0xE0
    };

    struct EventMask {
        static const int EventKind = 0xE0;
        static const int EventInfo = 0x1F;
    };

    struct ScaleInfoMask {
        static const int Device = 0x18;
        static const int Scale  = 0x07;
    };

    enum EventSource {
        GyroscopeAvailable,
        AccelerometerAvailable,
        MagnetometerAvailable
    };

    //Constants to select device
    enum ImuDevice {
        ImuAccelerometer = 0,
        ImuGyroscope = 0x08,
        ImuMagnetometer = 0x10,

        ImuReserved = 0x18
    };
}

#endif // __EV3_COMMAND_INFO_H