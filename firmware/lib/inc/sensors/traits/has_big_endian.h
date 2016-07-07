#ifndef __SENSORS_TRAITS_HAS_BIG_ENDIAN_H
#define __SENSORS_TRAITS_HAS_BIG_ENDIAN_H

namespace sensors {
namespace traits {

    //A metafunction to check if the device has member function with name setBigEndian
    template <typename Device>
    struct has_big_endian {
    private:
        typedef char yes[1];
        typedef char no[2];

        template <typename T, void (T::*)()> struct empty {};
        template <typename T> static no& check(...);
        template <typename T> static yes& check(empty<T, &T::setBigEndian>*);
    public:
        static const bool value = sizeof(check<Device>(0)) == sizeof(yes);
    };

}}


#endif //__SENSORS_TRAITS_HAS_BIG_ENDIAN_H