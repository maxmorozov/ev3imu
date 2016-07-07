#ifndef __STM8_PORT_DEFS_H
#define __STM8_PORT_DEFS_H


namespace stm8 {

    enum Level {
        Low = 0,
        High = 1
    };

    //Port data direction mode
    enum Direction {
        Input = 0,
        Output= 1
    };

    //Input port mode
    enum InputMode {
        Floating = 0,
        PullUp = 1
    };

    //Output pin mode
    enum OutputMode {
        OpenDrain = 0,
        PushPull = 1
    };

    //Output pin speed limitation
    enum OptputSpeedMode {
        Speed_2Mhz = 0,
        Speed_10Mhz = 1
    };

    //Interrupts on input pin
    enum InterruptMode {
        FallingEdgeAndLowLevel = 0,
        RisingEdge = 1,
        FallingEdge = 2,
        RisingAndFallingEdge = 3,

        InterruptsDisabled
    };

}

#endif //__STM8_PORT_DEFS_H