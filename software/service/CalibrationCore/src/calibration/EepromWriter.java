package calibration;

/**
 * Interface to write EEPROM to IMU sensor
 */
public interface EepromWriter {
    /**
     * Writes the 4x3 matrix to the sensor's EEPROM
     *
     * @param scaleNo scale index
     * @param data EEPROM data
     */
    void writeEeprom(int scaleNo, short[] data);
}
