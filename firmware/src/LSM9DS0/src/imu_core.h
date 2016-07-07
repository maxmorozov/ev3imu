#ifndef __EV3_LSM9DS0_IMU_CORE_H
#define __EV3_LSM9DS0_IMU_CORE_H

#include <mpl/vector_c.h>
#include <sensors/lsm9ds0/Accelerometer.h>
#include <sensors/lsm9ds0/Gyroscope.h>
#include <sensors/lsm9ds0/Magnetometer.h>
#include <ev3/command_info.h>

namespace ev3 {
namespace lsm9ds0 {

    template<typename AccelTransport, typename GyroTransport, template <typename> class SampleProvider, typename Derived>
    class ImuCore {
    public:
        enum State {
            StateInit, //Initial state should have zero value to place sensor object into bss section
            StateAll,
            StateAccelerometer,
            StateGyroscope,
            StateMagnetometer,
        };

		typedef sensors::lsm9ds0::Accelerometer<AccelTransport> Accelerometer;
		typedef sensors::lsm9ds0::Gyroscope<GyroTransport> Gyroscope;
		typedef sensors::lsm9ds0::Magnetometer<AccelTransport> Magnetometer;
		typedef Accelerometer accel_type;
		typedef Gyroscope gyro_type;
		typedef Magnetometer magnetometer_type;

        static const uint8_t MODE_COUNT = 4;

        static const uint8_t ACCEL_SAMPLES = 3;
        static const uint8_t GYRO_SAMPLES = 3;
        static const uint8_t MAGNETOMETER_SAMPLES = 3;
        static const uint8_t FULL_SAMPLES = ACCEL_SAMPLES + GYRO_SAMPLES + MAGNETOMETER_SAMPLES;

        static const uint8_t FULL_SAMPLE_SIZE = FULL_SAMPLES * sizeof(uint16_t);
        static const uint8_t ACCEL_SAMPLE_SIZE = ACCEL_SAMPLES * sizeof(uint16_t);
        static const uint8_t GYRO_SAMPLE_SIZE = GYRO_SAMPLES * sizeof(uint16_t);
        static const uint8_t MAGNETOMETER_SAMPLE_SIZE = MAGNETOMETER_SAMPLES * sizeof(uint16_t);

        static const uint8_t GYRO_SAMPLE_OFFSET = ACCEL_SAMPLE_SIZE;
        static const uint8_t MAGNETOMETER_SAMPLE_OFFSET = ACCEL_SAMPLE_SIZE + GYRO_SAMPLE_SIZE;
    public:
        //Sensor modes info
        typedef mpl::make_type_list<
            ev3::SensorMode<mpl::vector_c<char, 'I', 'M', 'U', '-', 'A', 'L', 'L'>::type,      FULL_SAMPLES,         ev3::Int16, 5, 0, true, SHRT_MIN, SHRT_MAX>,
            ev3::SensorMode<mpl::vector_c<char, 'I', 'M', 'U', '-', 'A', 'C', 'C'>::type,      ACCEL_SAMPLES,        ev3::Int16, 5, 0, true, SHRT_MIN, SHRT_MAX>,
            ev3::SensorMode<mpl::vector_c<char, 'I', 'M', 'U', '-', 'R', 'A', 'T', 'E'>::type, GYRO_SAMPLES,         ev3::Int16, 5, 0, true, SHRT_MIN, SHRT_MAX>,
            ev3::SensorMode<mpl::vector_c<char, 'I', 'M', 'U', '-', 'M', 'A', 'G'>::type,      MAGNETOMETER_SAMPLES, ev3::Int16, 5, 0, true, SHRT_MIN, SHRT_MAX>
        >::type mode_list;

        //Data sample size
        static const uint8_t sample_size = FULL_SAMPLE_SIZE;

    private:
        SampleProvider<Accelerometer> accel;
        SampleProvider<Gyroscope> gyro;
        SampleProvider<Magnetometer> magnetometer;

