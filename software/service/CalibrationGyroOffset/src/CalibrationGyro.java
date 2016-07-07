import deviation.DeviationCalc3D;
import deviation.FloatValue;
import lejos.hardware.Button;
import lejos.hardware.Key;
import lejos.hardware.KeyListener;
import lejos.hardware.lcd.LCD;
import lejos.hardware.port.SensorPort;
import lejos.hardware.sensor.imu.ImuLsm6ds3;
import lejos.robotics.SampleProvider;
import lejos.utility.Delay;
import lejos.utility.TextMenu;

import java.io.*;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;

/**
 * Calibrates gyroscope in steady state to correct offset error
 */
public class CalibrationGyro {
    public static final int SAMPLES_COUNT = 2000;

    enum DataSource {
        Measurement,
        File,
        None
    }

    private static short[] initialEeprom = {
            0x4000, 0, 0,
            0, 0x4000, 0,
            0, 0, 0x4000,
            0, 0, 0
    };

    //EEPROM for mode 125dps is recalculated from correction matrix for mode 250dps
    private static short[] eeprom = {
            16234,	204,	429,
            -154,	16534,	62,
            143,	-139,	16333,
            -60,	747,	358

    };

    private static short[] zeros = {
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0
    };

    public static void main(String[] args) throws IOException {
        //EV3IMU sensor = new EV3IMU(SensorPort.S1);
        try (ImuLsm6ds3 sensor = new ImuLsm6ds3(SensorPort.S1, true)) {
            SampleProvider combined = sensor.getCombinedMode();
            float[] sample = new float[combined.sampleSize()];

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

            //sensor.writeGyroEeprom(4, initialEeprom);
            //sensor.writeGyroEeprom(4, eeprom);
            //sensor.writeGyroEeprom(4, zeros);

            DataSource dataSource = getMode();
            if (dataSource != DataSource.None) {
                calibrate(sensor, dataSource);
            }
        }
    }

    private static void calibrate(ImuLsm6ds3 sensor, DataSource dataSource) throws IOException {
        List<int[][]> w;
        if (dataSource == DataSource.Measurement) {
            w = measureData(sensor);
        } else {
            w = loadData();
        }

        for (int scale = 0; scale < 5; ++scale) {
            DeviationCalc3D calc = new DeviationCalc3D();
            int[][] measurement = w.get(scale);
            for (int[] sample : measurement) {
                calc.add(sample);
            }
            //Save correction matrix
            FloatValue offset = calc.getAverage();
            saveMatrix(offset, String.format("X[%d].txt", scale));

            //Writing EEPROM
            short[] eeprom = new short[initialEeprom.length];
            System.arraycopy(initialEeprom, 0, eeprom, 0, 9);

            eeprom[9] = (short) Math.round(-offset.x / 2);
            eeprom[10] = (short) Math.round(-offset.y / 2);
            eeprom[11] = (short) Math.round(-offset.z / 2);

            sensor.writeGyroscopeEeprom(scale, eeprom);
        }
    }

    private static List<int[][]> loadData() throws IOException {
        LCD.clear();
        LCD.drawString("Loading", 1, 1);
        LCD.drawString("Scale=", 1, 2);

        List<int[][]> result = new ArrayList<>();
        for (int scale = 0; scale < 5; ++scale) {
            LCD.drawInt(scale, 2, 7, 2);
            result.add(readMatrix(SAMPLES_COUNT, 3, String.format("w[%d].txt", scale)));
        }
        return result;
    }

