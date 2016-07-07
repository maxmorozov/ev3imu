#ifndef __SENSORS_SAMPLE_PROVIDER_H
#define __SENSORS_SAMPLE_PROVIDER_H

#include <stdint.h>

namespace sensors {
    template <typename Device, typename Impl>
    class SampleProvider {
    public:
        typedef typename Device::Scale Scale;
        typedef typename Device::ODR ODR;
        typedef typename Device::DataReadyInterrupt DataReadyInterrupt;
    protected:
        Device device;
        Scale currentScale;

        const Impl* getImpl() const { return static_cast<const Impl*>(this); }
        Impl* getImpl() { return static_cast<Impl*>(this); }
    public:
        // Initialize the device.
        // Input:
        //	- scale = The scale of the device. Should be a Scale value.
        //	- odr = Output data rate of the device. ORD value.
        //  - dataReadyInterrupt = generate an interrupt when sensor's data is available
        INLINE void init(Scale scale, ODR odr, DataReadyInterrupt dataReadyInterrupt) {
            device.init(currentScale = scale, odr, dataReadyInterrupt);
            getImpl()->initDevice();
        }

        // Initialize the device.
        // Input:
        //	- scale = The scale of the device.
        //  - p1, p2, p3 - additional parameters of the device
        #pragma inline=forced
        template <typename T>
        void init(Scale scale, ODR odr, DataReadyInterrupt dataReadyInterrupt, T param) {
            device.init(currentScale = scale, odr, dataReadyInterrupt, param);
            getImpl()->initDevice();
        }

        //Returns the sensor to initial state
        INLINE void reset() {
            device.reset();
            currentScale = Scale(0);
        }

        //Updates the sensor's full scale range
        INLINE void setScale(Scale scale) {
            device.setScale(currentScale = scale);
        }

        //Checks if the device works correctly
        INLINE bool checkDevice() const {
            return device.checkDevice();
        }

        //Returns true if there is available data to read
        INLINE bool isNewDataAvailable() const {
            return device.isNewDataAvailable();
        }

        INLINE void readSample(uint8_t* data, uint8_t size) const {
            device.readSample(data, size);
            getImpl()->convertSample(data, size);
        }

        INLINE void updateEeprom(Scale scale, const uint8_t* data, uint8_t size) {
        }

    };

}


#endif //__SENSORS_SAMPLE_PROVIDER_H