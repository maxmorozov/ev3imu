package calibration.system;

/**
 * Interface to system delays
 */
public interface Delay {
    /**
     * Wait for the specified number of milliseconds.
     * Delays the current thread for the specified period of time. Can not
     * be interrupted (but it does preserve the interrupted state).
     *
     * @param period time to wait in ms
     */
    void msDelay(long period);
}
