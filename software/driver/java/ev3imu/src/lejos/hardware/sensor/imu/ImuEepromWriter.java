package lejos.hardware.sensor.imu;

/**
 * @author Max Morozov
 */
public interface ImuEepromWriter {
    /**
     * Update accelerometer EEPROM
     *
     * @param scaleNo scale index
     * @param data EEPROM data (4x3 matrix of 16-bit integers)
     * @return true if the EEPROM data has been successfully written
     */
    boolean writeAccelerometerEeprom(int scaleNo, short[] data);

    /**
     * Update gyroscope EEPROM
     *
     * @param scaleNo scale index
     * @param data EEPROM data (4x3 matrix of 16-bit integers)
     * @return true if the EEPROM data has been successfully written
     */
    boolean writeGyroscopeEeprom(int scaleNo, short[] data);

    /**
     * Update magnetometer EEPROM
     *
     * @param scaleNo scale index
     * @param data EEPROM data (4x3 matrix of 16-bit integers)
     * @return true if the EEPROM data has been successfully written
     */
    boolean writeMagnetomtereEeprom(int scaleNo, short[] data);
}
