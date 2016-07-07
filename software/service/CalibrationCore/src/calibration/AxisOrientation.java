package calibration;

/**
 * Describes axis orientation of the sensor.
 * It is useful for gyroscope calibration, when the calibration turntable rotates clockwise, but sensor produces
 * positive values for counterclockwise rotation.
 */
public enum AxisOrientation {
    Direct,
    Reversed
}
