#ifndef __SENSORS_SIMPLE_PROVIDER_H
#define __SENSORS_SIMPLE_PROVIDER_H

#include <sensors/SampleProvider.h>

namespace sensors {

    //Simple sample provider that doesn't perform any data tranformations
    template <typename Device>
    class SimpleProvider : public SampleProvider<Device, SimpleProvider<Device> >
    {
    protected:
        friend class SampleProvider<Device, SimpleProvider<Device> >;

        INLINE void initDevice() {
        }

        INLINE void convertSample(uint8_t* data, uint8_t size) const {
        }
    };

}

#endif //__SENSORS_SIMPLE_PROVIDER_H