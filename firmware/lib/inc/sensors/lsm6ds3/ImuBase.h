#ifndef __LSM6DS3_IMU_BASE_H
#define __LSM6DS3_IMU_BASE_H

#include <stddef.h>

namespace sensors {
namespace lsm6ds3 {

	class ImuBase {
	protected:
        ///////////////////////////////////
        // LSM6DS3 IMU Registers         //
        ///////////////////////////////////
        struct Registers {
            static const uint8_t TEST_PAGE = 0x00;
            static const uint8_t RAM_ACCESS = 0x01;
            static const uint8_t SENSOR_SYNC_TIME = 0x04;
            static const uint8_t SENSOR_SYNC_EN = 0x05;
            static const uint8_t FIFO_CTRL1 = 0x06;
            static const uint8_t FIFO_CTRL2 = 0x07;
            static const uint8_t FIFO_CTRL3 = 0x08;
            static const uint8_t FIFO_CTRL4 = 0x09;
            static const uint8_t FIFO_CTRL5 = 0x0A;
            static const uint8_t ORIENT_CFG_G = 0x0B;
            static const uint8_t REFERENCE_G = 0x0C;
            static const uint8_t INT1_CTRL = 0x0D;
            static const uint8_t INT2_CTRL = 0x0E;
            static const uint8_t WHO_AM_I = 0x0F;
            static const uint8_t CTRL1_XL = 0x10;
            static const uint8_t CTRL2_G = 0x11;
            static const uint8_t CTRL3_C = 0x12;
            static const uint8_t CTRL4_C = 0x13;
            static const uint8_t CTRL5_C = 0x14;
            static const uint8_t CTRL6_G = 0x15;
            static const uint8_t CTRL7_G = 0x16;
            static const uint8_t CTRL8_XL = 0x17;
            static const uint8_t CTRL9_XL = 0x18;
            static const uint8_t CTRL10_C = 0x19;
            static const uint8_t MASTER_CONFIG = 0x1A;
            static const uint8_t WAKE_UP_SRC = 0x1B;
            static const uint8_t TAP_SRC = 0x1C;
            static const uint8_t D6D_SRC = 0x1D;
            static const uint8_t STATUS_REG = 0x1E;
            static const uint8_t OUT_TEMP_L = 0x20;
            static const uint8_t OUT_TEMP_H = 0x21;
            static const uint8_t OUTX_L_G = 0x22;
            static const uint8_t OUTX_H_G = 0x23;
            static const uint8_t OUTY_L_G = 0x24;
            static const uint8_t OUTY_H_G = 0x25;
            static const uint8_t OUTZ_L_G = 0x26;
            static const uint8_t OUTZ_H_G = 0x27;
            static const uint8_t OUTX_L_XL = 0x28;
            static const uint8_t OUTX_H_XL = 0x29;
            static const uint8_t OUTY_L_XL = 0x2A;
            static const uint8_t OUTY_H_XL = 0x2B;
            static const uint8_t OUTZ_L_XL = 0x2C;
            static const uint8_t OUTZ_H_XL = 0x2D;
            static const uint8_t SENSORHUB1_REG = 0x2E;
            static const uint8_t SENSORHUB2_REG = 0x2F;
            static const uint8_t SENSORHUB3_REG = 0x30;
            static const uint8_t SENSORHUB4_REG = 0x31;
            static const uint8_t SENSORHUB5_REG = 0x32;
            static const uint8_t SENSORHUB6_REG = 0x33;
            static const uint8_t SENSORHUB7_REG = 0x34;
            static const uint8_t SENSORHUB8_REG = 0x35;
            static const uint8_t SENSORHUB9_REG = 0x36;
            static const uint8_t SENSORHUB10_REG = 0x37;
            static const uint8_t SENSORHUB11_REG = 0x38;
            static const uint8_t SENSORHUB12_REG = 0x39;
            static const uint8_t FIFO_STATUS1 = 0x3A;
            static const uint8_t FIFO_STATUS2 = 0x3B;
            static const uint8_t FIFO_STATUS3 = 0x3C;
            static const uint8_t FIFO_STATUS4 = 0x3D;
            static const uint8_t FIFO_DATA_OUT_L = 0x3E;
            static const uint8_t FIFO_DATA_OUT_H = 0x3F;
            static const uint8_t TIMESTAMP0_REG = 0x40;
            static const uint8_t TIMESTAMP1_REG = 0x41;
            static const uint8_t TIMESTAMP2_REG = 0x42;
            static const uint8_t STEP_COUNTER_L = 0x4B;
            static const uint8_t STEP_COUNTER_H = 0x4C;
            static const uint8_t FUNC_SRC = 0x53;
            static const uint8_t TAP_CFG1 = 0x58;
            static const uint8_t TAP_THS_6D = 0x59;
            static const uint8_t INT_DUR2 = 0x5A;
            static const uint8_t WAKE_UP_THS = 0x5B;
            static const uint8_t WAKE_UP_DUR = 0x5C;
            static const uint8_t FREE_FALL = 0x5D;
            static const uint8_t MD1_CFG = 0x5E;
            static const uint8_t MD2_CFG = 0x5F;