        State currentState;

        Derived* sender() {
            return static_cast<Derived*>(this);
        }

        //Access the buffer manager to get buffer to place sample in
        uint8_t* buffer() {
            return sender()->getBuffer();
        }

        template <uint8_t offset, uint8_t size>
        void sendSample(uint8_t mode) {
            sender()->sendData<offset, size>(mode);
        }

        //Convert sensor mode to the state
        static State getState(uint8_t mode) {
            return mode < MODE_COUNT ? State(mode + StateAll) : StateAll;
        }

        //Convert state to sensor mode
        static uint8_t getMode(State state) {
            return state - StateAll;
        }

        INLINE void readAccelSample(uint8_t mode) {
            accel.readSample(buffer(), ACCEL_SAMPLE_SIZE);
            sendSample<0, ACCEL_SAMPLE_SIZE>(mode);
        }

        INLINE void readGyroSample(uint8_t mode) {
            //The gyro sample uses the same place in the buffer is all modes to avoid
            //data placement conflicts during switching from gyro to accel+gyro mode
            gyro.readSample(buffer() + GYRO_SAMPLE_OFFSET, GYRO_SAMPLE_SIZE);
            sendSample<GYRO_SAMPLE_OFFSET, GYRO_SAMPLE_SIZE>(mode);
        }

        INLINE void readMagnetometerSample(uint8_t mode) {
            //The gyro sample uses the same place in the buffer is all modes to avoid
            //data placement conflicts during switching from gyro to accel+gyro mode
            magnetometer.readSample(buffer() + MAGNETOMETER_SAMPLE_OFFSET, MAGNETOMETER_SAMPLE_SIZE);
            sendSample<MAGNETOMETER_SAMPLE_OFFSET, MAGNETOMETER_SAMPLE_SIZE>(mode);
        }

    public:
        INLINE ImuCore()
            : currentState(StateInit)
        {
        }

        //Stops generation of data events
        void stop() {
            accel.reset();
            gyro.reset();
            magnetometer.reset();
            currentState = StateInit;
        }

        //Starts generation of data events
        void start() {
             if (gyro.checkDevice() && accel.checkDevice()) {
                setMode(0);
             }
        }

        //Change the current mode
        void setMode(uint8_t mode) {
            State newState = getState(mode);
            if (newState != currentState) {
                currentState = newState;

                switch (currentState) {
                case StateAll:
                    accel.init(Accelerometer::SCALE_2G, Accelerometer::ODR_200, Accelerometer::InterruptEnabled, Accelerometer::BW_194);
                    gyro.init(Gyroscope::SCALE_245DPS, Gyroscope::ODR_760_BW_100, Gyroscope::InterruptEnabled, Gyroscope::Sync);
                    magnetometer.init(Magnetometer::SCALE_2GS, Magnetometer::ODR_100, Magnetometer::InterruptDisabled);
                    break;

                case StateAccelerometer:
                    gyro.reset();
                    accel.init(Accelerometer::SCALE_2G, Accelerometer::ODR_800, Accelerometer::InterruptEnabled, Accelerometer::BW_362);
                    magnetometer.reset();
                    break;

                case StateGyroscope:
                    gyro.init(Gyroscope::SCALE_245DPS, Gyroscope::ODR_760_BW_100, Gyroscope::InterruptEnabled, Gyroscope::NoSync);
                    accel.reset();
                    magnetometer.reset();
                    break;

                case StateMagnetometer:
                    gyro.reset();
                    accel.init(Accelerometer::SCALE_2G, Accelerometer::ODR_100, Accelerometer::InterruptDisabled, Accelerometer::BW_50);
                    magnetometer.init(Magnetometer::SCALE_2GS, Magnetometer::ODR_100, Magnetometer::InterruptEnabled);
                    break;
                }
            }
        }

