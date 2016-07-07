package lejos.hardware.sensor.imu;

import lejos.hardware.port.Port;
import lejos.hardware.sensor.SensorMode;
import lejos.hardware.sensor.UARTSensor;
import lejos.robotics.SampleProvider;
import lejos.utility.Delay;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ImuLsm9ds0 extends UARTSensor implements ScaleSelector, ImuEepromWriter {
    private static final long SWITCHDELAY = 200;
    private static final long SCALE_SWITCH_DELAY = 10;

    //Return device to the state right after power on
    public static final byte DEVICE_RESET  = 0x11;

    //Accelerometer sensitivity
    public static final byte ACC_SCALE_2G = 0x20;
    public static final byte ACC_SCALE_4G = 0x21;
    public static final byte ACC_SCALE_6G = 0x22;
    public static final byte ACC_SCALE_8G = 0x23;
    public static final byte ACC_SCALE_16G = 0x24;

    //Gyroscope sensitivity
    public static final byte GYRO_SCALE_245DPS = 0x30;
    public static final byte GYRO_SCALE_500DPS = 0x31;
    public static final byte GYRO_SCALE_2000DPS = 0x32;

    //Magnetometer sensitivity
    public static final byte MAG_SCALE_2GS = 0x40;
    public static final byte MAG_SCALE_4GS = 0x41;
    public static final byte MAG_SCALE_5GS = 0x42;
    public static final byte MAG_SCALE_12GS = 0x43;

    //Write calibration matrix into EEPROM
    public static final byte CALIBRATE_ACC_2G  = 0x50;
    public static final byte CALIBRATE_ACC_4G  = 0x51;
    public static final byte CALIBRATE_ACC_6G  = 0x52;
    public static final byte CALIBRATE_ACC_8G  = 0x53;
    public static final byte CALIBRATE_ACC_16G = 0x54;

    public static final byte CALIBRATE_GYRO_245DPS = 0x60;
    public static final byte CALIBRATE_GYRO_500DPS  = 0x61;
    public static final byte CALIBRATE_GYRO_2000DPS = 0x62;

    public static final byte CALIBRATE_MAG_2GS = 0x70;
    public static final byte CALIBRATE_MAG_4GS = 0x71;
    public static final byte CALIBRATE_MAG_5GS = 0x72;
    public static final byte CALIBRATE_MAG_12GS = 0x73;

    private static final int ACCEL_SCALE = Short.MAX_VALUE + 1;

    private static final float[] gyroScale = {8.75e-3f, 17.5e-3f, 70e-3f};//in degree per second / digit
    private static final float[] accelScale = {2f / ACCEL_SCALE, 4f / ACCEL_SCALE, 6f / ACCEL_SCALE, 8f / ACCEL_SCALE , 24f / ACCEL_SCALE}; //in g / digit
    private static final float[] magScale = {0.08f, 0.16f, 0.32f, 0.48f}; //in mgauss / digit
    private boolean rawMode;

    public ImuLsm9ds0(Port port) {
        this(port, false);
    }

    public ImuLsm9ds0(Port port, boolean rawMode) {
        super(port);
        this.rawMode = rawMode;
        setModes(new SensorMode[]{new CombinedMode(), new AccelerationMode(), new GyroMode(), new MagnetometerMode()});
    }

    public void reset() {
        byte[] buffer = new byte[] {DEVICE_RESET};
        port.write(buffer, 0, buffer.length);
    }

    @Override
    public boolean setGyroscopeScale(int scaleNo) {
        if (scaleNo >= 0 && scaleNo < 3) {
            ImuSensorMode mode = (ImuSensorMode) getMode(getCurrentMode());
            mode.setGyroScale(getGyroScale(scaleNo));
            return setScale(GYRO_SCALE_245DPS + scaleNo);
        }
        return false;
    }

    @Override
    public boolean setAccelerometerScale(int scaleNo) {
        if (scaleNo >=0 && scaleNo < 5) {
            ImuSensorMode mode = (ImuSensorMode)getMode(getCurrentMode());
            mode.setAccelScale(getAccelScale(scaleNo));
            return setScale(ACC_SCALE_2G + scaleNo);
        }
        return false;
    }

    @Override
    public boolean setMagnetometerScale(int scaleNo) {
        if (scaleNo >=0 && scaleNo < 4) {
            ImuSensorMode mode = (ImuSensorMode)getMode(getCurrentMode());
            mode.setMagScale(getMagScale(scaleNo));
            return setScale(MAG_SCALE_2GS + scaleNo);
        }
        return false;
    }

    /**
     * Update accelerometer EEPROM
     *
     * @param scaleNo scale index
     * @param data    EEPROM data (4x3 matrix of 16-bit integers)
     * @return true if the EEPROM data has been successfully written
     */
    @Override
    public boolean writeAccelerometerEeprom(int scaleNo, short[] data) {
        return writeEeprom(CALIBRATE_ACC_2G + scaleNo, data);
    }

    /**
     * Update gyroscope EEPROM
     *
     * @param scaleNo scale index
     * @param data    EEPROM data (4x3 matrix of 16-bit integers)
     * @return true if the EEPROM data has been successfully written
     */
    @Override
    public boolean writeGyroscopeEeprom(int scaleNo, short[] data) {
        return writeEeprom(CALIBRATE_GYRO_245DPS + scaleNo, data);
    }

    /**
     * Update magnetometer EEPROM
     *
     * @param scaleNo scale index
     * @param data    EEPROM data (4x3 matrix of 16-bit integers)
     * @return true if the EEPROM data has been successfully written
     */
    @Override
    public boolean writeMagnetomtereEeprom(int scaleNo, short[] data) {
        return writeEeprom(CALIBRATE_MAG_2GS + scaleNo, data);
    }

    private boolean writeEeprom(int writeCommand, short[] data) {
        ByteBuffer command = ByteBuffer.allocate(data.length * 2 + 1);
        command.order(ByteOrder.LITTLE_ENDIAN);

        command.put((byte) writeCommand);
        for (short aData : data) {
            command.putShort(aData);
        }
        byte[] array = command.array();
        boolean success = port.write(array, 0, array.length) == array.length;
        if (success) {
            //Wait for writing the data to EEPROM
            Delay.msDelay(array.length * 3);
        }
        return success;
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

    private float getMagScale(int scaleNo) {
        if (rawMode)
            return 1;
        else
            return magScale[scaleNo];
    }

    public SensorMode getCombinedMode() {
        return getMode(0);
    }

    public SensorMode getAccelerationMode() {
        return getMode(1);
    }

    public SensorMode getGyroscopeMode() {
        return getMode(2);
    }

    public SensorMode getMagnetometerMode() {
        return getMode(3);
    }

    private class CombinedMode extends BaseSensorMode {
        @Override
        public int sampleSize() {
            return 9;
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
            for (int i = 3; i < 6; ++i) {
                this.scale[i] = scale;
            }
        }

        @Override
        public void setAccelScale(float scale) {
            for (int i = 0; i < 3; ++i) {
                this.scale[i] = scale;
            }
        }

        @Override
        public void setMagScale(float scale) {
            for (int i = 6; i < sampleSize(); ++i) {
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

    private class MagnetometerMode extends BaseSensorMode {
        @Override
        public int sampleSize() {
            return 3;
        }

        @Override
        public String getName() {
            return "Magnetometer";
        }

        @Override
        public int getMode() {
            return 3;
        }

        @Override
        public void setMagScale(float scale) {
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
            setMagScale(getMagScale(0));
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
        public void setMagScale(float scale) { }
    }

    interface ImuSensorMode extends SensorMode {
        int getMode();

        void setGyroScale(float scale);
        void setAccelScale(float scale);
        void setMagScale(float scale);
    }
}