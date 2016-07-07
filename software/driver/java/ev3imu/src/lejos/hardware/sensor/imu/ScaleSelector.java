package lejos.hardware.sensor.imu;

/**
 * @author Max Morozov
 */
public interface ScaleSelector {
    /**
     * Changes the gyroscope full-scale range
     *
     * @param scaleNo scale range index
     * @return true if the current scale range has been successfully changed
     */
    boolean setGyroscopeScale(int scaleNo);

    /**
     * Changes the accelerometer full-scale range
     *
     * @param scaleNo scale range index
     * @return true if the current scale range has been successfully changed
     */
    boolean setAccelerometerScale(int scaleNo);

    /**
     * Changes the magnetometer full-scale range
     *
     * @param scaleNo scale range index
     * @return true if the current scale range has been successfully changed
     */
    boolean setMagnetometerScale(int scaleNo);
}
