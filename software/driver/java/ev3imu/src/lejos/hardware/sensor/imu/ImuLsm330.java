package lejos.hardware.sensor.imu;

import lejos.hardware.port.Port;
import lejos.hardware.sensor.SensorMode;
import lejos.hardware.sensor.UARTSensor;
import lejos.robotics.SampleProvider;
import lejos.utility.Delay;

public class ImuLsm330 extends UARTSensor implements ScaleSelector {
    private static final long SWITCHDELAY = 200;
    private static final long SCALE_SWITCH_DELAY = 10;

    public static final byte DEVICE_RESET  = 0x11;

    //Accelerometer sensitivity
    public static final byte ACC_SCALE_2G = 0x20;
    public static final byte ACC_SCALE_4G = 0x21;
    public static final byte ACC_SCALE_8G = 0x22;
    public static final byte ACC_SCALE_16G = 0x23;

    //Gyroscope sensitivity
    public static final byte GYRO_SCALE_250DPS = 0x30;
    public static final byte GYRO_SCALE_500DPS = 0x31;
    public static final byte GYRO_SCALE_2000DPS = 0x32;

    private static final int ACCEL_SCALE = Short.MAX_VALUE + 1;

    private static final float[] gyroScale = {8.75e-3f, 17.5e-3f, 70e-3f};//in degree per second / digit
    private static final float[] accelScale = {2f / ACCEL_SCALE, 4f / ACCEL_SCALE, 8f / ACCEL_SCALE , 24f / ACCEL_SCALE}; //in g / digit
    private boolean rawMode;

    public ImuLsm330(Port port) {
        this(port, false);
    }

    public ImuLsm330(Port port, boolean rawMode) {
        super(port);
        setModes(new SensorMode[]{new CombinedMode(), new AccelerationMode(), new GyroMode()});
        this.rawMode = rawMode;
    }

    public void reset() {
        byte[] buffer = new byte[] {DEVICE_RESET};
        port.write(buffer, 0, buffer.length);
    }

    public boolean setGyroscopeScale(int scaleNo) {
        if (scaleNo >= 0 && scaleNo < 3) {
            ImuSensorMode mode = (ImuSensorMode) getMode(getCurrentMode());
            mode.setGyroScale(scaleNo);
            return setScale(GYRO_SCALE_250DPS + scaleNo);
        }
        return false;
    }

    public boolean setAccelerometerScale(int scaleNo) {
        if (scaleNo >=0 && scaleNo < 4) {
            ImuSensorMode mode = (ImuSensorMode)getMode(getCurrentMode());
            mode.setAccelScale(scaleNo);
            setScale(ACC_SCALE_2G + scaleNo);
        }
        return false;
    }

    @Override
    public boolean setMagnetometerScale(int scaleNo) {
        return false;
    }

    private boolean setScale(int scaleCommand) {
        byte[] buffer = new byte[] {(byte)scaleCommand};
        boolean success = port.write(buffer, 0, buffer.length) == buffer.length;
        if (success) {
            Delay.msDelay(SCALE_SWITCH_DELAY);
        }
        return success;
    }

    private float getAccelScale(int scaleNo) {
        if (rawMode)
            return 1;
        else
            return accelScale[scaleNo];
    }

    private float getGyroScale(int scaleNo) {
        if (rawMode)
            return 1;
        else
            return gyroScale[scaleNo];
    }

    public SensorMode getCombinedMode() {
        return getMode(0);
    }

    public SensorMode getAccelerationMode() {
        return getMode(1);
    }

    public SensorMode getGyroMode() {
        return getMode(2);
    }


    private class CombinedMode extends BaseSensorMode {
        @Override
        public int sampleSize() {
            return 6;
        }

        @Override
        public String getName() {
            return "ALL";
        }

        @Override
        public int getMode() {
            return 0;
        }

        @Override
        public void setGyroScale(float scale) {
            for (int i = 3; i < sampleSize(); ++i) {
                this.scale[i] = scale;
            }
        }

        @Override
        public void setAccelScale(float scale) {
            for (int i = 0; i < 3; ++i) {
                this.scale[i] = scale;
            }
        }
    }

    private class AccelerationMode extends BaseSensorMode {
        @Override
        public int sampleSize() {
            return 3;
        }

        @Override
        public String getName() {
            return "Acceleration";
        }

        @Override
        public int getMode() {
            return 1;
        }

        @Override
        public void setAccelScale(float scale) {
            for (int i = 0; i < sampleSize(); ++i) {
                this.scale[i] = scale;
            }
        }
    }

    private class GyroMode extends BaseSensorMode {
        @Override
        public int sampleSize() {
            return 3;
        }

        @Override
        public String getName() {
            return "Rate";
        }

        @Override
        public int getMode() {
            return 2;
        }

        @Override
        public void setGyroScale(float scale) {
            for (int i = 0; i < sampleSize(); ++i) {
                this.scale[i] = scale;
            }
        }
    }

    abstract class BaseSensorMode implements ImuSensorMode {
        protected float[] scale;
        private short[] buffer;

        public BaseSensorMode() {
            this.scale = new float[sampleSize()];
            this.buffer = new short[sampleSize()];
            setAccelScale(getAccelScale(0));
            setGyroScale(getGyroScale(0));
        }

        /**
         * Fetches a sample from a sensor or filter.
         *
         * @param sample The array to store the sample in.
         * @param offset
         */
        @Override
        public void fetchSample(float[] sample, int offset) {
            switchMode(getMode(), SWITCHDELAY);
            port.getShorts(buffer, 0, sampleSize());
            for(int i=0;i<sampleSize();++i) {
                sample[offset + i] = buffer[i] * scale[i];
            }
        }

        public void setGyroScale(float scale) { }
        public void setAccelScale(float scale) { }
    }

    interface ImuSensorMode extends SensorMode {
        int getMode();

        void setGyroScale(float scale);
        void setAccelScale(float scale);
    }
}