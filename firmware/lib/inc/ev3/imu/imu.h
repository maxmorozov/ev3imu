#ifndef __EV3_IMU_H
#define __EV3_IMU_H

#include <utils/inline.h>
#include <utils/blocking_queue.h>
#include <string.h>

namespace ev3 {
namespace imu {    

    /**
     * IMU wrapper to work with RTOS port.
     * It contains blocking queue to process events from the sensor and
     * switch mode commands from the EV3 host.
     *
     * ImuCore - device-specific template that contains device communication protocol.
     * Commands - class that implements commands that EV3 host can send to the sensor.
     * event_queue_size - size of internal ring buffer that keeps received commands from EV3 host.
     * EepromWriter - class that provides ability to write data to MCU EEPROM.
     * Derived - EV3 sensor implementation. Actually, it is Ev3UartSensor.
     */
    template<template <typename> class ImuCore, typename Commands, size_t events_queue_size, typename EepromWriter, typename Derived>
    class IMU : public ImuCore<Derived> {
    public:
        //Device specific commands are accesible via device type
        typedef Commands commands;
        typedef ImuCore<Derived> base_type;


    private:
        utils::blocking_queue<uint8_t, events_queue_size> events_queue;
        EepromWriter eepromWriter;

        //Callback to call updateEeprom method of ImuCore
        //Using the callback together with EepromWriter parameter allows us
        //to simplify EepromWriter interface and makes
        //EepromWriter independed from ImuCore interface
        class EepromCall {
            //We use pointer instead of reference to allow copying of the functor
            //and to allow calling non-contant methods from constant operator()
            base_type* core;
            uint8_t event;
        public:
            EepromCall(base_type& core_, uint8_t event_)
                : core(&core_), event(event_)
            {
            }

            void operator ()(const uint8_t* data, uint8_t size) const {
                core->updateEeprom(event, data, size);
            }
        };

    public:
        typedef typename base_type::mode_list mode_list;

    public:
        INLINE IMU()
        {
        }

        //Stops generation of data events
        void stop() {
            events_queue.push(StopEvent);
        }

        //Starts generation of data events
        void start() {
            events_queue.push(StartEvent);
        }

        //Event handler loop.
        //Should be called from the dedicated OS process
        void waitForEvent() {
            uint8_t event;
            if (events_queue.pop(event)) {
                switch (event & EventMask::EventKind) {
                case DataEvent:
                    base_type::handleEvent(EventSource(event));
                    break;
                case ScaleEvent:
                    base_type::setScale(event & EventMask::EventInfo);
                    break;
                case ModeEvent:
                    base_type::setMode(event & EventMask::EventInfo);
                    break;
                case ResetEvent:
                    base_type::reset();
                    break;
                case StopEvent:
                    base_type::stop();
                    break;
                case StartEvent:
                    base_type::start();
                    break;
//                case RequestEvent:
//                    core.dataRequest();
//                    break;
                case EepromEvent:
                    eepromWriter.updateEeprom(EepromCall(*this, event & EventMask::EventInfo));
                    break;
                }
            }
        }

        //This method should be called from ISR handler
        void handleAcelDataReady() {
            //TODO - add error processing
            events_queue.push_isr(AccelerometerAvailable);
        }

        //This method should be called from ISR handler
        void handleGyroDataReady() {
            //TODO - add error processing
            events_queue.push_isr(GyroscopeAvailable);
        }

        //This method is called from the host event processor process.
        //So we need to queue this event.
        void changeMode(uint8_t mode) {
            events_queue.push(ModeEvent | (mode & EventMask::EventInfo));
        }

        //Changes sensor's sensitivity
        void setScale(uint8_t scaleInfo) {
            events_queue.push(ScaleEvent | (scaleInfo & EventMask::EventInfo));
        }

        //Returns the device to initial state
        void reset() {
            events_queue.push(ResetEvent);
        }

//        void dataRequest() {
//            events_queue.push(RequestEvent);
//        }

        //Writes EEPROM data for the specified device and its scale into
        //appropriate section of the EEPROM data area
        void writeEeprom(uint8_t eepromInfo, const uint8_t* data, uint8_t size) {
            eepromWriter.write(data, size);
            events_queue.push(EepromEvent | (eepromInfo & EventMask::EventInfo));
        }
    };
}}

#endif //__EV3_IMU_H