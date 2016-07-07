package calibration;

import lejos.hardware.sensor.imu.ImuEepromWriter;
import lejos.hardware.sensor.imu.ScaleSelector;
import lejos.robotics.SampleProvider;

/**
 * Sample provider with ability to change scale and write EEPROM
 */
public class ImuDataProvider<T extends ScaleSelector & ImuEepromWriter> implements SampleProviderWithScale {
    private final SampleProvider sampleProvider;
    private final T sensor;
    private final SensorInfo deviceInfo;
    private float[] sample;

    public ImuDataProvider(SensorInfo deviceInfo, SampleProvider sampleProvider, T sensor) {
        this.deviceInfo = deviceInfo;
        this.sampleProvider = sampleProvider;
        this.sensor = sensor;
        sample = new float[sampleProvider.sampleSize()];
    }

    @Override
    public void setScale(int scaleNo) {
        deviceInfo.setScale(sensor, scaleNo);
    }

    @Override
    public void writeEeprom(int scaleNo, short[] data) {
        deviceInfo.writeEeprom(sensor, scaleNo, data);
    }

    /**
     * Returns the number of elements in a sample.<br>
     * The number of elements does not change during runtime.
     *
     * @return the number of elements in a sample
     */
    @Override
    public int sampleSize() {
        return 3;
    }

    /**
     * Fetches a sample from a sensor or filter.
     *
     * @param sample The array to store the sample in.
     * @param offset
     */
    @Override
    public void fetchSample(float[] sample, int offset) {
        sampleProvider.fetchSample(this.sample, 0);
        System.arraycopy(this.sample, deviceInfo.sampleOffset(), sample, offset, 3);
    }
}