        //Shanges the sensor's sensitivity
        void setScale(uint8_t scaleInfo) {
            switch (scaleInfo & ScaleInfoMask::Device) {
            case ImuGyroscope:
                if (currentState == StateAll || currentState == StateGyroscope)
                    gyro.setScale(Gyroscope::Scale(scaleInfo & ScaleInfoMask::Scale));
                break;

            case ImuAccelerometer:
                if (currentState == StateAll || currentState == StateAccelerometer)
                    accel.setScale(Accelerometer::Scale(scaleInfo & ScaleInfoMask::Scale));
                break;

            case ImuMagnetometer:
                if (currentState == StateAll || currentState == StateMagnetometer)
                    magnetometer.setScale(Magnetometer::Scale(scaleInfo & ScaleInfoMask::Scale));
                break;
            }
        }

        //Sets the sensor to initial state
        void reset() {
            setMode(0);
        }

        //Updates EEPROM data for the device and scale specified by eepromInfo
        void updateEeprom(uint8_t eepromInfo, const uint8_t* data, uint8_t size) {
            switch (eepromInfo & ScaleInfoMask::Device) {
            case ImuGyroscope:
                gyro.updateEeprom(Gyroscope::Scale(eepromInfo & ScaleInfoMask::Scale), data, size);
                break;

            case ImuAccelerometer:
                accel.updateEeprom(Accelerometer::Scale(eepromInfo & ScaleInfoMask::Scale), data, size);
                break;

            case ImuMagnetometer:
                magnetometer.updateEeprom(Magnetometer::Scale(eepromInfo & ScaleInfoMask::Scale), data, size);
                break;
            }
        }

/*
        void dataRequest() {
            switch (currentState) {
            case StateAll:
                //Gyroscope event follows the accelerometer event
                if (accel.isNewDataAvailable() && gyro.isNewDataAvailable()) {
                    accel.readSample(buffer, ACCEL_SAMPLE_SIZE);
                    gyro.readSample(buffer + GYRO_SAMPLE_OFFSET, GYRO_SAMPLE_SIZE);
                    magnetometer.readSample(buffer + MAGNETOMETER_SAMPLE_OFFSET, MAGNETOMETER_SAMPLE_SIZE);
                    sendSample(buffer, sizeof(buffer), currentState);
                }
                break;

            case StateAccelerometer:
                if (accel.isNewDataAvailable()) {
                    readAccelSample();
                }
                break;

            case StateGyroscope:
                if (gyro.isNewDataAvailable()) {
                    readGyroSample();
                }
                break;

            case StateMagnetometer:
                if (magnetometer.isNewDataAvailable()) {
                    readMagnetometerSample();
                }
                break;
            }
        }
*/

        //Process data ready event
        void handleEvent(EventSource event) {
            uint8_t mode = getMode(currentState);
            switch (currentState) {
            case StateAll:
                switch (event) {
                case AccelerometerAvailable:
                    accel.readSample(buffer(), ACCEL_SAMPLE_SIZE);
                    break;
                case GyroscopeAvailable:
                    //Gyroscope event follows the accelerometer event
                    gyro.readSample(buffer() + GYRO_SAMPLE_OFFSET, GYRO_SAMPLE_SIZE);
                    magnetometer.readSample(buffer() + MAGNETOMETER_SAMPLE_OFFSET, MAGNETOMETER_SAMPLE_SIZE);
                    sendSample<0, FULL_SAMPLE_SIZE>(mode);
                    break;
                }
                break;

            case StateAccelerometer:
                if (event == AccelerometerAvailable) {
                    readAccelSample(mode);
                }
                break;

            case StateGyroscope:
                if (event == GyroscopeAvailable) {
                    readGyroSample(mode);
                }
                break;

            case StateMagnetometer:
                //Version 1 shares data ready interrupt between accelerometer and magnetometer
                if (event == AccelerometerAvailable) {
                    readMagnetometerSample(mode);
                }
                break;
            }
        }
    };
}
}

#endif //__EV3_LSM9DS0_IMU_CORE_H
