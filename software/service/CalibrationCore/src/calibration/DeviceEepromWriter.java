package calibration;

import lejos.hardware.sensor.imu.ImuEepromWriter;

/**
 * Provides a way to write EEPROM to device
 */
public class DeviceEepromWriter<T extends ImuEepromWriter> implements EepromWriter {
    private final SensorInfo deviceInfo;
    private final T sensor;

    public DeviceEepromWriter(SensorInfo deviceInfo, T sensor) {
        this.deviceInfo = deviceInfo;
        this.sensor = sensor;
    }

    /**
     * Writes the 4x3 matrix to the sensor's EEPROM
     *
     * @param scaleNo scale index
     * @param data    EEPROM data
     */
    @Override
    public void writeEeprom(int scaleNo, short[] data) {
        deviceInfo.writeEeprom(sensor, scaleNo, data);
    }
}
