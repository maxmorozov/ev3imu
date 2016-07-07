package calibration;

import lejos.robotics.SampleProvider;

/**
 * Extends {@link SampleProvider} interface with scale selection mechanism
 * and ability to write EEPROM
 */
public interface SampleProviderWithScale extends SampleProvider, EepromWriter {
    /**
     * Selects the full-scale range by the index
     *
     * @param scaleNo scale index
     */
    void setScale(int scaleNo);

    /**
     * Writes the 4x3 matrix to the sensor's EEPROM
     *
     * @param scaleNo scale index
     * @param data EEPROM data
     */
    void writeEeprom(int scaleNo, short[] data);
}