            /************** Access Device RAM  *******************/
            static const uint8_t ADDR0_TO_RW_RAM = 0x62;
            static const uint8_t ADDR1_TO_RW_RAM = 0x63;
            static const uint8_t DATA_TO_WR_RAM = 0x64;
            static const uint8_t DATA_RD_FROM_RAM = 0x65;

            static const uint16_t RAM_SIZE = 4096;

            /************ External magnetometer raw data output registers  **************/
            static const uint8_t OUT_MAG_RAW_X_L = 0x66;
            static const uint8_t OUT_MAG_RAW_X_H = 0x67;
            static const uint8_t OUT_MAG_RAW_Y_L = 0x68;
            static const uint8_t OUT_MAG_RAW_Y_H = 0x69;
            static const uint8_t OUT_MAG_RAW_Z_L = 0x6A;
            static const uint8_t OUT_MAG_RAW_Z_H = 0x6B;


            /************** Embedded functions register mapping  *******************/
            static const uint8_t SLV0_ADD = 0x02;
            static const uint8_t SLV0_SUBADD = 0x03;
            static const uint8_t SLAVE0_CONFIG = 0x04;
            static const uint8_t SLV1_ADD = 0x05;
            static const uint8_t SLV1_SUBADD = 0x06;
            static const uint8_t SLAVE1_CONFIG = 0x07;
            static const uint8_t SLV2_ADD = 0x08;
            static const uint8_t SLV2_SUBADD = 0x09;
            static const uint8_t SLAVE2_CONFIG = 0x0A;
            static const uint8_t SLV3_ADD = 0x0B;
            static const uint8_t SLV3_SUBADD = 0x0C;
            static const uint8_t SLAVE3_CONFIG = 0x0D;
            static const uint8_t DATAWRITE_SRC_MODE_SUB_SLV0 = 0x0E;
            static const uint8_t CONFIG_PEDO_THS_MIN = 0x0F;
            static const uint8_t CONFIG_TILT_IIR = 0x10;
            static const uint8_t CONFIG_TILT_ACOS = 0x11;
            static const uint8_t CONFIG_TILT_WTIME = 0x12;
            static const uint8_t SM_STEP_THS = 0x13;
            static const uint8_t MAG_SI_XX = 0x24;
            static const uint8_t MAG_SI_XY = 0x25;
            static const uint8_t MAG_SI_XZ = 0x26;
            static const uint8_t MAG_SI_YX = 0x27;
            static const uint8_t MAG_SI_YY = 0x28;
            static const uint8_t MAG_SI_YZ = 0x29;
            static const uint8_t MAG_SI_ZX = 0x2A;
            static const uint8_t MAG_SI_ZY = 0x2B;
            static const uint8_t MAG_SI_ZZ = 0x2C;
            static const uint8_t MAG_OFFX_L = 0x2D;
            static const uint8_t MAG_OFFX_H = 0x2E;
            static const uint8_t MAG_OFFY_L = 0x2F;
            static const uint8_t MAG_OFFY_H = 0x30;
            static const uint8_t MAG_OFFZ_L = 0x31;
            static const uint8_t MAG_OFFZ_H = 0x32;

            /*******************************************************************************
            * Register      : TEST_PAGE
            * Address       : 0X00
            * Bit Group Name: FLASH_PAGE
            * Permission    : RW
            *******************************************************************************/
            static const uint8_t FLASH_PAGE = 0x40;
        };
	};

}}


#endif //__LSM6DS3_IMU_BASE_H