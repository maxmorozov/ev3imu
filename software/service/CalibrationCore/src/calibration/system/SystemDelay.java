package calibration.system;

/**
 * System delay
 */
public class SystemDelay implements Delay {
    /**
     * Wait for the specified number of milliseconds.
     * Delays the current thread for the specified period of time. Can not
     * be interrupted (but it does preserve the interrupted state).
     *
     * @param period time to wait in ms
     */
    @Override
    public void msDelay(long period) {
        lejos.utility.Delay.msDelay(period);
    }
}
