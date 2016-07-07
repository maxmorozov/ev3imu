package calibration.datasource;

import calibration.ui.Display;
import org.apache.commons.math.linear.Array2DRowRealMatrix;
import org.apache.commons.math.linear.RealMatrix;

import java.io.IOException;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;

/**
 * Data provider that reads the measurement results from a file.
 * The file contains comma separated values. Each line contains 4 numbers.
 */
public class FileDataSource implements DataSource {
    private final Display display;
    private final int scaleCount;
    private final String fileTemplate;

    public FileDataSource(Display display, int scaleCount, String fileTemplate) {
        this.display = display;
        this.scaleCount = scaleCount;
        this.fileTemplate = fileTemplate;
    }

    @Override
    public List<RealMatrix> get(int sampleCount, String[] positions) throws IOException {
        display.clear();
        display.drawString("Loading", 1, 1);
        display.drawString("Scale=", 1, 2);

        List<RealMatrix> w = new ArrayList<>();

        //Fill the EEPROM with initial values to measure raw data
        for (int scale = 0; scale < scaleCount; ++scale) {
            display.drawInt(scale, 2, 7, 2);

            w.add(readMatrix(positions.length * sampleCount, 4, String.format(fileTemplate, scale)));
        }

        return w;
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
    private static RealMatrix readMatrix(int rows, int columns, String fileName) throws IOException {
        RealMatrix w = new Array2DRowRealMatrix(rows, columns);
        final int[] rowCounter = {0};
        Files.lines(FileSystems.getDefault().getPath(fileName)).forEach((String line) -> {
            String[] parts = line.split("\\,");
            for (int i = 0; i < columns; ++i) {
                w.setEntry(rowCounter[0], i, Double.valueOf(parts[i]));
            }
            ++rowCounter[0];
        });
        return w;
    }

}
