#ifndef __STM8_EEPROM_H
#define __STM8_EEPROM_H

#include <mpl/fold.h>
#include <mpl/returns.h>
#include <utils/copy.h>
#include <stm8s_flash.h>

namespace stm8 {

    namespace details {
        //Initial state for type list folding
        struct root {
        };

        //Metafunction to generate linear hierarchy
        template <typename State, typename Item>
        struct HierarchyGenerator {
            struct type: Item, State {
            };
        };

        template <typename Item>
        struct HierarchyGenerator<root, Item> {
            typedef Item type;
        };
    }

    //EEPROM data associated with some device
    //The application should specialize this template for each
    //device type that should keep its data in EEPROM.
    template <typename Tag> struct EepromData;

    //Helper metafunction to make EEPROM data from device type
    template <typename T>
    struct make_eeprom {
        typedef EepromData<T> type;
    };

    //Combines data structures associated with some device typem(Tag type)
    template <typename TypeList>
    class Eeprom : mpl::fold<TypeList, details::root, details::HierarchyGenerator>::type {
    public:
        //Returns a constant reference to the device data structure by device tag
        template <typename Tag>
        const EepromData<Tag>& get() const {
            return *static_cast<const EepromData<Tag>*>(this);
        }

        //Returns a reference to the device data structure by device tag
        template <typename Tag>
        EepromData<Tag>& get() {
            return *static_cast<EepromData<Tag>*>(this);
        }

        //Returns appropriate EEPROM section type
        template <typename Tag>
        struct apply : mpl::returns< EepromData<Tag> > {};
    };

    //Enables write access to EEPROM
    class EepromWriter {
    private:
        void writeDword(uint16_t* dest, const uint16_t* src) {
          // Enable Word Write Once
          FLASH()->CR2 |= FLASH_CR2_WPRG;
          FLASH()->NCR2 &= (uint8_t)(~FLASH_NCR2_NWPRG);

          dest[0] = src[0]; // Write two bytes - from lowest address
          dest[1] = src[1]; // Write two bytes - from higher address

          //Does not have effect on low density stm8, because they don't have
          //Read-While-Write (RWW) capability
#if !defined(STM8S103) && !defined(STM8S003) && !defined(STM8S903) && !defined(STM8AF622x)
          while ((FLASH()->IAPSR & FLASH_IAPSR_EOP) == 0) {
          }
#endif
        }

    public:
        //Unlock data memory
        EepromWriter() {
            FLASH()->DUKR = FLASH_RASS_KEY2; /* Warning: keys are reversed on data memory !!! */
            FLASH()->DUKR = FLASH_RASS_KEY1;
        }

        ~EepromWriter() {
            FLASH()->IAPSR &= (uint8_t)FLASH_MEMTYPE_DATA;
        }

        //Writes the data block using packet writing
        void write(uint8_t* dest, const uint8_t* src, uint8_t size) {
            uint8_t dwords = size >> 2;
            uint8_t tail = size & 3;

            while (dwords > 0) {
                writeDword((uint16_t*)dest, (const uint16_t*)src);
                dest += 4;
                src  += 4;
                --dwords;
            }
            while (tail > 0) {
                *dest++ = *src++;
                --tail;
            }
        }

    };

}

#endif //__STM8_EEPROM_H