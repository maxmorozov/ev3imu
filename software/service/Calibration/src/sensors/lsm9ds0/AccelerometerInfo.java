package sensors.lsm9ds0;

import calibration.AxisOrientation;
import calibration.SensorInfo;
import lejos.hardware.sensor.imu.ImuEepromWriter;
import lejos.hardware.sensor.imu.ScaleSelector;

/**
 * @author Max Morozov
 */
public class AccelerometerInfo implements SensorInfo {
    public static final double G_ACTUAL = 9.812;
    public static final double G_REFERENCE = 9.80665;
    public static final double G_SCALED = 0x8000 * G_ACTUAL / G_REFERENCE;

    private static final double[] G_LSB = {
            G_SCALED / 2,
            G_SCALED / 4,
            G_SCALED / 6,
            G_SCALED / 8,
            G_SCALED / 24,
    };

    /**
     * Returns number of scale ranges
     *
     * @return number of scale ranges
     */
    @Override
    public int getScaleCount() {
        return G_LSB.length;
    }

    /**
     * Returns axis orientation of the sensor
     *
     * @return axis orientation
     */
    @Override
    public AxisOrientation getAxisOrientation() {
        return AxisOrientation.Direct;
    }

    @Override
    public int sampleOffset() {
        return 0;
    }

    @Override
    public void writeEeprom(ImuEepromWriter sensor, int scaleNo, short[] data) {
        sensor.writeAccelerometerEeprom(scaleNo, data);
    }

    @Override
    public void setScale(ScaleSelector sensor, int scaleNo) {
        sensor.setAccelerometerScale(scaleNo);
    }

    @Override
    public double[] getReferenceValues() {
        return G_LSB;
    }
}
