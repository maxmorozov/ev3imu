#include <scmRTOS.h>
#include <stm8/timer.h>
#include <stm8/pin.h>
#include <stm8/port_config.h>
#include <stm8/spi.h>
#include <stm8/clock_control.h>
#include <stm8/eeprom.h>

#include <usrlib.h>
#include <stm8/uart.h>
#include <ev3/uart_sensor.h>
#include <ev3/uart_speed.h>

#include <sensors/lsm6ds3/SpiAddressStrategy.h>
#include <ev3/imu/imu.h>

#include <ev3/empty_writer.h>
#include <ev3/eeprom_writer.h>

#include <stm8/eeprom.h>
#include <sensors/SimpleProvider.h>
#include <sensors/TransformProvider.h>
#include <math/matrix.h>

#include "imu_core.h"
#include "imu_commands.h"

using namespace stm8;
using namespace sensors;

//---------------------------------------------------------------------------
//
//      Process types
//
typedef OS::process<OS::pr0, 150> CommandHandler;
typedef OS::process<OS::pr1, 150> SensorHandler;

template<> void CommandHandler::exec();
template<> void SensorHandler::exec();

//---------------------------------------------------------------------------
//
//      Process objects
//
CommandHandler commandHandler @ "HW_STACK";
SensorHandler sensorHandler  @ "HW_STACK";

//---------------------------------------------------------------------------
//
//      Hardware initialization section
//
//---------------------------------------------------------------------------
//
//      GPIO pins configuration
//

//Unused I/O pins are configured as input with internal pull-up resistor.
//This configuration produces smaller code.
typedef input_traits<PullUp, InterruptsDisabled> unused_trait;
//Unused I/O pins are configured as output push-pull low.
//typedef output_traits<PushPull, Speed_2Mhz> unused_trait;
typedef Pin<Port_B, 4, unused_trait> PinB4;
typedef Pin<Port_B, 5, unused_trait> PinB5;
typedef Pin<Port_D, 3, unused_trait> PinD3;
typedef Pin<Port_D, 4, unused_trait> PinD4;
typedef Pin<Port_C, 3, unused_trait> PinC3;


//UART pins
typedef Pin<Port_D, 5, output_traits<PushPull, Speed_10Mhz> > PinUartTX;
typedef Pin<Port_D, 6, input_traits<Floating, InterruptsDisabled> > PinUartRX;

//Chip select pins
typedef Pin<Port_A, 3, output_traits<PushPull, Speed_10Mhz, High>, Low> PinImuSel;

//Data ready interrupt pins
typedef Pin<Port_C, 4, input_traits<Floating, RisingEdge> > PinAccelDataReady;
typedef Pin<Port_D, 2, input_traits<Floating, RisingEdge> > PinGyroDataReady;

typedef PortConfigurer<mpl::make_type_list<
    PinImuSel,
    PinAccelDataReady, PinGyroDataReady,
    PinUartTX, PinUartRX,
    PinB4, PinB5, PinD3, PinD4, PinC3>::type> configurer;


//---------------------------------------------------------------------------
//
//      System timer setup parameters
//

static const uint32_t systick_period = 1000; //in microseconds
typedef stm8::timer_8bit<systick_period, F_MASTER / 1000000> timer4cfg;

//---------------------------------------------------------------------------
//
//      SPI transport between MCU and the sensor
//

typedef stm8::spi_config<
    SpiPrescaler_2,
    SpiModeMaster,
    SpiFirstBitMsb,
    SpiClockPolarityHigh,
    SpiClockPhase2Edge,
    SpiDataDirectionDuplex,
    SpiNssSoft,
    0> SpiConfig;

typedef stm8::SPI<SpiConfig> Spi;

typedef sensors::SpiTransport<Spi, PinImuSel, sensors::lsm6ds3::SpiAddressStrategy> ImuTransport;

typedef lsm6ds3::Gyroscope<ImuTransport> Gyroscope;
typedef lsm6ds3::Accelerometer<ImuTransport> Accelerometer;

//---------------------------------------------------------------------------
//
//      EEPROM data layout
//

//Each sensor supports a number of modes with different full-range scales
//Each mode should be calibrated individually and we keep a transformation matrix
//for each mode
typedef math::DenseMatrix<int16_t, 4, 3, utils::WordCopy> TranformationMatrix;

template <typename T>
class EepromData {
private:
    TranformationMatrix matrices[sensors::traits::scale_count<T>::value];
public:
    static_assert(sizeof(matrices) <= 256, "Cannot use 8-bit index in pointer arithmetic");
    
