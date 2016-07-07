#ifndef __EV3_LSM330DLC_IMU_CORE_H
#define __EV3_LSM330DLC_IMU_CORE_H

#include <mpl/vector_c.h>
#include <sensors/lsm330dlc/Accelerometer.h>
#include <sensors/lsm330dlc/Gyroscope.h>
#include <ev3/command_info.h>

namespace ev3 {
namespace lsm330dlc {

    template<typename AccelTransport, typename GyroTransport, typename Derived>
    class ImuCore {
    public:
        enum State {
            StateInit, //Initial state should have zero value to place sensor object into bss section
            StateBoth,
            StateAccelerometer,
            StateGyroscope
        };

		typedef sensors::lsm330::Accelerometer<AccelTransport> Accelerometer;
		typedef sensors::lsm330::Gyroscope<GyroTransport> Gyroscope;
		typedef Accelerometer accel_type;
		typedef Gyroscope gyro_type;

        static const uint8_t MODE_COUNT = 3;

        static const uint8_t ACCEL_SAMPLES = 3;
        static const uint8_t GYRO_SAMPLES = 3;
        static const uint8_t FULL_SAMPLES = ACCEL_SAMPLES + GYRO_SAMPLES;

        static const uint8_t FULL_SAMPLE_SIZE = FULL_SAMPLES * sizeof(uint16_t);
        static const uint8_t ACCEL_SAMPLE_SIZE = ACCEL_SAMPLES * sizeof(uint16_t);
        static const uint8_t GYRO_SAMPLE_SIZE = GYRO_SAMPLES * sizeof(uint16_t);

    public:
        //Sensor modes info
        typedef mpl::make_type_list<
            ev3::SensorMode<mpl::vector_c<char, 'I', 'M', 'U', '-', 'A', 'L', 'L'>::type,      FULL_SAMPLES,  ev3::Int16, 5, 0, true, SHRT_MIN, SHRT_MAX>,
            ev3::SensorMode<mpl::vector_c<char, 'I', 'M', 'U', '-', 'A', 'C', 'C'>::type,      ACCEL_SAMPLES, ev3::Int16, 5, 0, true, SHRT_MIN, SHRT_MAX>,
            ev3::SensorMode<mpl::vector_c<char, 'I', 'M', 'U', '-', 'R', 'A', 'T', 'E'>::type, GYRO_SAMPLES,  ev3::Int16, 5, 0, true, SHRT_MIN, SHRT_MAX>
        >::type mode_list;

        //Data sample size
        static const uint8_t sample_size = FULL_SAMPLE_SIZE;

    private:
        Accelerometer accel;
        Gyroscope gyro;

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
            return mode < MODE_COUNT ? State(mode + StateBoth) : StateBoth;
        }

        //Convert state to sensor mode
        static uint8_t getMode(State state) {
            return state - StateBoth;
        }

        INLINE void readAccelSample(uint8_t mode) {
            accel.readSample(buffer(), ACCEL_SAMPLE_SIZE);
            sendSample<0, ACCEL_SAMPLE_SIZE>(mode);
        }

        INLINE void readGyroSample(uint8_t mode) {
            //The gyro sample uses the same place in the buffer is all modes to avoid
            //data placement conflicts during switching from gyro to accel+gyro mode
            gyro.readSample(buffer() + ACCEL_SAMPLE_SIZE, GYRO_SAMPLE_SIZE);
            sendSample<ACCEL_SAMPLE_SIZE, GYRO_SAMPLE_SIZE>(mode);
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
            currentState = StateInit;
        }

        //Starts generation of data events
        void start() {
             if (gyro.getId() == 0xD4) {
                setMode(0);
             }
        }

        //Change the current mode
        void setMode(uint8_t mode) {
            State newState = getState(mode);
            if (newState != currentState) {
                currentState = newState;

                switch (currentState) {
                case StateBoth:
                    gyro.init(Gyroscope::SCALE_250DPS, Gyroscope::ODR_760_BW_100, Gyroscope::InterruptEnabled, Gyroscope::Sync);
                    accel.init(Accelerometer::SCALE_2G, Accelerometer::ODR_400, Accelerometer::InterruptEnabled);
                    break;

                case StateAccelerometer:
                    gyro.reset();
                    accel.init(Accelerometer::SCALE_2G, Accelerometer::ODR_400, Accelerometer::InterruptEnabled);
                    break;

                case StateGyroscope:
                    gyro.init(Gyroscope::SCALE_250DPS, Gyroscope::ODR_760_BW_100, Gyroscope::InterruptEnabled, Gyroscope::NoSync);
                    accel.reset();
                    break;
                }
            }
        }

        //Shanges the sensor's sensitivity
        void setScale(uint8_t scaleInfo) {
            switch (scaleInfo & ScaleInfoMask::Device) {
            case ImuGyroscope:
                if (currentState == StateBoth || currentState == StateGyroscope)
                    gyro.setScale(Gyroscope::Scale(scaleInfo & ScaleInfoMask::Scale));
                break;

            case ImuAccelerometer:
                if (currentState == StateBoth || currentState == StateAccelerometer)
                    accel.setScale(Accelerometer::Scale(scaleInfo & ScaleInfoMask::Scale));
                break;
            }
        }

        //Sets the sensor to initial state
        void reset() {
            setMode(0);
        }

/*
        void dataRequest() {
            switch (currentState) {
            case StateBoth:
                //Gyroscope event follows the accelerometer event
                if (accel.isNewDataAvailable() && gyro.isNewDataAvailable()) {
                    accel.readSample(buffer, ACCEL_SAMPLE_SIZE);
                    gyro.readSample(buffer + ACCEL_SAMPLE_SIZE, GYRO_SAMPLE_SIZE);
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
            }
        }
*/

        //Process data ready event
        void handleEvent(EventSource event) {
            uint8_t mode = getMode(currentState);
            switch (currentState) {
            case StateBoth:
                switch (event) {
                case AccelerometerAvailable:
                    accel.readSample(buffer(), ACCEL_SAMPLE_SIZE);
                    break;
                case GyroscopeAvailable:
                    //Gyroscope event follows the accelerometer event
                    gyro.readSample(buffer() + ACCEL_SAMPLE_SIZE, GYRO_SAMPLE_SIZE);
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
            }
        }
    };
}
}

#endif //__EV3_LSM330DLC_IMU_CORE_H