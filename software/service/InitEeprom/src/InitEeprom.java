import lejos.hardware.lcd.LCD;
import lejos.hardware.port.SensorPort;
import lejos.hardware.sensor.imu.ImuLsm6ds3;
import lejos.hardware.sensor.imu.ImuLsm9ds0;
import lejos.utility.TextMenu;

import java.io.IOException;

public class InitEeprom {
    private static short[] initialEeprom = {
            0x4000, 0, 0,
            0, 0x4000, 0,
            0, 0, 0x4000,
            0, 0, 0
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
            for (int i = 0; i < 4; ++i) {
                sensor.writeAccelerometerEeprom(i, initialEeprom);
            }
            for (int i = 0; i < 5; ++i) {
                sensor.writeGyroscopeEeprom(i, initialEeprom);
            }
        }
    }


    private static void calibrateLsm9ds0() throws IOException {
        try (ImuLsm9ds0 sensor = new ImuLsm9ds0(SensorPort.S1, true)) {
            for (int i = 0; i < 5; ++i) {
                sensor.writeAccelerometerEeprom(i, initialEeprom);
            }
            for (int i = 0; i < 3; ++i) {
                sensor.writeGyroscopeEeprom(i, initialEeprom);
            }
            for (int i = 0; i < 4; ++i) {
                sensor.writeMagnetomtereEeprom(i, initialEeprom);
            }
        }
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