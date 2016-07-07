import calibration.*;
import calibration.datasource.DataSource;
import calibration.datasource.FileDataSource;
import calibration.datasource.MeasurementDataSource;
import calibration.datasource.SavingDataDecorator;
import calibration.system.Delay;
import calibration.system.SystemDelay;
import calibration.ui.*;
import lejos.hardware.Button;
import lejos.hardware.Key;
import lejos.hardware.KeyListener;
import lejos.hardware.lcd.LCD;
import lejos.hardware.port.SensorPort;
import lejos.hardware.sensor.UARTSensor;
import lejos.hardware.sensor.imu.ImuEepromWriter;
import lejos.hardware.sensor.imu.ImuLsm6ds3;
import lejos.hardware.sensor.imu.ImuLsm9ds0;
import lejos.hardware.sensor.imu.ScaleSelector;
import lejos.robotics.SampleProvider;
import lejos.utility.TextMenu;

import java.io.IOException;

public class Calibration {
    private static Display display = new EV3Display();
    private static Sound sound = new EV3Sound();
    private static Buttons buttons = new EV3Buttons();
    private static Delay delay = new SystemDelay();

    private static SensorInfo[] sensorsLsm6ds3 = {
            new sensors.lsm6ds3.AccelerometerInfo(),
            new sensors.lsm6ds3.GyroscopeInfo()
    };

    private static SensorInfo[] sensorsLsm9ds0 = {
            new sensors.lsm9ds0.AccelerometerInfo(),
            new sensors.lsm9ds0.GyroscopeInfo()
    };

    public static void main(String[] args) throws IOException {
        switch (getType()) {
            case 0:
                calibrateLsm6ds3();
                break;
            case 1:
                calibrateLsm9ds0();
        }
    }

    public static void calibrateLsm6ds3() throws IOException {
        try (ImuLsm6ds3 sensor = new ImuLsm6ds3(SensorPort.S1, true)) {
            calibrateSensor(sensor, sensor.getCombinedMode(), sensorsLsm6ds3);
        }
    }


    private static void calibrateLsm9ds0() throws IOException {
        try (ImuLsm9ds0 sensor = new ImuLsm9ds0(SensorPort.S1, true)) {
            calibrateSensor(sensor, sensor.getCombinedMode(), sensorsLsm9ds0);
        }
    }

    private static <T extends UARTSensor & ScaleSelector & ImuEepromWriter>
    void calibrateSensor(final T sensor, SampleProvider provider, SensorInfo[] sensorInfos) throws IOException {
        Button.ESCAPE.addKeyListener(new KeyListener() {
            @Override
            public void keyPressed(Key k) {
                sensor.close();
                System.exit(1);
            }

            @Override
            public void keyReleased(Key k) {

            }
        });

        SensorInfo sensorInfo = getDevice(sensorInfos);
        if (sensorInfo != null) {
            //Measured data source
            DataSource dataSource = getMode(sensorInfo.getScaleCount(), new ImuDataProvider<>(sensorInfo, provider, sensor));
            //Measured data writer
            DataSource dataWriter = new SavingDataDecorator(display, dataSource, "w[%d].txt");
            //Calibration implementation
            CalibrationSixPoints calibration = new CalibrationSixPoints(sensorInfo, dataWriter, display);
            EepromWriter eepromWriter = new DeviceEepromWriter<>(sensorInfo, sensor);

            calibration.start(eepromWriter);
        }
    }

    /**
     * Selects the appropriate navigator
     *
     * @return navigator instance bn
     */
    private static SensorInfo getDevice(SensorInfo[] sensors) {
        String[] navigatorItems = {"Acceleromter", "Gyroscope", "Exit"};
        TextMenu main = new TextMenu(navigatorItems, 1, "Device");
        LCD.clear();
        int selection = main.select();
        LCD.clear();

        switch (selection) {
            case 0:
                return sensors[0];
            case 1:
                return sensors[1];
        }
        return null;
    }

    private static DataSource getMode(int scaleCount, SampleProviderWithScale sampleProvider) {
        String[] navigatorItems = {"Measurement", "Files", "Exit"};
        TextMenu main = new TextMenu(navigatorItems, 1, "Data source");
        LCD.clear();
        int selection = main.select();
        LCD.clear();

        switch (selection) {
            case 0:
                return new MeasurementDataSource(display, buttons, sound, delay, scaleCount, sampleProvider);
            case 1:
                return new FileDataSource(display, scaleCount, "w[%d].txt");
        }
        return null;
    }

    private static int getType() {
        String[] navigatorItems = {"LSM6DS3", "LSM9DS0", "Exit"};
        TextMenu main = new TextMenu(navigatorItems, 1, "Sensor");
        LCD.clear();
        int selection = main.select();
        LCD.clear();

        return selection;
    }
}