    /**
     * Measures SAMPLE_COUNT samples an
     *
     * @param sensor IMU sensor
     * @return list of measured samples by scale ranges
     */
    private static List<int[][]> measureData(ImuLsm6ds3 sensor) throws IOException {
        SampleProvider combined = sensor.getCombinedMode();
        float[] sample = new float[combined.sampleSize()];

        LCD.clear();
        for (int i = 0; i < 10; ++i) {
            LCD.drawInt(10 - i, 11, 1, 2);
            Delay.msDelay(1000);
        }

        LCD.clear();
        LCD.drawString("Measuring", 1, 1);
        LCD.drawString("Scale=", 1, 2);

        //Offset of the gyroscope data in the sample
        final int sampleOffset = 3;
        List<int[][]> result = new ArrayList<>();
        for (int scale = 0; scale < 5; ++scale) {
            LCD.drawInt(scale, 2, 7, 2);

            sensor.writeGyroscopeEeprom(scale, initialEeprom);

            sensor.setGyroscopeScale(scale);

            int[][] measurement = new int[SAMPLES_COUNT][];
            for (int i = 0; i < SAMPLES_COUNT; ++i) {
                combined.fetchSample(sample, 0);
                //save source data
                int[] row = new int[3];
                for (int j = 0; j < row.length; ++j) {
                    row[j] = (int) sample[j + sampleOffset];
                }
                measurement[i] = row;

                Delay.msDelay(3);
            }
            result.add(measurement);
        }

        LCD.clear();
        LCD.drawString("Saving data", 1, 1);
        LCD.drawString("Scale=", 1, 2);
        for (int scale = 0; scale < 5; ++scale) {
            LCD.drawInt(scale, 2, 7, 2);
            saveMatrix(result.get(scale), String.format("w[%d].txt", scale));

        }

        return result;
    }

    /**
     * Writes matrix to the disk
     *
     * @param offset zero offset 3D vector
     * @param fileName file name
     * @throws IOException
     */
    private static void saveMatrix(FloatValue offset, String fileName) throws IOException {
        try (OutputStream os = new FileOutputStream(fileName);
             BufferedOutputStream bos = new BufferedOutputStream(os, 64000);
             PrintStream ps = new PrintStream(bos)) {

            ps.print("1.0, 0.0, 0.0,\n");
            ps.print("0.0, 1.0, 0.0,\n");
            ps.print("0.0, 0.0, 1.0,\n");
            ps.print(-offset.x);
            ps.print(",");
            ps.print(-offset.y);
            ps.print(",");
            ps.print(-offset.z);
            ps.print(",\n");

            bos.flush();
        }
    }

    /**
     * Writes matrix to the disk
     *
     * @param matrix matrix to write
     * @param fileName file name
     * @throws IOException
     */
    private static void saveMatrix(int[][] matrix, String fileName) throws IOException {
        try (OutputStream os = new FileOutputStream(fileName);
             BufferedOutputStream bos = new BufferedOutputStream(os, 64000);
             PrintStream ps = new PrintStream(bos)) {

            for (int row = 0; row < matrix.length; ++row) {
                int[] rowData = matrix[row];
                for (int col = 0; col < rowData.length; ++col) {
                    ps.print(rowData[col]);
                    ps.print(",");
                }
                ps.print("\n");
            }
            bos.flush();
        }
    }

    /**
     * Loads matrix from the disk
     *
     * @param rows number of rows
     * @param columns number of columns
     * @param fileName file name to load CSV data from
     * @return matrix with loaded data
     * @throws IOException
     */
    private static int[][] readMatrix(int rows, int columns, String fileName) throws IOException {
        int[][] w = new int[rows][];
        final int[] rowCounter = {0};
        Files.lines(FileSystems.getDefault().getPath(fileName)).forEach((String line) -> {
            String[] parts = line.split("\\,");
            int[] row = new int[columns];
            w[rowCounter[0]] = row;
            for (int i = 0; i < columns; ++i) {
                row[i] = Short.valueOf(parts[i]);
            }
            ++rowCounter[0];
        });
        return w;
    }

    private static DataSource getMode() {
        String[] navigatorItems = {"Measurement", "Files", "Exit"};
        TextMenu main = new TextMenu(navigatorItems, 1, "Data source");
        LCD.clear();
        int selection = main.select();
        LCD.clear();

        switch (selection) {
            case 0:
                return DataSource.Measurement;
            case 1:
                return DataSource.File;
        }
        return DataSource.None;
    }

}