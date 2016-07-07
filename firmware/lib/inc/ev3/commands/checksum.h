#ifndef __EV3_COMMANDS_CHECKSUM_H
#define __EV3_COMMANDS_CHECKSUM_H

namespace ev3 {
namespace commands {

    static const uint8_t CHECKSUM_SEED = 0xFF;

    // Calculate data buffer check sum
    //
    // @param ckecksum - checksum seed
    // @param data - data buffer
    // @param size - buffer size in bytes
    INLINE uint8_t checksum(uint8_t checksum, const uint8_t* data, uint8_t size) {
        for(int i = 0; i < size; ++i) {
            checksum ^= data[i];
        }
        return checksum;
    }

    // Calculate data buffer check sum
    //
    // @param ckecksum - checksum seed
    // @param data - data buffer
    // @param size - buffer size in bytes
    INLINE uint8_t checksum(const uint8_t* data, uint8_t size) {
        return checksum(CHECKSUM_SEED, data, size);
    }
    
    inline uint8_t checksum(uint8_t p1) {
        return CHECKSUM_SEED ^ p1;
    }

    inline uint8_t checksum(uint8_t p1, uint8_t p2) {
        return checksum(p1) ^ p2;
    }

    inline uint8_t checksum(uint8_t p1, uint8_t p2, uint8_t p3) {
        return checksum(p1 ^ p2) ^ p3;
    }

    //Metafunction for checksum calculation
    template <
        uint8_t p1, uint8_t p2 = 0, uint8_t p3 = 0, uint8_t p4 = 0,
        uint8_t p5 = 0, uint8_t p6 = 0, uint8_t p7 = 0, uint8_t p8 = 0
    >
    struct check_sum {
        static const uint8_t value = CHECKSUM_SEED ^ p1 ^ p2 ^ p3 ^ p4 ^ p5 ^ p6 ^ p7 ^ p8;
    };

}
}


#endif //__EV3_COMMANDS_CHECKSUM_H