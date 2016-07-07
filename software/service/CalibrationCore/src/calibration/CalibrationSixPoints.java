package calibration;

import calibration.datasource.DataSource;
import calibration.datasource.ReferenceDataSource;
import calibration.ui.Display;
import org.apache.commons.math.linear.RealMatrix;

import java.io.*;
import java.util.List;

/**
 * @author Max Morozov
 */
public class CalibrationSixPoints {
    private final SensorInfo deviceInfo;
    private final DataSource dataSource;
    private final String[] positions;

    private final Display display;

    public static final int SAMPLES_COUNT = 2000;

    private static final String[] directPositions = {
            "Z UP",
            "Z DOWN",
            "Y UP",
            "Y DOWN",
            "X UP",
            "X DOWN",
    };

    //Gyroscope orientations for clockwise rotation of turntable
    private static final String[] reversePositions = {
            "Z DOWN",
            "Z UP",
            "Y DOWN",
            "Y UP",
            "X DOWN",
            "X UP",
    };

    public CalibrationSixPoints(SensorInfo deviceInfo, DataSource dataSource, Display display) {
        this.deviceInfo = deviceInfo;
        this.dataSource = dataSource;
        this.display = display;
        this.positions = deviceInfo.getAxisOrientation() == AxisOrientation.Direct ? directPositions : reversePositions;
    }


    public void start(EepromWriter eepromWriter) throws IOException {
        DataSource referenceDataSource = new ReferenceDataSource(deviceInfo.getScaleCount(), 3, deviceInfo.getReferenceValues(), CalibrationSixPoints::getOrientations);

        calibrate(eepromWriter, deviceInfo.getScaleCount(), dataSource, referenceDataSource, positions);
    }

    private void calibrate(EepromWriter eepromWriter, int scaleCount, DataSource dataSource, DataSource referenceDataSource, String[] positions) throws IOException {
        //Calculation
        List<RealMatrix> Y = referenceDataSource.get(SAMPLES_COUNT, positions);
        List<RealMatrix> w = dataSource.get(SAMPLES_COUNT, positions);

        display.clear();
        display.drawString("Calculating", 1, 1);
        display.drawString("Scale=", 1, 2);

        //Least square method: X = w \ Y
        //X = inv(w_T * w) * w_T * Y
        for (int scale = 0; scale < scaleCount; ++scale) {
            display.drawInt(scale, 2, 7, 2);

            RealMatrix w_scale = w.get(scale);
            RealMatrix Y_scale = Y.get(scale);

            RealMatrix w_T = w_scale.transpose();
            RealMatrix X = w_T.multiply(w_scale).inverse().multiply(w_T).multiply(Y_scale);

            //Write to the file
            saveMatrix(X, String.format("X[%d].txt", scale));

            short[] ratios = calcIntegers(X);

            eepromWriter.writeEeprom(scale, ratios);
        }
    }

    //Calculates 16-bit integer constants to write to sensor's EEPROM
    private static short[] calcIntegers(RealMatrix matrix) {
        short[] result = new short[matrix.getRowDimension() * matrix.getColumnDimension()];
        int index = 0;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                result[index++] = (short) Math.round(matrix.getEntry(row, col) * 0x4000);
            }
        }
        for (int col = 0; col < 3; ++col) {
            result[index++] = (short) Math.round(matrix.getEntry(3, col) / 2);
        }
        return result;
    }

    /**
     * Writes matrix to the disk
     *
     * @param matrix   matrix to write
     * @param fileName file name
     * @throws IOException
     */
    private static void saveMatrix(RealMatrix matrix, String fileName) throws IOException {
        try (OutputStream os = new FileOutputStream(fileName);
             BufferedOutputStream bos = new BufferedOutputStream(os, 64000);
             PrintStream ps = new PrintStream(bos)) {

            for (int row = 0; row < matrix.getRowDimension(); ++row) {
                for (int col = 0; col < matrix.getColumnDimension(); ++col) {
                    ps.print(matrix.getEntry(row, col));
                    ps.print(",");
                }
                ps.print("\n");
            }
            bos.flush();
        }
    }

    private static double[][] getOrientations(double reference) {
        return new double[][]{
                {0, 0, reference},
                {0, 0, -reference},
                {0, reference, 0},
                {0, -reference, 0},
                {reference, 0, 0},
                {-reference, 0, 0},
        };
    }

}
