package calibration;

import lejos.hardware.sensor.imu.ImuEepromWriter;
import lejos.hardware.sensor.imu.ScaleSelector;

/**
 * @author Max Morozov
 */
public interface SensorInfo {
    /**
     * Returns number of scale ranges
     *
     * @return number of scale ranges
     */
    int getScaleCount();

    /**
     * Returns axis orientation of the sensor
     *
     * @return axis orientation
     */
    AxisOrientation getAxisOrientation();

    /**
     * Returns information about offset of the sensor related data in the sample received from the device
     *
     * @return sensor's sample offset
     */
    int sampleOffset();

    /**
     * Writes the 4x3 matrix to the sensor's EEPROM
     *
     * @param sensor sensor EEPROM writer interface
     * @param scaleNo scale index
     * @param data EEPROM data
     */
    void writeEeprom(ImuEepromWriter sensor, int scaleNo, short[] data);

    /**
     * Selects the full-scale range by the index
     *
     * @param sensor sensor's scale selection interface
     * @param scaleNo scale index
     */
    void setScale(ScaleSelector sensor, int scaleNo);

    /**
     * Returns list of reference values for each sensor full-scale range.
     * The values are in raw sensor's LSB units.
     *
     * @return list of reference values in sensor's LSB
     */
    double[] getReferenceValues();
}
