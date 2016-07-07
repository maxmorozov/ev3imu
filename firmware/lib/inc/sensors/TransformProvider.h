#ifndef __SENSORS_TRANSFORM_PROVIDER_H
#define __SENSORS_TRANSFORM_PROVIDER_H

#include <mpl/if.h>
#include <mpl/var.h>
#include <sensors/SampleProvider.h>
#include <sensors/traits/has_big_endian.h>
#include <utils/byte_order.h>
#include <utils/utilities.h>
#include <math/correction.h>
#include <stm8/eeprom.h>

namespace sensors {

    namespace details {
        //This strategy configures the device to produce data in big endian format
        struct BigEndianDevice {
            //Switch device to big-endian mode
            template <typename Device>
            static void init(Device& device) {
                device.setBigEndian();
            }

            //Uses MCU to convert data to big-endian format
            template <uint8_t size>
            static void convert(int16_t (&buffer)[size]) {
            }
        };

        //This strategy converts the samples to big endian format on MCU
        struct BigEndianMcu {
            //Switch device to big-endian mode
            template <typename Device>
            static void init(Device& device) {
            }

            //Uses MCU to convert data to big-endian format
            template <uint8_t size>
            static void convert(int16_t (&data)[size]) {
                static_assert(size == 3, "Expected sample size is 3");
                swap_sample(data);
            }
        };
    }

    //Usage of metafunction class allows us to bind an external Eeprom implemenetation.
    template <typename Eeprom, Eeprom& eeprom>
    struct TransformProvider {

        //Sample provider that corrects samples using transformation matrix
        template <typename Device>
        class Provider : public SampleProvider<Device, Provider<Device> >
        {
            friend class SampleProvider<Device, Provider<Device> >;

            typedef SampleProvider<Device, Provider<Device> > base_type;
            using base_type::device;
            using typename base_type::Scale;
        private:
            math::Transformation<Eeprom, eeprom, Device> transformation;

            typedef uint8_t* pbyte;

            //Select byte order conversion strategy
            typedef typename mpl::if_<traits::has_big_endian<Device>, details::BigEndianDevice, details::BigEndianMcu>::type big_endian_conversion;

        protected:
            //Init the sensor to returning samples in big-endian format
            INLINE void initDevice() {
                big_endian_conversion::init(device);
            }

            //We use explicit offset calculation here to use 8-bit multiplictaion operation
            //By default IAR used 16-bit multiplication routine.
            //The scale is always < 8 because we get it from eepromInfo message, where
            //scale info occupies only 3 bits
            static uint8_t* getDeviceMatrix(Scale scale) {
                return pbyte(eeprom.template get<Device>().matrices[0]) + uint8_t(scale * sizeof(eeprom.template get<Device>().matrices[0]));
            }

        public:
            Provider()
            {
            }

            //Overrides and replaces readSample from base class to
            //avoid redundant data copying
            INLINE void readSample(uint8_t* data, uint8_t size) const {
                int16_t sample[3];
                if (size == sizeof(sample)) {
                    device.readSample((uint8_t*)sample, sizeof(sample));

                    //Convert sample to big-endian format if the device doesn't
                    //support big endian sample format. We need big-endian format
                    //because STM8 has big-endian architecture
                    big_endian_conversion::convert(sample);

                    transformation.transform(base_type::currentScale, sample, (int16_t*)data);
                }
            }

            INLINE void updateEeprom(Scale scale, const uint8_t* data, uint8_t size) {
                stm8::EepromWriter writer;
                writer.write(getDeviceMatrix(scale), data, size);
            }
        };
    };

}

#endif //__SENSORS_TRANSFORM_PROVIDER_H