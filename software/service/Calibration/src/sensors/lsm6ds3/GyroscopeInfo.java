package sensors.lsm6ds3;

import calibration.AxisOrientation;
import calibration.SensorInfo;
import lejos.hardware.sensor.imu.ImuEepromWriter;
import lejos.hardware.sensor.imu.ScaleSelector;

/**
 * @author Max Morozov
 */
public class GyroscopeInfo implements SensorInfo {
    public static final double ROTATION_SPEED = 199.951171875;

    private static final double[] ROTATION_LSB = {
            ROTATION_SPEED / 8.75e-3,   //250 DPS
            ROTATION_SPEED / 17.5e-3,   //500DPS
            ROTATION_SPEED / 35e-3,     //1000 DPS
            ROTATION_SPEED / 70e-3      //2000 DPS
    };

    /**
     * Returns number of scale ranges
     *
     * @return number of scale ranges
     */
    @Override
    public int getScaleCount() {
        return ROTATION_LSB.length;
    }

    /**
     * Returns axis orientation of the sensor
     *
     * @return axis orientation
     */
    @Override
    public AxisOrientation getAxisOrientation() {
        return AxisOrientation.Reversed;
    }

    @Override
    public int sampleOffset() {
        return 3;
    }

    @Override
    public void writeEeprom(ImuEepromWriter sensor, int scaleNo, short[] data) {
        sensor.writeGyroscopeEeprom(scaleNo, data);
    }

    @Override
    public void setScale(ScaleSelector sensor, int scaleNo) {
        sensor.setGyroscopeScale(scaleNo);
    }

    @Override
    public double[] getReferenceValues() {
        return ROTATION_LSB;
    }
}