    INLINE int16_t* get(uint8_t scale) {
        return (int16_t*)((uint8_t*)(matrices[0]) + uint8_t(scale * sizeof(matrices[0])));
    }
};

typedef mpl::make_type_list<Gyroscope, Accelerometer>::type devices;
typedef stm8::Eeprom<EepromData, devices> eeprom_type;

#pragma location=FLASH_DATA_START_PHYSICAL_ADDRESS
__no_init eeprom_type eeprom;

//---------------------------------------------------------------------------
//
//      UART configuration
//

typedef UartConfig<F_MASTER, 115200> uart_config;
typedef Uart<Uart1, 32> uart_type;

//---------------------------------------------------------------------------
//
//      IMU class that integrates all devices
//

#if 1
template <typename Derived>
struct imu_core_type : ev3::lsm6ds3::ImuCore<ImuTransport, sensors::TransformProvider<eeprom_type, eeprom>::Provider, Derived> {};
template <typename Derived>
struct imu_type : ev3::imu::IMU<imu_core_type, ev3::lsm6ds3::Commands, 32, ev3::EepromWriter<TranformationMatrix>, Derived> {};
#else
template <typename Derived>
struct imu_core_type : ev3::lsm6ds3::ImuCore<ImuTransport, sensors::SimpleProvider, Derived> {};
template <typename Derived>
struct imu_type : ev3::imu::IMU<imu_core_type, ev3::lsm6ds3::Commands, 32, ev3::EmptyEepromWriter, Derived> {};
#endif


//---------------------------------------------------------------------------
//
//      EV3 UART sensor that provides data to EV3 host
//

typedef ev3::Ev3UartSensor<97, uart_type, uart_config, imu_type> sensor_type;
sensor_type sensor;

//Configure HSE as the clock source
bool initClock();

//Turns off unused peripheral blocks
inline void disableUnusedPeripherals() {
    CLK()->PCKENR1 = CLK_PCKENR1_TIM4 | CLK_PCKENR1_UART2 | CLK_PCKENR1_UART1 | CLK_PCKENR1_SPI;
    CLK()->PCKENR2 = 0;
}

int main()
{
    disableUnusedPeripherals();

    initClock();

    //Initial port configuration
	configurer::configure();

    Spi::init();
    Spi::enable();

    // Start System Timer
    // TODO: Set lowest priority for system timer ISR
    TIM4()->PSCR = timer4cfg::prescaler;
    TIM4()->ARR = timer4cfg::auto_reset;
    TIM4()->CR1 = TIM4_CR1_CEN;
    TIM4()->EGR = 0;
    TIM4()->IER = TIM4_IER_UIE;

    OS::run();
}

//Configure HSE as the clock source
bool initClock() {
    CLK()->CKDIVR = 0;
    CLK()->SWCR |= CLK_SWCR_SWEN; //Enable automatic switch to HSE
    CLK()->SWR = (uint8_t)ClockSourceHSE;

    uint16_t timeout = 0xFFFF;
    while ((CLK()->SWCR & CLK_SWCR_SWBSY) == 1 && timeout != 0) {
        --timeout;
    }

    if (timeout != 0) {
        CLK()->ICKR &= ~CLK_ICKR_HSIEN; //Disable HSI
        CLK()->CSSR |= CLK_CSSR_CSSEN;  //Enable fallback to HSI
        return true;
    }

    return false;
}

namespace OS {

template<> void CommandHandler::exec()
{
    for(;;) {
        sensor.process();
    }
}

template<> void SensorHandler::exec()
{
    for(;;) {
        sensor.processIMU();
    }
}
} // namespace OS

INTERRUPT_HANDLER(GYRO_DataReady_ISR, ITC_IRQ_PORTD)
{
    OS::TISRW ISRW;

    sensor.handleGyroDataReady();
}

INTERRUPT_HANDLER(ACCEL_DataReady_ISR, ITC_IRQ_PORTC)
{
    OS::TISRW ISRW;

    sensor.handleAcelDataReady();
}

//---------------------------------------------------------------------------
INTERRUPT_HANDLER(UART_Rx_ISR, ITC_IRQ_UART1_RX)
{
    OS::TISRW ISRW;

    sensor.handleUartReceive();
}

INTERRUPT_HANDLER(UART_Tx_ISR, ITC_IRQ_UART1_TX)
{
    OS::TISRW ISRW;

    sensor.handleUartTransmit();
}
